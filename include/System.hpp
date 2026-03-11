
#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <boost/math/tools/roots.hpp>
#include <boost/cstdint.hpp>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>
#include <Eigen/Dense>
#include "ChebFunc.hpp"

#include "Settings.hpp"
#include "ValueManager.hpp"

class System
{
    public:
        System(Eigen::VectorXd b_coeffs, Eigen::VectorXd a_coeffs);

        using FuncType = std::variant<std::function<double(void)>, std::function<double(double)>>;

        Eigen::VectorXd step_response(const Eigen::VectorXd& t_dense);
        Eigen::VectorXd impulse_response(const Eigen::VectorXd& t_dense);

        void set_forcing_func(FuncType func);
        std::pair<double, double> do_RK4_step(double dt = 0.001);
        void operator()(const Eigen::VectorXd& Z, Eigen::VectorXd& dZdt, double t);

    protected:
        void virtual update();

        struct TransferFunc
        {
            Eigen::VectorXd b_coeffs;
            Eigen::VectorXd a_coeffs;
            Eigen::MatrixXd M;
            void create_M();
        } tf_;

        struct ForcingFunc
        {
            std::function<double(double)> func;
            inline double operator()(double t);
        } forcing_func_;

        
        boost::numeric::odeint::runge_kutta4<Eigen::VectorXd> stepper_;
        Eigen::VectorXd state_;
        double t_;
        double dt_;
};


class SecondOrderSystem : public System
{
    public:
        SecondOrderSystem();

        using DoubleSetter = std::function<void(double)>;
        using DoubleGetter = std::function<double(void)>;

        struct Params
        {
            ValueManager zeta;
            ValueManager r;
            ValueManager f;
        };

        Params& get_params();

    private:
        void update() override;
        Params params_;
};

#endif