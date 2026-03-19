#pragma once
#include <string>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <filesystem>

#include "Vector2D.hpp"

namespace fs = std::filesystem;


struct SDLTextureDeleter 
{
    void operator()(SDL_Texture* texture) const noexcept 
    {
        if( texture != nullptr ) SDL_DestroyTexture(texture);
    }
};



class Texture 
{
    public:
        Texture() = default;

        explicit Texture( const SDL_Renderer* renderer ): 
            renderer_{ renderer } 
        {}


        Texture( const SDL_Renderer* renderer, const std::string& path ): 
            renderer_{ renderer }
        {
            if( !load_file(path) ) 
                throw std::invalid_argument( "Nie udało się załadować pliku -> " + path );
        }


        Texture( const SDL_Renderer* renderer, const std::string& text, SDL_Color color, const TTF_Font* font ): 
            renderer_{ renderer }
        {
            if( !load_text(text, color, font) ) 
                throw std::invalid_argument( "Nie udało się utworzyć tekstu -> " + text );
        }




        Texture(const Texture&)            = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept            = default;
        Texture& operator=(Texture&& other) noexcept = default;

        ~Texture() = default; 




        void set_blend_mode( SDL_BlendMode blending ) 
        {
            if( texture_ != nullptr ) 
                SDL_SetTextureBlendMode( texture_.get(), blending );
        }


        void set_alpha( Uint8 alpha ) 
        {
            if( texture_ != nullptr ) 
                SDL_SetTextureAlphaMod( texture_.get(), alpha );
        }


        void render(int x, int y, const SDL_Rect* clip = nullptr, double angle = 0.0,
                    const SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE) const
        {
            if( texture_ != nullptr )
            {
                SDL_Rect rect{ x, y, width_, height_ };

                if ( clip != nullptr ) 
                {
                    rect.w = clip->w;
                    rect.h = clip->h;
                }

                SDL_RenderCopyEx( const_cast<SDL_Renderer*>(renderer_), texture_.get(), clip, &rect, angle, center, flip );
            }
        }

        void render(Vector2D<int> pos, const SDL_Rect* clip = nullptr, double angle = 0.0,
                    const SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE) const
        {
            this->render( pos.x, pos.y, clip, angle, center, flip );    
        }
        



        SDL_Texture* get() const { return texture_.get(); }

        int get_width()  const { return width_;  }
        int get_height() const { return height_; }
        Vector2D<int> get_shape() const { return {width_, height_}; } 



        
    private:
        std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture_;

        const SDL_Renderer* renderer_;

        int width_;
        int height_;


        bool load_file(const std::string& path) 
        {
            // char* base_path = SDL_GetBasePath();

            // if( !base_path )
            //     throw std::invalid_argument( "nie wyszlo z base_path" );

            // fs::path exe_path(base_path);
            // SDL_free(base_path);
            // fs::path project_root = exe_path.parent_path().parent_path();
            // fs::path file_path = project_root / path;


            //SDL_Surface* surface = IMG_Load(file_path.string().c_str());

            SDL_Surface* surface = IMG_Load(path.c_str());

            if( surface == nullptr ) 
            {
                SDL_Log( "IMG_Load error: %s", IMG_GetError() );
                return false;
            }

            SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0xFF, 0xFF) );

            texture_.reset(SDL_CreateTextureFromSurface(const_cast<SDL_Renderer*>(renderer_), surface));

            if ( texture_ == nullptr ) 
            {
                SDL_Log( "SDL_CreateTextureFromSurface error: %s", SDL_GetError() );
                SDL_FreeSurface(surface);
                return false;
            }

            width_  = surface->w;
            height_ = surface->h;

            SDL_FreeSurface( surface );
            return true;
        }

        bool load_text(const std::string& text, SDL_Color color, const TTF_Font* font) 
        {
            SDL_Surface* surface = TTF_RenderText_Solid(const_cast<TTF_Font*>(font), text.c_str(), color);

            if (!surface) 
            {
                SDL_Log( "TTF_RenderText_Solid error: %s", TTF_GetError() );
                return false;
            }

            texture_.reset(SDL_CreateTextureFromSurface(const_cast<SDL_Renderer*>(renderer_), surface));
            if (!texture_) 
            {
                SDL_Log( "SDL_CreateTextureFromSurface error: %s", SDL_GetError() );
                SDL_FreeSurface(surface);
                return false;
            }

            width_  = surface->w;
            height_ = surface->h;

            SDL_FreeSurface( surface );
            return true;
        }
};