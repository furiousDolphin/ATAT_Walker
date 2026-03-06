#ifndef MATRIX_BUILDERS_HPP_
#define MATRIX_BUILDERS_HPP_

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>
#include <unsupported/Eigen/FFT>
#include <Eigen/Dense>

#include <vector>
#include <cmath>

using Eigen::MatrixXd;

Eigen::MatrixXd DifferentiationOperator(int N);
Eigen::VectorXd ChebNodes(int N, double x1, double x2);
Eigen::VectorXd EvaluateCheb( const Eigen::VectorXd& c, const Eigen::VectorXd& Xi );
Eigen::VectorXd ChebCoeffs(const Eigen::VectorXd& f_values);
Eigen::MatrixXd ChebMassMatrix(int N);
double Clenshaw(const Eigen::VectorXd& c, double t); 
Eigen::VectorXd InterpolateToChebNodes( const Eigen::VectorXd& x_dense, const Eigen::VectorXd& y_dense, const Eigen::VectorXd& cheb_nodes);

#endif