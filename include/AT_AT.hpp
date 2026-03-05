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

class KinematicsProvider;
class Leg
{   
    public:
        enum Type;
        Leg(double x_init, Vector2D<double> pos, Type type );
        void update(const KinematicsProvider& kinematics_provider, float dt);
        void render(const GraphicsManager& graphics_manager) const;
        void set_speed(double speed);

        enum Type 
        { 
            FRONT_LEFT, 
            FRONT_RIGHT, 
            BACK_LEFT,
            BACK_RIGHT 
        }; 

        enum Phase
        {
            STEP,
            STROKE
        };

        struct Geometry
        {
            std::array<Vector2D<double>, 4> vectors;
            std::array<double, 3> angles;
        };

        struct Params
        {
            double phi_zero;
            double h;
            double k;
            std::array<double, 4> segment_lengths;
            Vector2D<int> axis;
        };

        struct EllipseParams
        {
            double a;
            double b;
            std::size_t N;
            double total_arc_len;
        } ellipse_params_;

    private:
        Type type_;
        Geometry geometry_;
        Params params_;

        double x_;
        double distance_;
        double velocity_;
        double speed_;
        Vector2D<double> pos_;
};

class KinematicsProvider
{
    public:
        KinematicsProvider();

        Leg::Geometry compute_movement_params(double x, const Leg::Params& leg_params, Leg::Type leg_type, Leg::Phase leg_phase) const;
    private:
        double find_phi_for_x(double x, double phi_zero);
        std::pair<double, Vector2D<double>> find_theta_e_vec_for_x(double x);
        double find_arc_len_for_x(double x);
        double find_arc_len_for_theta(double theta);
        void create_ellipse_data();

        struct EllipseParams
        {
            double a;
            double b;
            std::size_t N;
            double total_arc_len;
        } ellipse_params_;

        std::vector<std::pair<double, Vector2D<double>>> theta_e_vec_table_;
};

class AT_AT
{
    public:
        AT_AT(const GraphicsManager& graphics_manager, const KinematicsProvider& kinematics_provider, const float& dt);
        
        void update();
        void render() const;
        void set_speed( double speed );
    private:
        struct Context
        {
            const GraphicsManager& graphics_manager;
            const KinematicsProvider& kinematics_provider;
            const float& dt;
        } context_;

        std::vector<Leg> legs_;      
};


#endif