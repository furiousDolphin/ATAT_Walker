#ifndef AT_AT_HPP_
#define AT_AT_HPP_

#include <Eigen/Dense>

#include "Vector2D.hpp"
#include "FileManagement.hpp"
#include "GraphicsManager.hpp"
#include "System.hpp"
#include "ValueManager.hpp"
#include "Settings.hpp"

inline constexpr double PI = 3.14159265358979323846;
inline constexpr double TOLERANCE = 1e-6;

inline constexpr double to_radians(double degrees) 
{ return degrees * PI / 180.0; }

inline constexpr double to_degrees(double radians) 
{ return radians * 180.0 / PI; }


class KinematicsProvider;
class Legs;

class AT_AT
{
    public:
        AT_AT(
            const GraphicsManager& graphics_manager, 
            const float& dt);
        
        void init();
        void update();
        void render() const;

        class Params
        {
            public:
                Params();

                struct EllipseParams
                {
                    double a;
                    double b;
                    std::size_t N;        
                };

                struct LegsParams
                {
                    double phi_zero;
                    double h;
                    double k;
                    std::array<double, 4> segment_lengths;
                    Vector2D<int> axis;
                };

                const EllipseParams& get_ellipse_params() const;
                const LegsParams& get_leg_params() const;

            private:
                void create_data();

                EllipseParams ellipse_;
                LegsParams legs_;
        };

        struct SpeedInputs
        {
            ValueManager y;
            ValueManager u;
        };

        SpeedInputs& get_speed_inputs();
        SecondOrderSystem::Params& get_sys_inputs();

    private:
        struct Context
        {
            const GraphicsManager& graphics_manager;
            const float& dt;
        } context_;

        Params params_;
        SecondOrderSystem sos_;
        SpeedInputs speed_inputs_;

        std::unique_ptr<KinematicsProvider> kinematics_provider_ptr_;
        std::unique_ptr<Legs> legs_ptr_;   
        
};


class KinematicsProvider;
class Leg
{   
    public:
        enum Type;
        Leg(const AT_AT::Params& params, double x_init, Vector2D<double> pos, Type type);
        void update(
            const KinematicsProvider& kinematics_provider, 
            const AT_AT::Params& params, 
            float dt, 
            const AT_AT::SpeedInputs& speed_inputs);
        void render(const GraphicsManager& graphics_manager, const AT_AT::Params& params) const;

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

    private:
        Type type_;
        Geometry geometry_;

        double x_;
        double distance_;
        double velocity_;
        Vector2D<double> pos_;
};

class KinematicsProvider
{
    public:
        KinematicsProvider(const AT_AT::Params& params);
        Leg::Geometry compute_movement_params(double x, Leg::Type leg_type, Leg::Phase leg_phase) const;
    private:
        double find_phi_for_x(double x) const;
        std::pair<double, Vector2D<double>> find_theta_e_vec_for_x(double x) const;
        double find_arc_len_for_x(double x) const;
        double find_arc_len_for_theta(double theta) const;
        void create_ellipse_data();

        struct Context
        {
            const AT_AT::Params& params;
        } context_;

        std::vector<std::pair<double, Vector2D<double>>> theta_e_vec_table_;
        double total_arc_len_;
};

class Legs
{
    public:
        Legs(
            const GraphicsManager& graphics_manager, 
            const KinematicsProvider& kinematics_provider, 
            const float& dt,
            const AT_AT::SpeedInputs& speed_inputs,
            const AT_AT::Params& params);
        void update();
        void render() const;

    private:
        struct
        {
            const GraphicsManager& graphics_manager;
            const KinematicsProvider& kinematics_provider;
            const float& dt;
            const AT_AT::SpeedInputs& speed_inputs;
            const AT_AT::Params& params;
        } context_;

        std::array<Leg, 4> legs_;
};

#endif