
#include "EventManager.hpp"

void EventManager::update()
{
    last_clicking_  = current_clicking_;
    last_mouse_pos_ = current_mouse_pos_;

    scroll_state_ = 0;

    SDL_Event event;

    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
            case SDL_MOUSEBUTTONDOWN:
                if( event.button.button == SDL_BUTTON_LEFT )   current_clicking_[0] = true;
                if( event.button.button == SDL_BUTTON_MIDDLE ) current_clicking_[1] = true;
                if( event.button.button == SDL_BUTTON_RIGHT )  current_clicking_[2] = true;
                break;
            case SDL_MOUSEBUTTONUP:
                if( event.button.button == SDL_BUTTON_LEFT )   current_clicking_[0] = false;
                if( event.button.button == SDL_BUTTON_MIDDLE ) current_clicking_[1] = false;
                if( event.button.button == SDL_BUTTON_RIGHT )  current_clicking_[2] = false;
                break;
            case SDL_MOUSEMOTION:
                current_mouse_pos_.x = event.motion.x;
                current_mouse_pos_.y = event.motion.y;
                break;
            case SDL_MOUSEWHEEL:
                     if( event.wheel.y < 0 ) scroll_state_ = -1; 
                else if( event.wheel.y > 0 ) scroll_state_ =  1;
                else                         scroll_state_ =  0;
                break;
            case SDL_KEYDOWN:
                keys_down_[ event.key.keysym.scancode ] = true;
                break;
            case SDL_KEYUP:
                keys_down_[ event.key.keysym.scancode ] = false;
                break;
            case SDL_QUIT:
                quit_ = true;
                break;
            default:
                break;
        }
    }
}

bool EventManager::key_down(SDL_Scancode scancode) const 
{
    auto it = keys_down_.find(scancode);
    return it != keys_down_.end() ? it->second : false;
}

