
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


EditorMenuButton::EditorMenuButton( 
    Vector2D<int> pos,
    std::function< void(int) > func,
    const std::vector< std::pair< int, const Texture* > >* main_textures,
    const std::vector< std::pair< int, const Texture* > >* alt_textures 
):
    Button{pos},
    func_{func},
    main_textures_{main_textures},
    alt_textures_{alt_textures},
    texture_index_{0},
    main_active_{true},
    referencial_pos_{pos}
{
    this->update_rect();
}

int EditorMenuButton::get_id() const
{
    const auto* textures = (main_active_ || alt_textures_ == nullptr) ? main_textures_ : alt_textures_;
    const auto& [id, texture_ptr] = textures->at(texture_index_);
    return id;
}

void EditorMenuButton::update_rect()
{
    const auto* textures = (main_active_ || alt_textures_ == nullptr) ? main_textures_ : alt_textures_;
    const auto& [id, texture_ptr] = textures->at(texture_index_);

    auto [ref_x, ref_y] = referencial_pos_;
    auto w = texture_ptr->get_width();
    auto h = texture_ptr->get_height();

    rect_ = { ref_x - w/2 + TILE_SIZE/2, ref_y - h/2 + TILE_SIZE/2, w, h };    
}

bool EditorMenuButton::update( const EventManager& event_manager )
{
    Vector2D<int> mouse_pos = event_manager.mouse_pos();
    bool collision = false;
    main_active_ = true;

    if ( rect_.collide_point( mouse_pos ) )
    {
        collision = true;

        if ( event_manager.left_got_clicked() )
        { this->func_( this->get_id() ); }

        if( event_manager.key_down( SDL_SCANCODE_LALT ) )
        {
            main_active_ = false;
            this->update_rect();
        } 
        
        int scroll_state = event_manager.get_scroll_state();

        if( scroll_state != 0 )
        {
            texture_index_ += scroll_state;
            texture_index_ = std::max( 0, std::min( texture_index_, static_cast<int>(main_textures_->size()) - 1 ) );

            this->update_rect();
            this->func_( this->get_id() );
        }      
    } 
    return collision;
}

void EditorMenuButton::render() const
{
    // SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xff );
    // SDL_RenderFillRect( renderer, rect_.sdl_rect_ptr() );

    const auto* textures = (main_active_ || alt_textures_ == nullptr) ? main_textures_ : alt_textures_;
    const auto& [id, texture_ptr] = textures->at(texture_index_);
    
    texture_ptr->render(rect_.get_pos());
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
