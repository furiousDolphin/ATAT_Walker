

#ifndef APP_HPP_
#define APP_HPP_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <unordered_map>
#include <memory>

#include "Settings.hpp"
#include "EventManager.hpp"
#include "GraphicsManager.hpp"
#include "SDL_Management.hpp"
#include "Mode.hpp"
#include "PersistentState.hpp"



class App
{
    public:
        App( SDL_Window* window, SDL_Renderer* renderer );
        ~App();
        
        void run();

    private:
        SDL_Window*     window_;
        SDL_Renderer*   renderer_;
        EventManager    event_manager_;
        GraphicsManager graphics_manager_;

        PersistentState persistent_state_;

        Uint32 last_time_;
        Uint32 current_time_;
        float delta_time_;

        std::unordered_map< ModeType, std::unique_ptr<Mode> > modes_map_;
};



using DoubleSetter = std::function<void(double)>;
using DoubleGetter = std::function<double(void)>;

class ValueManager  //to trzeba do pybind wyslac
{
    public:
        ValueManager();
        double get_val() const;
        DoubleGetter getter;
        DoubleSetter setter;
    private:
        double val_;
};

#endif