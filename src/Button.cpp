
#include <Button.hpp>


Button::Button( Vector2D<int> pos):
    rect_{ pos }
{}



TextButton::TextButton( 
    Vector2D<int> pos, 
    std::function< void(void) > func, 
    const GraphicsManager::TextButtonTextures* textures
):
    Button{pos},
    func_{func},
    textures_{textures}
{
    const auto& texture = textures->marked;
    rect_.set_w( texture.get_width() );
    rect_.set_h( texture.get_height() );
}

bool TextButton::update( const EventManager& event_manager )
{
    Vector2D<int> mouse_pos = event_manager.mouse_pos();
    bool collision = false;
    if ( rect_.collide_point(mouse_pos) )
    {
        if ( event_manager.left_got_clicked() )
        { this->func_(); }
        
        collision = true;
    }
    is_marked_ = collision;
    return collision;
}

void TextButton::render() const
{
    if ( is_marked_ )
    { textures_->marked.render(rect_.get_pos()); }
    else
    { textures_->unmarked.render(rect_.get_pos()); }
}



Buttons::Buttons(EventManager& event_manager, GraphicsManager& graphics_manager) :
    context_{event_manager, graphics_manager}
{}

bool Buttons::update()
{
    bool any = false;

        for ( auto& button : buttons_ )
        {
            any = button->update(context_.event_manager);
            if ( any )
            { break; }
        }
    return any;
}

void Buttons::render() const
{
    for ( const auto& button : buttons_ )
    { button->render(); }
}

void Buttons::add( std::unique_ptr<Button> button )
{ buttons_.push_back(std::move(button)); }
