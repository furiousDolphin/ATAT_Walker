#ifndef RECT_HPP_
#define RECT_HPP_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <initializer_list>


#include "Vector2D.hpp"

struct Circle
{   
    int x;
    int y;
    double r;
};

class Rect
{
    public:
        constexpr Rect( int x=0, int y=0, int w=0, int h=0 )
        {
            rect_.x=x;
            rect_.y=y;
            rect_.w=w;
            rect_.h=h;
        }
        Rect( const Vector2D<int>& v, int w=0, int h=0 )
        {
            rect_.x=v.x;
            rect_.y=v.y;
            rect_.w=w;
            rect_.h=h;
        }

        constexpr Rect( const Vector2D<int>& v, const Vector2D<int>& shape )
        {
            rect_.x=v.x;
            rect_.y=v.y;
            rect_.w=shape.x;
            rect_.h=shape.y;
        }

        virtual ~Rect() = default;

        constexpr Rect( Rect&& other ) = default;
        constexpr Rect& operator=( Rect&& other ) = default;

        constexpr Rect( const Rect& other )
        {
            rect_.x = other.get_x();
            rect_.y = other.get_y();
            rect_.w = other.get_w();
            rect_.h = other.get_h();
        }

        constexpr Rect( const SDL_Rect& sdl_rect )
        {
            rect_.x = sdl_rect.x;
            rect_.y = sdl_rect.y;
            rect_.w = sdl_rect.w;
            rect_.h = sdl_rect.h;
        }

        constexpr Rect& operator=( const Rect& other )
        {
            
            rect_.x = other.get_x();
            rect_.y = other.get_y();
            rect_.w = other.get_w();
            rect_.h = other.get_h(); 
            
            return *this;
        }

        constexpr Rect& operator=( const SDL_Rect& sdl_rect )
        {
            rect_.x = sdl_rect.x;
            rect_.y = sdl_rect.y;
            rect_.w = sdl_rect.w;
            rect_.h = sdl_rect.h;

            return *this;
        }

        const SDL_Rect* sdl_rect_ptr() const { return &rect_; } 

        inline void move( int x, int y ) 
        { 
            rect_.x = rect_.x + x;
            rect_.y = rect_.y + y;
        }

        inline void set_pos( int x, int y ) 
        { 
            rect_.x = x;
            rect_.y = y;
        }

        virtual inline Vector2D<int> get_pos() const
        { 
            return { rect_.x, rect_.y };
        }

        inline Vector2D<int> get_size() const
        { return { rect_.w, rect_.h }; }

        inline void move( const Vector2D<int>& v ) 
        { 
            rect_.x = rect_.x + v.x;
            rect_.y = rect_.y + v.y;
        }

        virtual inline void set_pos( const Vector2D<int>& v ) 
        { 
            rect_.x = v.x;
            rect_.y = v.y;
        }

        inline Vector2D<int> get_shape() const { return {rect_.w, rect_.h}; }
        inline void set_shape(const Vector2D<int>& new_shape) 
        {  
            rect_.w = new_shape.x;
            rect_.h = new_shape.y;
        }

        inline int get_right() const { return rect_.x + rect_.w; }
        inline int get_left() const { return rect_.x; }
        inline int get_top() const { return rect_.y; }
        inline int get_bottom() const { return rect_.y + rect_.h; }

        inline void set_right(int x) { rect_.x = x - rect_.w; }
        inline void set_left(int x) { rect_.x = x; }
        inline void set_top(int y) { rect_.y = y; }
        inline void set_bottom(int y) { rect_.y = y - rect_.h; }

        inline int get_centerx() const { return rect_.x + rect_.w/2; }
        inline int get_centery() const { return rect_.y + rect_.h/2; }

        inline int set_centerx(int x) { rect_.x = x - rect_.w/2; }
        inline int set_centery(int y) { rect_.y = y - rect_.h/2; }        

        inline constexpr int get_x() const { return rect_.x; }
        inline constexpr int get_y() const { return rect_.y; }
        inline constexpr int get_w() const { return rect_.w; }
        inline constexpr int get_h() const { return rect_.h; }

        inline constexpr void set_x(int x) { rect_.x = x; }
        inline constexpr void set_y(int y) { rect_.y = y; }
        inline constexpr void set_w(int w) { rect_.w = w; }
        inline constexpr void set_h(int h) { rect_.h = h; }

        virtual bool collide_point(const Vector2D<int>& v) const
        {
            //doMorgan zamiast robić : 
            // auto a = rect_.x < v.x;
            // auto b = v.x < rect_.x + rect_.w;
            // auto c = rect_.y < v.y;
            // auto d = v.y < rect_.y + rect_.h;

            //return a && b && c && d;

            // to robimy z domorgana -> not ( a+b+c+d ) = not a && not b && not c && not d
            // czyli not ( not a + not b + not c + not d ) = a && b && c && d czyli to co chcieliśmy

            // auto not_a = rect_.x >= v.x;
            // auto not_b = v.x >= rect_.x + rect_.w;
            // auto not_c = rect_.y >= v.y;
            // auto not_d = v.y >= rect_.y + rect_.h;

            // return !( not_a || not_b || not_c || not_d );

            return !( rect_.x >= v.x || 
                      v.x >= rect_.x + rect_.w || 
                      rect_.y >= v.y || 
                      v.y >= rect_.y + rect_.h );
        }

        bool collide_circle(const Circle& circle) const
        {
            int nearest_x = std::min(
                rect_.x + rect_.w,
                std::max(circle.x, rect_.x)
            );

            int nearest_y = std::min(
                rect_.y + rect_.h,
                std::max(circle.y, rect_.y)
            );

            int dx = nearest_x - circle.x;
            int dy = nearest_y - circle.y;

            return (dx * dx + dy * dy) <= circle.r * circle.r;
        }

        bool collide_rect(const Rect& other) const
        {
            return !(rect_.x + rect_.w <= other.rect_.x ||
                    other.rect_.x + other.rect_.w <= rect_.x ||
                    rect_.y + rect_.h <= other.rect_.y ||
                    other.rect_.y + other.rect_.h <= rect_.y);
        }

        bool get_rect_intersection(const Rect& other, Rect& result) const 
        {
            int left   = std::max( rect_.x, other.rect_.x );
            int top    = std::max( rect_.y, other.rect_.y );
            int right  = std::min( rect_.x + rect_.w, other.rect_.x + other.rect_.w );
            int bottom = std::min( rect_.y + rect_.h, other.rect_.y + other.rect_.h );

            if ( right <= left || bottom <= top )
            { return false; }

            result.rect_.x = left;
            result.rect_.y = top;
            result.rect_.w = right - left;
            result.rect_.h = bottom - top;

            return true;
        }   


    protected:
        SDL_Rect rect_;     
};



#endif