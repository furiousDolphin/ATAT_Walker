#ifndef AT_AT_HPP_
#define AT_AT_HPP_

#include "Vector2D.hpp"
#include "FileManagement.hpp"
#include "GraphicsManager.hpp"
#include "Settings.hpp"

inline constexpr double PI = 3.14159265358979323846;
inline constexpr double TOLERANCE = 1e-6;

inline constexpr double to_radians(double degrees) 
{ return degrees * PI / 180.0; }

inline constexpr double to_degrees(double radians) 
{ return radians * 180.0 / PI; }



using MathFunc = std::function<double(double)>;
struct KinematicsProvider
{
    MathFunc get_phi;
    MathFunc get_theta;
    MathFunc get_arc_len;
};


struct LegParameters 
{
    double l1 = 200.0, l2 = 200.0, l3 = 200.0, l4 = 200.0;
    double h = 375.0;
    double phi_zero = 80.0;
    SDL_Point axis = {20, 20};
};

enum class LegType 
{ 
    FRONT_LEFT, 
    FRONT_RIGHT, 
    BACK_LEFT,
    BACK_RIGHT 
}; 

class Leg
{   
    public:
        Leg( const GraphicsManager& graphics_manager double x_init, Vector2D<double> pos, LegType leg_type );
        void update(float dt);
        void render() const;
        void set_speed(double speed);
    private:
        void compute_vectors_for_step();
        void compute_vectors_for_stroke();

        LegType leg_type_;

        struct Vectors
        {
            Vector2D<double> v1;
            Vector2D<double> v2;
            Vector2D<double> v3;
            Vector2D<double> v4;                    
        } vectors_;

        struct Angles
        {
            double angle1;
            double angle2;
            double angle3;                    
        } angles_;

        double x_;
        double distance_;
        double velocity_;
        double speed_;
        Vector2D<double> pos_;
};


struct AT_ATParams
{
    double ellipse_a_parameter = 80.0;
    double ellipse_b_parameter = 36.0;
    double total_arc_len;   
};

class AT_AT
{
    public:
        AT_AT(const GraphicsManager& graphics_manager, const float& dt);
        
        void update();
        void render() const;
        void set_speed( double speed );
    private:
        struct Context
        {
            const GraphicsManager& graphics_manager;
            const float& dt;
        } context_;

        void create_ellipse_data();

        double find_phi_for_x(double x);
        double find_theta_for_x(double x);
        double find_arc_len_for_theta(double theta);

        std::vector<std::pair<double, double>> arc_len_theta_table_;
        std::vector<std::pair<double, double>> ex_ey_table_;
        std::vector<Leg> legs_;      
        AT_ATParams params_;
};


#endif