
#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <memory>
#include <functional>
#include <string>
#include <vector>

#include "Texture.hpp"
#include "Vector2D.hpp"
#include "Rect.hpp"
#include "Settings.hpp"
#include "EventManager.hpp"
#include "GraphicsManager.hpp"
#include "SDL_Management.hpp"

class Buttons;
class Button
{
    public:
        friend Buttons;

        Button( Vector2D<int> pos );
        virtual ~Button() = default;
        
    protected:
        virtual bool update( const EventManager& event_manager ) = 0;
        virtual void render() const = 0;
        Rect rect_;

};


class TextButton : public Button
{
    public:
        friend Buttons;

        TextButton( 
            Vector2D<int> pos, 
            std::function< void(void) > func,
            const GraphicsManager::TextButtonTextures* textures
        );


    private:
        bool update( const EventManager& event_manager ) override;
        void render() const override;
        const GraphicsManager::TextButtonTextures* textures_;
        std::function< void(void) > func_;
        bool is_marked_;

};



class Buttons
{
    public:
        Buttons(EventManager& event_manager, GraphicsManager& graphics_manager);
        bool update();
        void render() const;
        void add( std::unique_ptr<Button> button );
    private:
        std::vector<std::unique_ptr<Button>> buttons_;

        struct Context
        {
            EventManager& event_manager;
            GraphicsManager& graphics_manager;
        } context_;
};

#endif