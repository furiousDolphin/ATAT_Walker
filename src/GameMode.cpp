
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
    land_{graphics_manager, origin_},
    entities_{graphics_manager, event_manager, land_, dt, origin_},
    player_{nullptr},
    water_objects_{renderer, origin_, dt}
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
    JsonLevelFormat json_level_format_data{context_.persistent_state.level};
    json_level_format_data.import_from_json();

    std::cout << "import data game" << "\n";
    land_.load_level(json_level_format_data);
    entities_.load_level(json_level_format_data);
    water_objects_.load_level(json_level_format_data);

    const auto& imported_data = json_level_format_data.get_import_data();
    auto it = imported_data.find(JsonLevelFormat::PLAYER);
    if (it != imported_data.end() && !it->second.empty()) 
    { 
        const auto& players = it->second;
        auto [int_center, id] = *players.begin(); 
        std::cout << "tworzy playera" << "\n";
        player_ = std::make_unique<Player>(static_cast<Vector2D<double>>(int_center));       
    }
}

void GameMode::update()
{
    auto& request_game_reload = context_.persistent_state.request_game_reload;
    if (request_game_reload)
    {
        request_game_reload = false;
        this->import_data();
    }

    land_.update();
    buttons_.update();
    entities_.update();
    player_->update(context_.graphics_manager, context_.event_manager, land_, context_.dt);
    //water_objects_.update();

    

    // Vector2D<double> center = player_->get_center();
    // render_scroll_.x += (center.x - WIDTH/2 - render_scroll_.x) / 1000.0;
    // render_scroll_.y += (center.y - HEIGHT/2 - render_scroll_.y) / 1000.0;

    // origin_ = static_cast<Vector2D<int>>(render_scroll_);

    float smoothness = 5.0f; 
    Vector2D<double> center = player_->get_center();
    Vector2D<double> target = { center.x - WIDTH/2, center.y - HEIGHT/2 };

    double factor = 1.0 - std::exp(-smoothness * context_.dt);

    render_scroll_.x += (target.x - render_scroll_.x) * factor;
    render_scroll_.y += (target.y - render_scroll_.y) * factor;

    origin_ = static_cast<Vector2D<int>>(render_scroll_);
}

void GameMode::render()
{
    SDL_Renderer* renderer = context_.renderer;
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xff );
    SDL_RenderClear( renderer );

    land_.render();
    buttons_.render();
    entities_.render();
    player_->render(origin_);
    water_objects_.render();
    
    SDL_RenderPresent( renderer );
}

void GameMode::run()
{
    this->update();
    this->render();
}