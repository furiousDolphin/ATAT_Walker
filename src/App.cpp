
#include <memory>
#include <iomanip>

#include "App.hpp"
#include "GameMode.hpp"
#include "MainMenuMode.hpp"

App::App(OscilloscopeInputs& oscilloscope_inputs, const std::string& base_path) :
    window_{nullptr},
    renderer_{nullptr},
    event_manager_{},
    graphics_manager_{},
    persistent_state_{""},
    modes_map_{}
{

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    if (!this->init())
    { throw std::runtime_error("SDL nie ruszyl"); }

    try
    {
        graphics_manager_.init(renderer_, base_path);
        modes_map_.emplace(ModeType::GAME, std::make_unique<GameMode>  ( renderer_, event_manager_, graphics_manager_, persistent_state_, oscilloscope_inputs, base_path, delta_time_) );
        modes_map_.emplace(ModeType::MAIN_MENU, std::make_unique<MainMenuMode>( renderer_, event_manager_, graphics_manager_, persistent_state_, delta_time_) );
    }
    catch(const std::exception& e)
    { std::cerr << e.what() << '\n'; }
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

}


bool App::init()
{ return Init( window_, renderer_ ); }

App::~App()
{ Close( window_, renderer_ ); }

bool App::run_once()
{
    bool quit = event_manager_.check_quit();
    
    bool cond = !quit;
    if (cond)
    {
        current_time_ = SDL_GetTicks();
        delta_time_ = (current_time_ - last_time_) / 1000.0f;
        last_time_ = current_time_;

        modes_map_.at( persistent_state_.mode )->run();
        event_manager_.update();    
    }
    return cond;
}

void App::run()
{
    while( this->run_once() )
    {} 
}
