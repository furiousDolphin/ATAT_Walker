#ifndef MAINMENUMODE_HPP_
#define MAINMENUMODE_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>


#include "Settings.hpp"
#include "Mode.hpp"
#include "EventManager.hpp"
#include "Button.hpp"
#include "PersistentState.hpp"




class MainMenuMode : public Mode
{
    public:
        MainMenuMode( 
            SDL_Renderer* renderer, 
            EventManager& event_manager, 
            GraphicsManager& graphics_manager, 
            PersistentState& persistent_state,
            float& dt  );

        void run() override;

    private:
        void create_buttons();

        void update();
        void render();

        struct Context
        {
            SDL_Renderer* renderer;
            EventManager& event_manager;
            GraphicsManager& graphics_manager;
            PersistentState& persistent_state;
            float& dt;
        } context_;

        Buttons buttons_;
};


#endif