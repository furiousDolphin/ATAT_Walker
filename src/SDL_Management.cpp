

#include <iostream>
#include "Settings.hpp"
#include "SDL_Management.hpp"

bool Init( SDL_Window*& window, SDL_Renderer*& renderer )
{
    bool success = true;

    if( SDL_Init( SDL_INIT_AUDIO | SDL_INIT_VIDEO ) ) 
    {
        std::cerr << "Blad inicjalizacji SDL" << SDL_GetError() << std::endl;
        success = false;
    }

    else
    {
        window = SDL_CreateWindow( "name",
                                   SDL_WINDOWPOS_CENTERED, 
                                   SDL_WINDOWPOS_CENTERED, 
                                   WIDTH, 
                                   HEIGHT, 
                                   SDL_WINDOW_SHOWN );

        if( window == nullptr )
        {
            std::cerr << "Blad tworzenia \"window\"" << SDL_GetError() << std::endl;
            success = false;
        }
        else
        {
            renderer = SDL_CreateRenderer( window,
                                           -1, 
                                           SDL_RENDERER_ACCELERATED );
            if( renderer == nullptr )
            {
                std::cerr << "Blad tworzenia \"renderer\"" << SDL_GetError() << std::endl;
                success = false;
            }
            else
            {
                int img_flags = IMG_INIT_PNG;

                if( !( IMG_Init(img_flags) & img_flags ) )
                {
                    std::cerr << "Blad inicjalizacji \"IMG\"" << IMG_GetError() << std::endl;
                    success = false;
                }
                else
                {
                    if(  TTF_Init() == -1 )
                    {
                        std::cerr << "Blad inicjalizacji \"TTF\"" << IMG_GetError() << std::endl;
                        success = false;
                    }
                }
            }
        }
    }

   return success; 
}

void Close( SDL_Window*& window, SDL_Renderer*& renderer ) 
{ 
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );

    window   = nullptr;
    renderer = nullptr;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

FontManager::FontManager( const std::string& font_path, std::size_t font_size )
{
    font_      = std::shared_ptr<TTF_Font>( TTF_OpenFont( font_path.c_str(), font_size ), []( TTF_Font* f )
                                                                                            { 
                                                                                                TTF_CloseFont( f ); 
                                                                                            } );
    font_path_ = font_path;
    font_size_ = font_size;
}