

#ifndef EVENTMANAGER_HPP_
#define EVENTMANAGER_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <array>
#include <unordered_map>


#include "Vector2D.hpp"



class EventManager
{
    public:
        EventManager():
            current_clicking_{ {false, false, false} },
            last_clicking_   { {false, false, false} },
            quit_            { false                 }
        {}

        bool left_got_clicked()   const { return ( current_clicking_[0] && !last_clicking_[0] ); }
        bool middle_got_clicked() const { return ( current_clicking_[1] && !last_clicking_[1] ); }
        bool right_got_clicked()  const { return ( current_clicking_[2] && !last_clicking_[2] ); }

        bool left_is_clicked()   const { return ( current_clicking_[0] ); }
        bool middle_is_clicked() const { return ( current_clicking_[1] ); }
        bool right_is_clicked()  const { return ( current_clicking_[2] ); }

        bool left_got_unclicked()   const { return !current_clicking_[0] && last_clicking_[0]; }
        bool middle_got_unclicked() const { return !current_clicking_[1] && last_clicking_[1]; }
        bool right_got_unclicked()  const { return !current_clicking_[2] && last_clicking_[2]; }


        bool key_down( SDL_Scancode scancode ) const;

        bool mouse_motion() const { return ( current_mouse_pos_ != last_mouse_pos_ ); }
        Vector2D<int> mouse_pos() const { return ( current_mouse_pos_ ); }

        int get_scroll_state() const { return scroll_state_; }


        bool check_quit() const { return quit_; }

        void update();

    private:

        bool quit_;

        std::array<bool, 3> current_clicking_;
        std::array<bool, 3> last_clicking_;

        int scroll_state_;

        Vector2D<int> current_mouse_pos_;
        Vector2D<int> last_mouse_pos_;

        std::unordered_map< SDL_Scancode, bool > keys_down_;
};

#endif

