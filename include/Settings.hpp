
#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <variant>

#include "Vector2D.hpp"
#include "Texture.hpp"
#include "Rect.hpp" 
#include "SDL_Management.hpp"



/*-------------------------------------------------------------------------------*/

inline constexpr int WIDTH = 960;
inline constexpr int HEIGHT = 540;
inline constexpr int TILE_SIZE = 64;
inline constexpr double SQRT2 = 1.414213562373095;

/*-------------------------------------------------------------------------------*/

enum class ModeType 
{ 
    GAME, 
    EDITOR,
    MAIN_MENU
};

namespace std 
{
    template<>
    struct hash<ModeType> 
    {
        size_t operator()(const ModeType& mt) const noexcept 
        {
            return std::hash<int>()(static_cast<int>(mt));
        }
    };
}

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1); 
    }
};

/*-------------------------------------------------------------------------------*/

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


#endif