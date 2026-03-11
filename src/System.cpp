
#include <numbers>

#include "System.hpp"



System::System(
    Eigen::VectorXd b_coeffs, 
    Eigen::VectorXd a_coeffs
) : 
    tf_{b_coeffs, a_coeffs},
    t_{0.0},
    dt_{0.0002}
{

}

void System::TransferFunc::create_M()
{
    int m = b_coeffs.size()-1;
    int n = a_coeffs.size()-1;
    if ( m > n )
    { throw std::runtime_error("m nie moze byc wieksze od n"); }

    M = Eigen::MatrixXd::Zero(n, n);
    if (n > 1)
    { M.block(0, 1, n-1, n-1) = Eigen::MatrixXd::Identity(n-1, n-1); }
    M.row(n-1) = a_coeffs.segment(0, n)*(-1/a_coeffs[n]);    
}

void System::update()
{
    tf_.create_M();
}

Eigen::VectorXd System::step_response(const Eigen::VectorXd& t_dense)
{
    this->update();
    forcing_func_.func = [](double t){return 1.0;};

    int n = tf_.a_coeffs.size() - 1;
    double a_0 = tf_.a_coeffs[0];
    state_ = Eigen::VectorXd::Zero(n);
    //state_[0] = 1.0/a_0; // to daje mi że uklad od poczatku jest w stanie ustalonym

    Eigen::VectorXd Y = Eigen::VectorXd::Zero(t_dense.size());
    for (int i = 1; i < t_dense.size(); i++)
    { 
        auto [_, y] = this->do_RK4_step(t_dense[i]-t_dense[i-1]); 
        Y[i] = y;
    }
    return Y;
}

Eigen::VectorXd System::impulse_response(const Eigen::VectorXd& t_dense)
{
    this->update();
    forcing_func_.func = [](double t){return 0.0;};

    int n = tf_.a_coeffs.size()-1;
    double a_n = tf_.a_coeffs[n];
    state_ = Eigen::VectorXd::Zero(n);
    state_[n-1] = 1.0/a_n;

    Eigen::VectorXd Y = Eigen::VectorXd::Zero(t_dense.size());
    for (int i = 1; i < t_dense.size(); i++)
    { 
        auto [_, y] = this->do_RK4_step(t_dense[i]-t_dense[i-1]); 
        Y[i] = y;
    }
    return Y;
}

void System::set_forcing_func(FuncType func)
{
    std::visit(overloaded{
        [&](std::function<double(void)> void_func)
        {forcing_func_.func = [void_func](double t){return void_func();};},
        [&](std::function<double(double)> double_func)
        {forcing_func_.func = double_func;}
    }, func);

    t_ = 0.0;
    tf_.create_M();
    int N = 16;
    int m = tf_.b_coeffs.size() - 1;
    int n = tf_.a_coeffs.size() - 1;

    Eigen::VectorXd X_derivatives_vect = Eigen::VectorXd::Zero(n + 1); 

    std::visit(overloaded{
        [&](std::function<double(void)> void_func)
        { 
            if (m < N-1) 
            {
                double L = 0.001;
                std::pair<double, double> cheb_range{0.0, L};
                Eigen::VectorXd cheb_nodes = ChebNodes(N, cheb_range.first, cheb_range.second);
                Eigen::VectorXd cheb_vals = cheb_nodes.unaryExpr([&](double t){return void_func();});
                Eigen::VectorXd cheb_coeffs = ChebCoeffs(cheb_vals);
                Eigen::MatrixXd D = DifferentiationOperator(N); 
                Eigen::VectorXd kth_deriv_coeffs = cheb_coeffs;

                for (int i = 0; i < n; i++) 
                {
                    X_derivatives_vect[i] = Clenshaw(kth_deriv_coeffs, cheb_range.first);
                    kth_deriv_coeffs = D * kth_deriv_coeffs; 
                }
            } 
            else 
            { throw std::runtime_error("..."); }
        },
        [&](std::function<double(double)> double_func)
        { X_derivatives_vect[0] = double_func(0.0); }
    }, func);

    Eigen::MatrixXd X_derivatives_matrix = Eigen::MatrixXd::Zero(n, n);
    for (int i = 0; i < n; i++) 
    {
        int len = n - i;
        X_derivatives_matrix.col(i).segment(i, len) = X_derivatives_vect.segment(0, len);
    }

    Eigen::VectorXd Y_derivatives_vect = Eigen::VectorXd::Zero(n);
    if (n > 1) 
    { Y_derivatives_vect[0] = (tf_.b_coeffs[0]/tf_.a_coeffs[0]) * X_derivatives_vect[1]; }

    Eigen::MatrixXd lin_eq_sys_matrix = Eigen::MatrixXd::Zero(n, n);
    Eigen::VectorXd b = Eigen::VectorXd::Zero(n);
    b.segment(0, m+1) = tf_.b_coeffs;

    lin_eq_sys_matrix.row(0) = b.transpose() * tf_.M;
    for (int i = 1; i < n; i++) 
    { lin_eq_sys_matrix.row(i) = lin_eq_sys_matrix.row(i-1) * tf_.M; }

    Eigen::VectorXd out_vect = lin_eq_sys_matrix.col(n-1).transpose() * X_derivatives_matrix;

    out_vect *= (-1.0 / tf_.a_coeffs[n]);
    out_vect += Y_derivatives_vect;

    state_ = lin_eq_sys_matrix.colPivHouseholderQr().solve(out_vect);
}

std::pair<double, double> System::do_RK4_step(double dt)
{
    this->update();
    double remaining_time = dt;

    while (remaining_time >= dt_)
    {
        stepper_.do_step(*this, state_, t_, dt_);
        t_ += dt_;
        remaining_time -= dt_;
    }

    if (remaining_time > 0.0)
    {
        stepper_.do_step(*this, state_, t_, remaining_time);
        t_ += remaining_time;
    }

    double y = 0.0;
    for(int i = 0; i < tf_.a_coeffs.size()-1; ++i) 
    { y += tf_.b_coeffs[i] * state_[i]; }

    return {t_, y};
}

void System::operator()(const Eigen::VectorXd& Z, Eigen::VectorXd& dZdt, double t)
{
    int n = tf_.a_coeffs.size()-1;
    double a_n = tf_.a_coeffs[n];
    double x = forcing_func_(t);

    dZdt = tf_.M*Z;
    dZdt[n-1] += x/a_n;
}

inline double System::ForcingFunc::operator()(double t)
{ return func(t); }


SecondOrderSystem::SecondOrderSystem():
    System(
        (Eigen::VectorXd{2} << 1.0, 0.0).finished(),
        (Eigen::VectorXd{3} << 1.0, 2.0, 1.0).finished())
{

}

void SecondOrderSystem::update()
{
    auto& [zeta_vm, r_vm, f_vm] = params_;

    if 
    ( 
        zeta_vm.check_and_reset_dirty() || 
        r_vm.check_and_reset_dirty() || 
        f_vm.check_and_reset_dirty() 
    )

    {
        double zeta = zeta_vm.get_val();
        double r = r_vm.get_val();
        double f = f_vm.get_val();

        double PI = std::numbers::pi;
        double w = 2*PI*f;

        double k1 = (2*zeta)/w;
        double k2 = 1/(w*w);
        double k3 = (zeta*r)/w;    

        tf_.b_coeffs[0] = 1.0;
        tf_.b_coeffs[1] = k3;

        tf_.a_coeffs[0] = 1.0;
        tf_.a_coeffs[1] = k1; 
        tf_.a_coeffs[2] = k2;

        tf_.create_M();
    }
}

SecondOrderSystem::Params& SecondOrderSystem::get_params()
{ return params_; }


