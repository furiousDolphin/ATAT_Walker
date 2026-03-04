
#ifndef SDL_MANAGEMENT_HPP_
#define SDL_MANAGEMENT_HPP_

#include <memory>
#include <string>


#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>



bool Init(  SDL_Window*& window, SDL_Renderer*& renderer );
void Close( SDL_Window*& window, SDL_Renderer*& renderer );

class FontManager
{
    public:
        FontManager( const std::string& font_path, std::size_t font_size );

        FontManager( const FontManager& )            = delete;
        FontManager& operator=( const FontManager& ) = delete;

        FontManager( FontManager&& )            = default;
        FontManager& operator=( FontManager&& ) = default;

        TTF_Font* get_font() { return font_.get(); }
        const TTF_Font* get_font() const { return font_.get(); }

    private:
        std::shared_ptr<TTF_Font> font_;
        std::string               font_path_;
        std::size_t               font_size_;



};

#endif