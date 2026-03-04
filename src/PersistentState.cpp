

#include "PersistentState.hpp"

PersistentState::PersistentState(std::string levels_folder_path) :
    mode{ModeType::MAIN_MENU},
    level{"map.json"},
    request_editor_reload{false},
    request_game_reload{false}
{   

}