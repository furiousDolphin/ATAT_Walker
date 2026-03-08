

#ifndef SECOND_ORDER_SYSTEM_HPP_
#define SECOND_ORDER_SYSTEM_HPP_

#include <Eigen/Dense>

class SecondOrderSystem
{
    public:
        SecondOrderSystem();
        void operator()(const Eigen::VectorXd& X, Eigen::VectorXd& dXdt, double dt);
        void set_r(double new_r);
        void set_zeta(double new_zeta);
        void set_f(double new_f);

    private:
        void reset_ode_coeffs();
        struct TransferFunctionParams
        {
            double r;
            double zeta;
            double f;
        
        } tf_params_;

        struct ODECoeffs
        {
            double k1;
            double k2;
            double k3;
        
        } ode_coeffs_;
};

#endif