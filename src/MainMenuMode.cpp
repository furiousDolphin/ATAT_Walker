

#include "MainMenuMode.hpp"


MainMenuMode::MainMenuMode( 
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

void MainMenuMode::create_buttons()
{
    const auto* game_button_textures = context_.graphics_manager.get_text_button_textures_ptr("GAME", GraphicsManager::MINECRAFT_24);
    auto game_button_func = [this]()
    { 
        context_.persistent_state.mode = ModeType::GAME; 
        context_.persistent_state.request_game_reload = true;
    };
    buttons_.add(std::make_unique<TextButton>(Vector2D<int>(0, 0), game_button_func, game_button_textures));


    const auto* editor_button_textures = context_.graphics_manager.get_text_button_textures_ptr("EDITOR", GraphicsManager::MINECRAFT_24);
    auto editor_button_func = [this]()
    { 
        context_.persistent_state.mode = ModeType::EDITOR;
        context_.persistent_state.request_editor_reload = true;
    };
    buttons_.add(std::make_unique<TextButton>(Vector2D<int>(0, 40), editor_button_func, editor_button_textures));
}

void MainMenuMode::update()
{ buttons_.update(); }

void MainMenuMode::render()
{
    SDL_Renderer* renderer = context_.renderer;
    SDL_SetRenderDrawColor( renderer, 0xff, 0xff, 0xff, 0xff );
    SDL_RenderClear( renderer );

    buttons_.render();

    SDL_RenderPresent( renderer );
}

void MainMenuMode::run()
{
    this->update();
    this->render();
}

