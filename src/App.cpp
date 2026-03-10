
#include <memory>
#include <iomanip>

#include "App.hpp"
#include "GameMode.hpp"
#include "MainMenuMode.hpp"

App::App( SDL_Window* window, SDL_Renderer* renderer ) :
    window_{window},
    renderer_{renderer},
    event_manager_{},
    graphics_manager_{renderer},
    persistent_state_{""},
    modes_map_{}
{

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    modes_map_.emplace(ModeType::GAME, std::make_unique<GameMode>  ( renderer_, event_manager_, graphics_manager_, persistent_state_, delta_time_) );
    modes_map_.emplace(ModeType::MAIN_MENU, std::make_unique<MainMenuMode>( renderer_, event_manager_, graphics_manager_, persistent_state_, delta_time_) );

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

}

App::~App() = default;

void App::run()
{
    while( !event_manager_.check_quit() )
    {
        current_time_ = SDL_GetTicks();
        delta_time_ = (current_time_ - last_time_) / 1000.0f;
        last_time_ = current_time_;

        modes_map_.at( persistent_state_.mode )->run();
        event_manager_.update();
    } 
}
