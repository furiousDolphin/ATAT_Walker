#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <cmath>
#include <array>
#include <numbers>

#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/cstdint.hpp>

#include "Texture.hpp"
#include "Rect.hpp"
#include "Settings.hpp"
#include "ChebFunc.hpp"
#include "AT_AT.hpp"


using namespace boost::math::quadrature;
using namespace boost::math::tools;


double KinematicsProvider::find_phi_for_x(double x, double phi_zero)
{ 
    double PI = std::numbers::pi;
    return (PI/2-phi_zero)/ellipse_params_.a * x + PI/2; 
}

std::pair<double, Vector2D<double>> KinematicsProvider::find_theta_e_vec_for_x(double x)
{
    auto& [e_a, e_b, N, total_arc_len] = ellipse_params_;
    double arc_len = this->find_arc_len_for_x(x);
    
    std::size_t idx = std::min<std::size_t>(N - 1, static_cast<std::size_t>(N * arc_len / total_arc_len));
    return theta_e_vec_table_[idx];
}

double KinematicsProvider::find_arc_len_for_x(double x)
{
    //e_a, e_b
    //(x = e_a -> arc_len = 0) & (x = -e_a -> arc_len = total_arc_len)
    //A*e_a    + B = 0
    //A*(-e_a) + B = total_arc_len
    //B = total_arc_len/2 & A = (-1)*total_arc_len/(2*e_a)

    auto& [ellipse_a_parameter, ellipse_b_parameter, N, total_arc_len] = ellipse_params_;
    if (std::abs(x) >= ellipse_a_parameter )
    { throw std::runtime_error("podany x nie nalezy do przedzialu [-e_a, e_a]"); }
    double A = (-1.0)*total_arc_len/(2*ellipse_a_parameter);
    double B = total_arc_len/2;
    return A*x+B;
}

double KinematicsProvider::find_arc_len_for_theta(double theta)
{
    auto& [ellipse_a_parameter, ellipse_b_parameter, N, total_arc_length] = ellipse_params_;
    auto integrand = [&](double t) 
    { return std::sqrt( std::pow(ellipse_a_parameter*std::sin(t), 2) + 
                        std::pow(ellipse_b_parameter*std::cos(t), 2) ); };

    boost::math::quadrature::gauss_kronrod<double, 61> integrator;

    return integrator.integrate(integrand, 0.0, theta, TOLERANCE);
}

void KinematicsProvider::create_ellipse_data()
{
    auto& [e_a, e_b, N, total_arc_len] = ellipse_params_;

    std::pair<double, double> theta_range{0, std::numbers::pi};
    total_arc_len = this->find_arc_len_for_theta(theta_range.second);
    
    Eigen::VectorXd cheb_nodes = ChebNodes(64, theta_range.first, theta_range.second);
    Eigen::VectorXd cheb_nodes_vals = cheb_nodes.unaryExpr(
        [this](double theta)
        {return this->find_arc_len_for_theta(theta);});
    Eigen::VectorXd cheb_coeffs = ChebCoeffs(cheb_nodes_vals);

    Eigen::VectorXd X = Eigen::VectorXd::LinSpaced(N, e_a, -e_a);

    theta_e_vec_table_.reserve(N);

    for ( int idx = 0; idx < N; idx++ )
    {
        double x = X[idx];
        double arc_len = this->find_arc_len_for_x(x);
        auto func = [this, &cheb_coeffs, &theta_range, &arc_len](double theta)
        { 
            auto [theta1, theta2] = theta_range;
            double t = ( -2.0/(theta1 - theta2) )*theta + ( (theta1 + theta2)/(theta1 - theta2) );
            return Clenshaw(cheb_coeffs, t) - arc_len; 
        };
        boost::math::tools::eps_tolerance<double> tol(30);
        boost::uintmax_t max_iter = 100;
        auto [theta, iter] = boost::math::tools::toms748_solve(func, theta_range.first, theta_range.second, tol, max_iter);
        theta_e_vec_table_.emplace_back(theta, Vector2D<double>{e_a*std::cos(theta), e_b*std::sin(theta)});
    } 
}

Leg::Geometry KinematicsProvider::compute_movement_params(double x, const Leg::Params& leg_params, Leg::Type leg_type, Leg::Phase leg_phase) const
{
    double gx, gy;
    double ex, ey;

    double sin1, cos1;
    double sin2, cos2;
    double sin3, cos3;

    double A, B, C, D;

    double theta;
    double phi;

    auto& [e_a, e_b, N, total_arc_len] = ellipse_params_;
    auto& [phi_zero, h, k, segment_lengths, axis] = leg_params;
    auto [l1, l2, l3, l4] = segment_lengths;
    auto [theta, e_vec] = this->find_theta_e_vec_for_x(x);
    auto [ex, ey] = e_vec;

    phi = this->find_phi_for_x(x, phi_zero);

    cos3 = std::cos(phi);
    sin3 = std::sin(phi);

    gx = ex + l3*cos3 + k;
    gy = ey + l3*sin3 - h;

    A = gx;
    B = gy;

    C = ( A*A + B*B + l1*l1 - l2*l2 ) / ( 2*B*l1 );
    D = A / B;


    double delta_sqrt = sqrt( D*D - C*C + 1 );

    if( leg_type == Leg::Type::BACK_LEFT || leg_type == Leg::Type::BACK_RIGHT )
    {
        cos1 = ( -C*D - delta_sqrt ) / ( D*D + 1 );
        sin1 = ( -C + D*delta_sqrt ) / ( D*D + 1 );
    }
    else
    {
        cos1 = ( -C*D + delta_sqrt ) / ( D*D + 1 );
        sin1 = ( -C - D*delta_sqrt ) / ( D*D + 1 );      //ten co byl wczesniej
    }

    cos2 = ( -gx - l1*cos1 ) / l2;
    sin2 = ( -gy - l1*sin1) / l2;

    Leg::Geometry leg_geometry;
    auto [v1, v2, v3, v4] = leg_geometry.vectors;
    auto [angle1, angle2, angle3] = leg_geometry.angles;

    switch (leg_phase)
    {
        case Leg::Phase::STROKE:
            v1 = { ex + k, ey };
            break;
        case Leg::Phase::STEP:
            v1 = { x + k, 0.0 };
            break;
        default:
            throw std::runtime_error("podano nieobslugiwany enum w compute_movement_params");
    }

    v2 = { l3*cos3 + v1.x, l3*sin3 + v1.x};
    v3 = { l1*cos1 + v2.x, l1*sin1 + v2.x};
    v4 = { l2*cos2 + v3.x, l2*sin2 + v3.y};

    v1.y = -v1.y + static_cast<double>(HEIGHT);
    v2.y = -v2.y + static_cast<double>(HEIGHT);
    v3.y = -v3.y + static_cast<double>(HEIGHT);
    v4.y = -v4.y + static_cast<double>(HEIGHT);

    angle1 = -phi;
    angle2 = -std::atan2( sin1, cos1 );
    angle3 = -std::atan2( sin2, cos2 );

    return Leg::Geometry{{v1, v2, v3, v4}, {angle1, angle2, angle3}};
}


Leg::Leg(
    double x_init, 
    Vector2D<double> pos, 
    Type type 
) :
    x_{x_init},
    pos_{pos},
    type_{type},
    speed_{20.0}
{
    throw std::runtime_error("dokonczyc konstruktor leg");
    //distance_ = x_ + ellipse_a_parameter_;

    // if( type_ == BACK_LEFT || type_ == BACK_RIGHT ) 
    // { params_.k = -params_.k; }   
}

void Leg::update(const KinematicsProvider& kinematics_provider, float dt)
{
    auto [e_a, e_b, N, total_arc_len] = ellipse_params_;

    if( x_ <= -4.0*e_a || x_ >= 4.0*e_a) 
    { x_ = 0;}
    else if( (x_ < 3.0*e_a && x_ > 1.0*e_a) )
    {
        kinematics_provider.compute_movement_params( -x_ + 2.0*e_a, params_, type_, STEP );
        velocity_ = 3*speed_;
    }
    else if( (x_ > -3.0*e_a && x_ < -1.0*e_a) )
    {
        kinematics_provider.compute_movement_params( -x_ + 2.0*e_a, params_, type_, STEP );
        velocity_ = 3*speed_;
    }
    else if( x_ >= -1.0*e_a && x_ <= 1.0*e_a)
    {
        kinematics_provider.compute_movement_params( x_, params_, type_, STROKE );
        velocity_ = speed_;
    }
    else if( x_ > 3.0*e_a )
    {
        kinematics_provider.compute_movement_params( x_ - 4.0*e_a, params_, type_, STROKE );
        velocity_ = speed_;
    }
    else if( x_ < -3.0*e_a )
    {
        kinematics_provider.compute_movement_params( x_ - 4.0*e_a, params_, type_, STROKE );
        velocity_ = speed_;
    }

    x_ += velocity_*dt;
}

void Leg::render(const GraphicsManager& graphics_manager) const
{
    constexpr std::size_t N = 3;
    std::array<GraphicsManager::SingularTextureKey, N> keys
    {
        GraphicsManager::ATAT_LEG_SEGMENT_1,
        GraphicsManager::ATAT_LEG_SEGMENT_2,
        GraphicsManager::ATAT_LEG_SEGMENT_3
    };

    auto double_axis = static_cast<Vector2D<double>>(params_.axis);
    auto sdl_axis = static_cast<SDL_Point>(params_.axis);    

    for ( std::size_t idx = 0; idx < N; idx++ )
    {
        const Texture& texture = graphics_manager.get_texture(keys[idx]);
        auto pos = static_cast<Vector2D<int>>(geometry_.vectors[idx]-double_axis+pos_);
        texture.render(pos, nullptr, geometry_.angles[idx], &sdl_axis, SDL_FLIP_NONE);
    }
}

void Leg::set_speed(double speed)
{ speed_ = speed; }
