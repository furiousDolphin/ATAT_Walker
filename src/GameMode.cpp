
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

#include <filesystem>
namespace fs = std::filesystem;

#include "GameMode.hpp"

GameMode::GameMode( 
    SDL_Renderer* renderer, 
    EventManager& event_manager, 
    GraphicsManager& graphics_manager, 
    PersistentState& persistent_state,
    OscilloscopeInputs& oscilloscope_inputs, 
    float& dt 
) :
    context_{renderer, event_manager, graphics_manager, persistent_state, oscilloscope_inputs, dt},
    buttons_{event_manager, graphics_manager},
    sliders_{event_manager, graphics_manager},
    at_at_{graphics_manager, dt}
{
    this->create_buttons();
    this->create_sliders();

    sliders_.init();
    at_at_.init();

    auto& [os_u, os_y] = context_.oscilloscope_inputs;
    auto& [at_u, at_y] = at_at_.get_speed_inputs();

    os_y.getter = at_y.getter;
}

void GameMode::create_buttons()
{
    const auto* main_menu_textures = context_.graphics_manager.get_text_button_textures_ptr("MAIN MENU", GraphicsManager::MINECRAFT_24);
    auto main_menu_button_func = [this]()
    {context_.persistent_state.mode = ModeType::MAIN_MENU;};
    buttons_.add(std::make_unique<TextButton>(Vector2D<int>(0, 0), main_menu_button_func, main_menu_textures));
}

void GameMode::create_sliders()
{
    int w = 100;
    int margin = 20;

    auto& [os_u, os_y] = context_.oscilloscope_inputs;
    auto& [at_u, at_y] = at_at_.get_speed_inputs();
    auto& [zeta, r, f] = at_at_.get_sys_inputs();
    const auto& graphics_manager = context_.graphics_manager;

    //-----------------------------------------   z tego trzeba zrobic json'a aby latwo testowac

    struct Range
    {
        double min_v;
        double max_v;
        double init_v;
    };

    Range f_r{0.1, 5.0, 1.0};
    Range r_r{0.1, 2.0, 0.8};
    Range zeta_r{0.1, 2.0, 0.8};
    Range speed_u_r{-50.0, 50.0, 0.0};

    //-----------------------------------------

    sliders_.add(std::make_unique<Slider>(
        Vector2D<int>{WIDTH-1*w-1*margin, margin}, 
        graphics_manager, speed_u_r.min_v, speed_u_r.max_v, speed_u_r.init_v, 
        [this, &os_u, &at_u](double val)
        {os_u.set_val(val); at_u.set_val(val);}));
    sliders_.add(std::make_unique<Slider>(
        Vector2D<int>{WIDTH-2*w-2*margin, margin}, 
        graphics_manager, zeta_r.min_v, zeta_r.max_v, zeta_r.init_v, 
        zeta.setter));
    sliders_.add(std::make_unique<Slider>(
        Vector2D<int>{WIDTH-3*w-3*margin, margin}, 
        graphics_manager, r_r.min_v, r_r.max_v, r_r.init_v, 
        r.setter));
    sliders_.add(std::make_unique<Slider>(
        Vector2D<int>{WIDTH-4*w-4*margin, margin}, 
        graphics_manager, f_r.min_v, f_r.max_v, f_r.init_v, 
        f.setter));
}

void GameMode::import_data()
{

}

void GameMode::update()
{
    auto& request_game_reload = context_.persistent_state.request_game_reload;
    if (request_game_reload)
    {
        request_game_reload = false;
        this->import_data();
    }

    buttons_.update();
    sliders_.update();
}

void GameMode::render()
{
    SDL_Renderer* renderer = context_.renderer;
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xff );
    SDL_RenderClear( renderer );

    buttons_.render();
    sliders_.render();
    
    SDL_RenderPresent( renderer );
}

void GameMode::run()
{
    this->update();
    this->render();
}