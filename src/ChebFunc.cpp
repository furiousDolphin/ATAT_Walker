    
#define _USE_MATH_DEFINES

#include <stdexcept>
#include <cmath>
#include <regex>
#include <string>
#include <boost/math/interpolators/makima.hpp>

#include "ChebFunc.hpp"

Eigen::VectorXd ChebNodes(int N, double x1, double x2)
{
    Eigen::VectorXd nodes = ( Eigen::VectorXd::LinSpaced(N+1, 0, N) * (M_PI/N) ).array().cos();
    return ( (x2 - x1)/2 )*nodes.array() + ( (x2 + x1)/2 );
}

Eigen::VectorXd EvaluateCheb( const Eigen::VectorXd& c, const Eigen::VectorXd& t )
{
    int N = c.size() - 1;
    int M = t.size();

    Eigen::VectorXd bk1 = Eigen::VectorXd::Zero(M);
    Eigen::VectorXd bk2 = Eigen::VectorXd::Zero(M);
    
    for (int k = N; k >= 1; k--) 
    {
        Eigen::VectorXd temp = bk1; 
        bk1 = (2.0 * t.array() * bk1.array() - bk2.array() + c(k)).matrix();
        bk2 = temp;
    }

    return (t.array() * bk1.array() - bk2.array() + c(0)).matrix();
}

Eigen::VectorXd ChebCoeffs(const Eigen::VectorXd& f_vals) 
{
    int N = f_vals.size() - 1;
    
    Eigen::VectorXd g_vals(2 * N);

    for (int j = 0; j <= N; j++) 
    { g_vals[j] = f_vals[j]; }

    for (int j = 1; j < N; j++) 
    { g_vals[2 * N - j] = f_vals[j]; }

    Eigen::FFT<double> fft;
    Eigen::VectorXcd freq;
    fft.fwd(freq, g_vals); 

    Eigen::VectorXd coeffs = freq.head(N + 1).real() / static_cast<double>(N);
    
    coeffs(0) /= 2.0;
    coeffs(N) /= 2.0;

    return coeffs;
}

Eigen::MatrixXd DifferentiationOperator(int N)
{
    Eigen::MatrixXd D = Eigen::MatrixXd::Zero(N+1, N+1);

    for ( int i = 0; i < N+1; i++ )
    {
        double factor = (i == 0) ? 1.0 : 2.0;

        for ( int j = i+1; j < N+1; j+=2 )
        { D(i, j) = factor*j; }
    }

    return D;
}

double Clenshaw(const Eigen::VectorXd& c, double t) 
{
    int N = c.size() - 1;
    double bk1 = 0.0, bk2 = 0.0;
    
    for (int k = N; k >= 1; --k) {
        double bk = 2.0 * t * bk1 - bk2 + c(k);
        bk2 = bk1;
        bk1 = bk;
    }
    return t * bk1 - bk2 + c(0);
}   

Eigen::MatrixXd ChebMassMatrix(int N)
{
    Eigen::VectorXd weights( N+1 );
    
    weights.fill(M_PI / 2.0);
    weights[0] = M_PI;

    Eigen::DiagonalMatrix<double, Eigen::Dynamic> M = weights.asDiagonal();

    return M;
}

Eigen::VectorXd InterpolateToChebNodes(
    const Eigen::VectorXd& x_dense, 
    const Eigen::VectorXd& y_dense, 
    const Eigen::VectorXd& cheb_nodes)
{

    std::vector<double> xv(x_dense.data(), x_dense.data() + x_dense.size());
    std::vector<double> yv(y_dense.data(), y_dense.data() + y_dense.size());

    auto spline = boost::math::interpolators::makima<std::vector<double>>(std::move(xv), std::move(yv));

    Eigen::VectorXd y_at_cheb(cheb_nodes.size());
    for (int i = 0; i < cheb_nodes.size(); i++) { 
        double t_x = std::clamp(cheb_nodes(i), x_dense.minCoeff(), x_dense.maxCoeff());
        y_at_cheb(i) = spline(t_x);
    }
    return y_at_cheb;
}
