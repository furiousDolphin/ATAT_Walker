

#ifndef SLIDER_HPP_
#define SLIDER_HPP_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>


#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <memory>
#include <initializer_list>
#include <cmath>
#include <algorithm>

#include "Texture.hpp"
#include "Vector2D.hpp"
#include "Rect.hpp"
#include "EventManager.hpp"
#include "GraphicsManager.hpp"



class Sliders;

class Slider
{
    public:
        friend Sliders;
        Slider(
            Vector2D<int> platform_pos,
            const GraphicsManager& graphics_manager,
            double min_val, 
            double max_val, 
            double init_val, 
            std::function< void(double) > fun);

        void get_val();
    
        void update();
        void render() const;


    private:
        Rect* get_colliding_rect_ptr(Vector2D<int> p);

        class SlideRect : public Rect
        {
            public:
                enum Axis;
                SlideRect(
                    Vector2D<int> center, 
                    Vector2D<int> shape, 
                    SlideRect::Axis axis, 
                    std::function<Vector2D<int>(void)> getter,  
                    std::function<void(int)> setter);

                    void set_pos(const Vector2D<int>& new_top_left) override;
                    Vector2D<int> get_pos() const override; 
                    bool collide_point(const Vector2D<int>& v) const override;

                enum Axis
                { HORIZONTAL, VERTICAL };

            private:
                Axis axis_;
                std::function<Vector2D<int>(void)> getter_;  
                std::function<void(int)> setter_;        
        };  
        
        struct Textures
        {
            const Texture& platform;
            const Texture& marked_button;
            const Texture& unmarked_button;
        } textures_;

        bool marked_;

        struct Params
        {
            double min_val;
            double max_val;
            double cur_val;
        } params_;

        std::function< void(double) > fun_;
        Rect platform_rect_;
        std::unique_ptr<SlideRect> button_rect_ptr_;

        Vector2D<int> bot_pos_;
        Vector2D<int> top_pos_;
        Vector2D<int> cur_pos_;
};


class Sliders
{
    public:

        Sliders(
            const EventManager& event_manager, 
            const GraphicsManager& graphics_manager);

        void add(std::unique_ptr<Slider> slider_ptr);
        void init();
        void update();
        void render() const;
     
    private:
        struct Context
        {
            const EventManager& event_manager;
            const GraphicsManager& graphics_manager;
        } context_;

        class GrabbedState
        {
            public:
                GrabbedState();        
                Slider& operator*() const;
                operator bool() const; 
                void clear(); 
                void set_new(Slider* new_obj_ptr, Rect* new_rect_ptr, Vector2D<int> global_mouse_pos); 
                void update(Vector2D<int> global_mouse_pos);

            private:
                Slider* obj_ptr_;
                Rect* rect_ptr_;
                bool active_;
                Vector2D<int> offset_;
        };

        GrabbedState grabbed_;
        std::list<std::unique_ptr<Slider>> sliders_;

        
};

#endif