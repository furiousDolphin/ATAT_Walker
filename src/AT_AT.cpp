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
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <filesystem>
namespace fs = std::filesystem;

#include "Texture.hpp"
#include "Rect.hpp"
#include "Settings.hpp"
#include "ChebFunc.hpp"
#include "AT_AT.hpp"


AT_AT::AT_AT(
    const GraphicsManager& graphics_manager, 
    const float& dt,
    const std::string& base_path
) :
    context_{graphics_manager, dt},
    params_{base_path},
    kinematics_provider_ptr_{std::make_unique<KinematicsProvider>(params_)},
    speed_inputs_{},
    legs_ptr_{std::make_unique<Legs>(graphics_manager, *(kinematics_provider_ptr_.get()), dt, speed_inputs_, params_)},
    sos_{}
{

}

void AT_AT::init()
{ sos_.set_forcing_func(speed_inputs_.u.getter); }

void AT_AT::update()
{ 
    //legs_ptr_->update();
    auto [t, y] = sos_.do_RK4_step(context_.dt);
    speed_inputs_.y.set_val(y);
}

void AT_AT::render() const
{ legs_ptr_->render(); }

AT_AT::Params::Params(const std::string& base_path)
{ 
    this->create_data(base_path);
}

const AT_AT::Params::EllipseParams& AT_AT::Params::get_ellipse_params() const
{ return ellipse_; }
const AT_AT::Params::LegsParams& AT_AT::Params::get_leg_params() const
{ return legs_; }

void AT_AT::Params::create_data(const std::string& base_path)
{
    fs::path full_file_path{base_path + "/params.json"};
    std::ifstream file{full_file_path};
    if (!file.is_open())
    { throw std::runtime_error("nie udalo sie otworzyc json w AT_AT::Params::create_data()");}

    nlohmann::json j;
    file >> j;

    auto safe_get_string = 
    [](
        const nlohmann::json& object, 
        const std::string& key, 
        const std::string default_value
    )
    {
        if ( object.contains(key) && !object[key].is_null() )
        { return object[key].get<std::string>(); }
        return default_value;
    };

    auto& j_ellipse = j["ellipse"];
    ellipse_.a = j_ellipse.value("a", 80.0);
    ellipse_.b = j_ellipse.value("b", 36.0);
    ellipse_.N = j_ellipse.value("N", 1440);

    auto& j_leg = j["leg"];
    legs_.phi_zero = j_leg.value("phi_zero", 80.0);
    legs_.h = j_leg.value("h", 375.0);
    legs_.k = j_leg.value("k", 220.0);

    if (j_leg.contains("axis")) {
        legs_.axis.x = j_leg["axis"].value("x", 20);
        legs_.axis.y = j_leg["axis"].value("y", 20);
    }

    if (j_leg.contains("segment_lengths")) {
        auto& j_seg = j_leg["segment_lengths"];
        legs_.segment_lengths[0] = j_seg.value("l1", 200.0);
        legs_.segment_lengths[1] = j_seg.value("l2", 200.0);
        legs_.segment_lengths[2] = j_seg.value("l3", 200.0);
        legs_.segment_lengths[3] = j_seg.value("l4", 200.0);
    }
}     

AT_AT::SpeedInputs& AT_AT::get_speed_inputs()
{ return speed_inputs_; }

SecondOrderSystem::Params& AT_AT::get_sys_inputs()
{ return sos_.get_params(); }

KinematicsProvider::KinematicsProvider(const AT_AT::Params& params) :
    context_{params}
{

}

double KinematicsProvider::find_phi_for_x(double x) const
{ 
    double PI = std::numbers::pi;
    const auto& params = context_.params;
    double phi_zero = params.get_leg_params().phi_zero;
    double a = params.get_ellipse_params().a;
    return (PI/2-phi_zero)/a * x + PI/2; 
}

std::pair<double, Vector2D<double>> KinematicsProvider::find_theta_e_vec_for_x(double x) const
{
    const auto& [e_a, e_b, N] = context_.params.get_ellipse_params();
    double arc_len = this->find_arc_len_for_x(x);
    
    std::size_t idx = std::min<std::size_t>(N - 1, static_cast<std::size_t>(N * arc_len / total_arc_len_));
    return theta_e_vec_table_[idx];
}

double KinematicsProvider::find_arc_len_for_x(double x) const
{
    //e_a, e_b
    //(x = e_a -> arc_len = 0) & (x = -e_a -> arc_len = total_arc_len)
    //A*e_a    + B = 0
    //A*(-e_a) + B = total_arc_len
    //B = total_arc_len/2 & A = (-1)*total_arc_len/(2*e_a)

    const auto& [e_a, e_b, N] = context_.params.get_ellipse_params();
    if (std::abs(x) >= e_a )
    { throw std::runtime_error("podany x nie nalezy do przedzialu [-e_a, e_a]"); }
    double A = (-1.0)*total_arc_len_/(2*e_a);
    double B = total_arc_len_/2;
    return A*x+B;
}

double KinematicsProvider::find_arc_len_for_theta(double theta) const
{
    const auto& [e_a, e_b, N] = context_.params.get_ellipse_params();
    auto integrand = [&](double t) 
    { return std::sqrt( std::pow(e_a*std::sin(t), 2) + 
                        std::pow(e_b*std::cos(t), 2) ); };

    boost::math::quadrature::gauss_kronrod<double, 61> integrator;

    return integrator.integrate(integrand, 0.0, theta, TOLERANCE);
}

void KinematicsProvider::create_ellipse_data()
{
    auto& [e_a, e_b, N] = context_.params.get_ellipse_params();

    std::pair<double, double> theta_range{0, std::numbers::pi};
    total_arc_len_ = this->find_arc_len_for_theta(theta_range.second);
    
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

Leg::Geometry KinematicsProvider::compute_movement_params(double x, Leg::Type leg_type, Leg::Phase leg_phase) const
{
    double gx, gy;

    double sin1, cos1;
    double sin2, cos2;
    double sin3, cos3;

    double A, B, C, D;

    const auto& params = context_.params;

    const auto& [e_a, e_b, N] = params.get_ellipse_params();
    const auto& [phi_zero, h, _, segment_lengths, axis] = params.get_leg_params();
    double k = (leg_type == Leg::BACK_LEFT || leg_type == Leg::BACK_RIGHT ) 
        ? -params.get_leg_params().k 
        : params.get_leg_params().k;
    auto [l1, l2, l3, l4] = segment_lengths;
    auto [theta, e_vec] = this->find_theta_e_vec_for_x(x);
    auto [ex, ey] = e_vec;

    double phi = this->find_phi_for_x(x);

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
    const AT_AT::Params& params,
    double x_init, 
    Vector2D<double> pos, 
    Type type 
) :
    x_{x_init},
    pos_{pos},
    type_{type},
    distance_{x_ + params.get_ellipse_params().a}
{
  
}

void Leg::update(
    const KinematicsProvider& kinematics_provider, 
    const AT_AT::Params& params, 
    float dt, 
    const AT_AT::SpeedInputs& speed_inputs)
{
    double speed = speed_inputs.y.get_val();
    auto [e_a, e_b, N] = params.get_ellipse_params();

    if( x_ <= -4.0*e_a || x_ >= 4.0*e_a) 
    { x_ = 0;}
    else if( (x_ < 3.0*e_a && x_ > 1.0*e_a) )
    {
        kinematics_provider.compute_movement_params( -x_ + 2.0*e_a, type_, STEP );
        velocity_ = 3*speed;
    }
    else if( (x_ > -3.0*e_a && x_ < -1.0*e_a) )
    {
        kinematics_provider.compute_movement_params( -x_ + 2.0*e_a, type_, STEP );
        velocity_ = 3*speed;
    }
    else if( x_ >= -1.0*e_a && x_ <= 1.0*e_a)
    {
        kinematics_provider.compute_movement_params( x_, type_, STROKE );
        velocity_ = speed;
    }
    else if( x_ > 3.0*e_a )
    {
        kinematics_provider.compute_movement_params( x_ - 4.0*e_a, type_, STROKE );
        velocity_ = speed;
    }
    else if( x_ < -3.0*e_a )
    {
        kinematics_provider.compute_movement_params( x_ - 4.0*e_a, type_, STROKE );
        velocity_ = speed;
    }

    x_ += velocity_*dt;
}

void Leg::render(const GraphicsManager& graphics_manager, const AT_AT::Params& params) const
{
    constexpr std::size_t N = 3;
    std::array<GraphicsManager::SingularTextureKey, N> keys
    {
        GraphicsManager::ATAT_LEG_SEGMENT_1,
        GraphicsManager::ATAT_LEG_SEGMENT_2,
        GraphicsManager::ATAT_LEG_SEGMENT_3
    };

    const auto& legs_params = params.get_leg_params();
    auto double_axis = static_cast<Vector2D<double>>(legs_params.axis);
    auto sdl_axis = static_cast<SDL_Point>(legs_params.axis);    

    for ( std::size_t idx = 0; idx < N; idx++ )
    {
        const Texture& texture = graphics_manager.get_texture(keys[idx]);
        auto pos = static_cast<Vector2D<int>>(geometry_.vectors[idx]-double_axis+pos_);
        texture.render(pos, nullptr, geometry_.angles[idx], &sdl_axis, SDL_FLIP_NONE);
    }
}

Legs::Legs(
    const GraphicsManager& graphics_manager, 
    const KinematicsProvider& kinematics_provider, 
    const float& dt,
    const AT_AT::SpeedInputs& speed_inputs,
    const AT_AT::Params& params
) :
    context_{graphics_manager, kinematics_provider, dt, speed_inputs, params},
    legs_
    {
        Leg{params, -1.0 * params.get_ellipse_params().a, { WIDTH/2 - 200 + 100, 0 }, Leg::Type::FRONT_LEFT},
        Leg{params, -1.0/3.0 * params.get_ellipse_params().a, { WIDTH/2 - 200 - 100, 0 }, Leg::Type::BACK_LEFT},
        Leg{params, 1.0/3.0 * params.get_ellipse_params().a, { WIDTH/2 - 200 + 100, 0 }, Leg::Type::FRONT_RIGHT},
        Leg{params, 1.0 * params.get_ellipse_params().a, { WIDTH/2 - 200 - 100, 0 }, Leg::Type::BACK_RIGHT}         
    }
{
    
}

void Legs::update()
{
    const auto& [graphics_manager, kinematics_provider, dt, speed_inputs, params] = context_;
    for ( auto& leg : legs_ )
    { leg.update(kinematics_provider, params, dt, speed_inputs); }
}

void Legs::render() const
{
    const auto& [graphics_manager, kinematics_provider, dt, speed_inputs, params] = context_;
    for ( auto& leg : legs_ )
    { leg.render(graphics_manager, params); }
}
