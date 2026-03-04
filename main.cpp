#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <functional>

#include "Settings.hpp"
#include "App.hpp"


int main( int argc, char* argv[] )
{
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;

    if( Init( window, renderer ) )
    {
        App app{ window, renderer };
        app.run();

        Close( window, renderer );
    }
    

    return 0;
}