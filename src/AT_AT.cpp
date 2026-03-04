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


#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/cstdint.hpp>

#include "Texture.hpp"
#include "Rect.hpp"
#include "FileManagement.hpp"
#include "Settings.hpp"
#include "AT_AT.hpp"


using namespace boost::math::quadrature;
using namespace boost::math::tools;


AT_AT::AT_AT(
    const GraphicsManager& graphics_manager, 
    const float& dt
):
    context_{graphics_manager, dt}
{
    this->create_ellipse_data();

    legs_ = { 
        Leg( renderer, textures_,     -1.0 * ellipse_a_parameter_, { WIDTH/2 - 200 + 100, 0 }, LegType::FRONT_LEFT ),
        Leg( renderer, textures_, -1.0/3.0 * ellipse_a_parameter_, { WIDTH/2 - 200 - 100, 0 }, LegType::BACK_LEFT ),
        Leg( renderer, textures_,  1.0/3.0 * ellipse_a_parameter_, { WIDTH/2 - 200 + 100, 0 }, LegType::FRONT_RIGHT ),
        Leg( renderer, textures_,      1.0 * ellipse_a_parameter_, { WIDTH/2 - 200 - 100, 0 }, LegType::BACK_RIGHT ) }; 

}

void AT_AT::set_speed( double speed )
{ expected_speed_ = speed; }

void AT_AT::update( float dt )
{
    for( auto& leg : legs_ )
    { leg.set_speed( sys_.update( expected_speed_, 0.0, dt ) ); }

    for( auto& leg : legs_ )
    { leg.update( dt ); }
}

void AT_AT::render() const
{
    for( const auto& leg : legs_ )
    { leg.render(); }
}

void AT_AT::create_ellipse_data()
{
    auto [ellipse_a_parameter, ellipse_b_parameter, _] = params_;

    params_.total_arc_len = this->find_arc_len_for_theta(180.0);

    std::size_t N = 1440;
    arc_len_theta_table_.reserve(N);
    ex_ey_table_.reserve(N);

    for( double theta = 0.0; theta < 180.0; theta+=180.0/N )
    { arc_len_theta_table_.emplace_back(this->find_arc_len_for_theta(theta), theta); }

    double dx = -(2*ellipse_a_parameter) / (double)N;

    for( int x = ellipse_a_parameter; x > 0; x+=dx )
    {
        double theta = this->find_theta_for_x(x);
        ex_ey_table_.emplace_back(ellipse_a_parameter*std::cos(theta), ellipse_b_parameter*std::sin(theta));
    }
}

double AT_AT::find_arc_len_for_theta(double theta)
{ 
    auto [ellipse_a_parameter, ellipse_b_parameter, _] = params_;
    auto integrand = [&](double t) 
    { return std::sqrt( std::pow(ellipse_a_parameter*std::sin(to_degrees(t)), 2) + 
                        std::pow(ellipse_b_parameter*std::cos(to_degrees(t)), 2) ); };

    boost::math::quadrature::gauss_kronrod<double, 61> integrator;

    return integrator.integrate(integrand, 0.0, to_radians(theta), TOLERANCE);
}

double AT_AT::find_theta_for_x(double x)
{   
    auto [ellipse_a_parameter, ellipse_b_parameter, total_arc_len] = params_;
    double target = (ellipse_a_parameter-x) / (2*ellipse_a_parameter) * total_arc_len;
    target = std::clamp(target, 0.0, total_arc_len);

    const auto arc_len_begin = arc_len_theta_table_[0].cbegin();
    const auto arc_len_end = arc_len_theta_table_[0].cend();

    const auto theta_begin = arc_len_theta_table_[1].begin();


    auto arc_len_it = std::lower_bound(arc_len_begin, arc_len_end, target, []( const auto& arc_len_val, double value ) { return arc_len_val < value; } );

    auto theta_it = theta_begin + ( arc_len_it - arc_len_begin );

    if( arc_len_it == arc_len_begin )
        return *theta_it;
    if( arc_len_it == arc_len_end )
        return *(theta_it - 1);

    double l2, p2;
    double l1, p1;

    l2 = *arc_len_it;
    p2 = *theta_it;
    l1 = *(arc_len_it-1);
    p1 = *(theta_it-1);

    double t = ( target - l1 )/( l2 - l1 );
    return p1 + t*(p2 - p1);
}

double AT_AT::find_phi_for_x(double x)
{
    return (90.0-phi_zero)/params_.ellipse_a_parameter * x + 90.0;
}

Leg::Leg( SDL_Renderer* renderer, std::map< std::string, std::shared_ptr< Texture > >*const  textures, double x, Vector2D<double> pos, LegType leg_type ): textures_{ textures }, leg_type_{ leg_type }
{
    renderer_ = renderer;
    x_ = x;
    distance_ = x + ellipse_a_parameter_;
    pos_ = pos;
    leg_type_ = leg_type;
    speed_ = 20.0;

    k = 220;

    if( leg_type_ == LegType::BACK_LEFT || leg_type_ == LegType::BACK_RIGHT ) k = -k;
}

void Leg::update( float dt )
{
    auto [ellipse_a_parameter, ellipse_b_parameter, total_arc_len] = params_;
    if( x_ <= -4.0*ellipse_a_parameter || x_ >= 4.0*ellipse_a_parameter) x_ = 0;

    else if( (x_ < 3.0*ellipse_a_parameter && x_ > 1.0*ellipse_a_parameter) )
    {
        this->compute_vectors_for_step( -x_ + 2.0*ellipse_a_parameter );
        velocity_ = 3*speed_;
    }

    else if( (x_ > -3.0*ellipse_a_parameter && x_ < -1.0*ellipse_a_parameter) )
    {
        this->compute_vectors_for_step( -x_ -2.0*ellipse_a_parameter_  );
        velocity_ = 3*speed_;
    }
    else if( x_ >= -1.0*ellipse_a_parameter && x_ <= 1.0*ellipse_a_parameter)
    {
        this->compute_vectors_for_stroke( x_ );
        velocity_ = speed_;
    }
    else if( x_ > 3.0*ellipse_a_parameter )
    {
        this->compute_vectors_for_stroke( x_ - 4.0*ellipse_a_parameter );
        velocity_ = speed_;
    }
    else if( x_ < -3.0*ellipse_a_parameter )
    {
        this->compute_vectors_for_stroke( x_ + 4.0*ellipse_a_parameter );
        velocity_ = speed_;
    }

    x_ += velocity_*dt;    
}

void Leg::render() const
{   

    (*textures_).at("AT_AT_leg_limb_1")->render(v1-leg_axis, nullptr, angle1, &leg_axis, SDL_FLIP_NONE);     
    (*textures_).at("AT_AT_leg_limb_2")->render(v2-leg_axis, nullptr, angle2, &leg_axis, SDL_FLIP_NONE);    
    (*textures_).at("AT_AT_leg_limb_3")->render(v3-leg_axis, nullptr, angle3, &leg_axis, SDL_FLIP_NONE);

    

    SDL_SetRenderDrawColor(renderer_, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer_, v2.x_,    v2.y_,    v1.x_,    v1.y_ );
    SDL_RenderDrawLine(renderer_, v3.x_,    v3.y_,    v2.x_,    v2.y_ );
    SDL_RenderDrawLine(renderer_, v4.x_,    v4.y_,    v3.x_,    v3.y_ );
}

double Leg::find_phi_for_x(double x)
{
    return (90.0-phi_zero)/ellipse_a_parameter_ * x + 90.0;
}

void Leg::compute_vectors_for_step( double x )
{



    double gx, gy;
    double ex, ey;

    double sin1, cos1;
    double sin2, cos2;
    double sin3, cos3;

    double A, B, C, D;


    double theta;
    double phi;
    double delta_sqrt;


   
   theta = AT_AT::find_theta_for_x( x );
   phi = find_phi_for_x( x );

    cos3 = std::cos(phi);
    sin3 = std::sin(phi);

    ex = ellipse_a_parameter_*std::cos(theta);
    ey = ellipse_b_parameter_*std::sin(theta);

    gx = ex + l3*cos3 + k;
    gy = ey + l3*sin3 - h;

    A = gx;
    B = gy;



    

    C = ( A*A + B*B + l1*l1 - l2*l2 ) / ( 2*B*l1 );
    D = A / B;


    delta_sqrt = sqrt( D*D - C*C + 1 );

    if( leg_type_ == LegType::BACK_LEFT || leg_type_ == LegType::BACK_RIGHT )
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


    v1 = { ex + k, ey };
    v2 = { l3*cos3 + ex + k , l3*sin3 + ey};
    v3 = { l1*cos1 + l3*cos3 + ex + k, l1*sin1 + l3*sin3 + ey};
    v4 = { l2*cos2 + l1*cos1 + l3*cos3 + ex + k, l2*sin2 + l1*sin1 + l3*sin3 + ey};

    v1 += pos_;
    v2 += pos_;
    v3 += pos_;
    v4 += pos_;

    v1.y_ = -v1.y_ + static_cast<double>(HEIGHT);
    v2.y_ = -v2.y_ + static_cast<double>(HEIGHT);
    v3.y_ = -v3.y_ + static_cast<double>(HEIGHT);
    v4.y_ = -v4.y_ + static_cast<double>(HEIGHT);




    angle1 = -phi;
    angle2 = -std::atan2( sin1, cos1 );
    angle3 = -std::atan2( sin2, cos2 );
}

void AT_AT::Leg::compute_vectors_for_stroke( double x )
{



    double gx, gy;

    double sin1, cos1;
    double sin2, cos2;
    double sin3, cos3;

    double A, B, C, D;


    double theta;
    double phi;
    double delta_sqrt;

    
    theta = AT_AT::find_theta_for_x( x );
    phi = find_phi_for_x( x );

    cos3 = std::cos(phi);
    sin3 = std::sin(phi);


    gx = x + l3*cos3 + k;
    gy = l3*sin3 - h;

    

    A = gx;
    B = gy;
    C = ( A*A + B*B + l1*l1 - l2*l2 ) / ( 2*B*l1 );
    D = A / B;



    delta_sqrt = std::sqrt( D*D - C*C + 1 );


    if( leg_type_ == LegType::BACK_LEFT || leg_type_ == LegType::BACK_RIGHT )
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


    v1 = { x + k, 0 };
    v2 = { l3*cos3 + x + k , l3*sin3};
    v3 = { l1*cos1 + l3*cos3 + x + k, l1*sin1 + l3*sin3};
    v4 = { l2*cos2 + l1*cos1 + l3*cos3 + x + k, l2*sin2 + l1*sin1 + l3*sin3};

    v1 += pos_;
    v2 += pos_;
    v3 += pos_;
    v4 += pos_;

    v1.y_ = -v1.y_ + static_cast<double>(HEIGHT);
    v2.y_ = -v2.y_ + static_cast<double>(HEIGHT);
    v3.y_ = -v3.y_ + static_cast<double>(HEIGHT);
    v4.y_ = -v4.y_ + static_cast<double>(HEIGHT);


    angle1 = -phi;
    angle2 = -std::atan2( sin1, cos1 );
    angle3 = -std::atan2( sin2, cos2 );
}
