

#ifndef PERSISTENT_STATE_HPP_
#define PERSISTENT_STATE_HPP_


#include "Settings.hpp"

class PersistentState
{
    public:
        PersistentState(std::string levels_folder_path);
        ModeType mode;
        std::string level;
        bool request_editor_reload;
        bool request_game_reload;
        std::vector<std::string> levels;
        //tu powinny byc tez sciezki wszelkiej masci
    private:
};

#endif