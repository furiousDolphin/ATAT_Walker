
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
    buttons_{event_manager, graphics_manager}
{
    this->create_buttons();
}

void GameMode::create_buttons()
{
    const auto* main_menu_textures = context_.graphics_manager.get_text_button_textures_ptr("MAIN MENU", GraphicsManager::MINECRAFT_24);
    auto main_menu_button_func = [this]()
    {context_.persistent_state.mode = ModeType::MAIN_MENU;};
    buttons_.add(std::make_unique<TextButton>(Vector2D<int>(0, 0), main_menu_button_func, main_menu_textures));
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
}

void GameMode::render()
{
    SDL_Renderer* renderer = context_.renderer;
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xff );
    SDL_RenderClear( renderer );

    buttons_.render();
    
    SDL_RenderPresent( renderer );
}

void GameMode::run()
{
    this->update();
    this->render();
}