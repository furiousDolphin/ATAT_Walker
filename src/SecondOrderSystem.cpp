

#include "SecondOrderSystem.hpp"

SecondOrderSystem::SecondOrderSystem()
{

}

void SecondOrderSystem::operator()(const Eigen::VectorXd& X, Eigen::VectorXd& dXdt, double dt)
{
    double v = X[0];
    double v_prim = X[1];
    //x'a moge podawac do konstruktora jako ptr, albo napisać setter który by ustawiał ten wskaźnik

    dXdt[0] = v_prim;

}

void SecondOrderSystem::set_r(double new_r)
{ 
    tf_params_.r = new_r;
    this->reset_ode_coeffs();
 }
void SecondOrderSystem::set_zeta(double new_zeta)
{ 
    tf_params_.zeta = new_zeta;
    this->reset_ode_coeffs();
 }
void SecondOrderSystem::set_f(double new_f)
{ 
    tf_params_.f = new_f;
    this->reset_ode_coeffs();
 }