#ifndef VECTOR2D_HPP_
#define VECTOR2D_HPP_

#include <SDL.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>


template < typename T >
struct Vector2D
{
    constexpr Vector2D( T x, T y ):
        x{x},
        y{y}
    {}

    constexpr Vector2D():
        Vector2D<T>{ 0, 0 }
    {}

    constexpr Vector2D( const SDL_Point& sdl_point ):
        x{ static_cast<T>( sdl_point.x ) },
        y{ static_cast<T>( sdl_point.y ) }
    {}

    template <typename U>
        explicit Vector2D(const Vector2D<U>& other) 
            : x(static_cast<T>(other.x))
            , y(static_cast<T>(other.y)) 
        {}
    

    Vector2D(const Vector2D&) = default;
    Vector2D(Vector2D&&) = default;
    Vector2D& operator=(const Vector2D&) = default;
    Vector2D& operator=(Vector2D&&) = default;
    ~Vector2D() = default;
    constexpr bool operator<(const Vector2D<T>& other) const noexcept   
    {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    constexpr bool operator==( const Vector2D<T>& other ) const noexcept
    {
        return (x == other.x && y == other.y);
    }

    constexpr bool operator!=( const Vector2D<T>& other ) const noexcept
    {
        return !((*this) == other);
    }    

    constexpr Vector2D<T> operator+( const Vector2D<T>& other ) const noexcept
    {
        return Vector2D{ x + other.x,
                         y + other.y };
    }
    constexpr Vector2D<T> operator-( const Vector2D<T>& other ) const noexcept
    {
        return Vector2D{ x - other.x,
                         y - other.y };
    }
    
    constexpr Vector2D<T> operator*( T a ) const noexcept
    {
        return Vector2D{ x*a, y*a };
    }
    constexpr Vector2D<T> operator/( T b ) const
    {
        return Vector2D{ x/b, y/b };
    }

    constexpr Vector2D<T> operator*( const Vector2D<T>& other )
    { return Vector2D<T>{ x*other.x, y*other.y }; }  

    constexpr Vector2D<T>& operator+=( const Vector2D<T>& other ) noexcept
    {
        x += other.x;
        y += other.y;

        return *this;
    }
    constexpr Vector2D<T>& operator-=( const Vector2D<T>& other ) noexcept
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }
    constexpr Vector2D<T>& operator*=( T a ) noexcept
    {
        x*=a;
        y*=a;
        return *this;
    }
    constexpr Vector2D<T>& operator/=( T b )
    {
        x/=b;
        y/=b;
        return *this;
    }

    constexpr Vector2D<T> operator-()
    {
        return Vector2D<T>{ -x, -y };
    }

    template <typename U = T>
    constexpr Vector2D<int> to_grid(U tile_size) const noexcept
    {
        return Vector2D<int>{
            static_cast<int>(x / tile_size) - (x < 0 ? 1 : 0),
            static_cast<int>(y / tile_size) - (y < 0 ? 1 : 0)
        };
    }

    constexpr T dot( const Vector2D<T>& other ) const noexcept
    {
        return  x*other.x + y*other.y;
    }

    operator SDL_Point() const
    {
        return SDL_Point{ static_cast<int>(x),
                          static_cast<int>(y) };
    }

    constexpr T operator^( const Vector2D<T>& other )
    {
        return x * other.y - y*other.x;
    }

    auto norm() const noexcept
    {
        return std::sqrt( static_cast<double>( x*x + y*y ) );
    }

    Vector2D<T>& normalize()
    {
        auto n = this->norm();

        return ( n==0 ) ? (*this) : (*this) /= n;
    }

    [[nodiscard]]
    Vector2D<T> normalized()
    {
        auto n = this->norm();

        return ( (*this) == 0 ) ? (*this) : (*this) /= n;
    }

    T x;
    T y;
};

template < typename T >
constexpr Vector2D<T> operator+( const Vector2D<T>& v,  const SDL_Point& p ) noexcept
{
    return Vector2D{ v.x + p.x,
                     v.y + p.y };
}

template < typename T >
constexpr Vector2D<T> operator-( const Vector2D<T>& v,  const SDL_Point& p ) noexcept
{
    return Vector2D{ v.x - p.x,
                     v.y - p.y };
}

template < typename T >
constexpr Vector2D<T> operator+( const SDL_Point& p, const Vector2D<T>& v ) noexcept
{
    return Vector2D{ v.x + p.x,
                     v.y + p.y };
}

template < typename T >
constexpr Vector2D<T> operator-( const SDL_Point& p, const Vector2D<T>& v ) noexcept
{
    return Vector2D{ p.x - v.x,
                     p.y - v.y };
}

template < typename T >
constexpr Vector2D<T> operator-( const Vector2D<T>& v )
{
    return Vector2D<T>{ -v.x, -v.y };
}

template < typename T >
constexpr Vector2D<T> operator*( T scalar, const Vector2D<T>& v )
{
    return v*scalar;
}


template < typename T >
constexpr auto distance_between( const Vector2D<T>& v1, const Vector2D<T>& v2 )
{
    return (v1-v2).norm();
}

template < typename T >
constexpr auto angle_between( const Vector2D<T>& v1, const Vector2D<T>& v2 )
{
    double denominator = v1.norm()*v2.norm();

    if( denominator == 0.0 )
    { throw std::invalid_argument( "dzielenie przez zero" ); }
    
    double s = (v1^v2) / denominator;
    double c = (v1*v2) / denominator;

    return std::atan2( s, c );
}

template < typename T >
std::string to_string( const Vector2D<T>& v )
{
    std::ostringstream oss;
    oss << "(" << std::setw(5) << v.x 
        << ";" << std::setw(5) << v.y << ")";

    return oss.str();
}

template < typename T >
std::ostream& operator<<( std::ostream& o, const Vector2D<T>& v )
{
    o << to_string( v );
    return o;
}

namespace std
{
    template < typename T >
    struct hash< Vector2D<T> >
    {
        std::size_t operator()( const Vector2D<T>& v ) const noexcept
        {
            std::size_t h1 = std::hash<T>{}(v.x);
            std::size_t h2 = std::hash<T>{}(v.y);
            return h1 ^ ( h2 + 0x9e3779b9 + ( h1 << 6 ) + ( h1 >> 2 ) );
        }
    };
}

#endif