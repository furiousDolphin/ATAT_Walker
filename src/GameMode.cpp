
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
    float& dt 
) :
    context_{renderer, event_manager, graphics_manager, persistent_state, dt},
    buttons_{event_manager, graphics_manager},
    sliders_{event_manager, graphics_manager}
{
    this->create_buttons();
    this->create_sliders();
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
    const auto& graphics_manager = context_.graphics_manager;
    sliders_.add(std::make_unique<Slider>(Vector2D<int>{WIDTH-1*w-1*margin, margin}, graphics_manager, 0, 100, 50, [this](double val){std::cout<<val<<"\n";}));
    sliders_.add(std::make_unique<Slider>(Vector2D<int>{WIDTH-2*w-2*margin, margin}, graphics_manager, 0, 100, 50, [this](double val){std::cout<<val<<"\n";}));
    sliders_.add(std::make_unique<Slider>(Vector2D<int>{WIDTH-3*w-3*margin, margin}, graphics_manager, 0, 100, 50, [this](double val){std::cout<<val<<"\n";}));
    sliders_.add(std::make_unique<Slider>(Vector2D<int>{WIDTH-4*w-4*margin, margin}, graphics_manager, 0, 100, 50, [this](double val){std::cout<<val<<"\n";}));
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