
#include "Slider.hpp"


Slider::SlideRect::SlideRect(
    Vector2D<int> center, 
    Vector2D<int> shape, 
    SlideRect::Axis axis, 
    std::function<Vector2D<int>(void)> getter,  
    std::function<void(int)> setter
) :
    Rect{center.x-shape.x/2, center.y-shape.y/2, shape.x, shape.y}, 
    axis_{axis},
    getter_{getter},
    setter_{setter}
{

}

Vector2D<int> Slider::SlideRect::get_pos() const
{
    auto [x, y] = getter_();
    return {x - rect_.w/2, y-rect_.h/2};
}

void Slider::SlideRect::set_pos(const Vector2D<int>& new_top_left)
{
    auto [x, y] = new_top_left;
    switch ( axis_ )
    {
        case Axis::HORIZONTAL:
            setter_(x+rect_.w/2);
            break;
        case Axis::VERTICAL:
            setter_(y+rect_.h/2);
            break;
    }
}

bool Slider::SlideRect::collide_point(const Vector2D<int>& v) const
{
    auto [x, y] = this->get_pos();
    return !( 
        x >= v.x || 
        v.x >= x + rect_.w || 
        y >= v.y || 
        v.y >= y + rect_.h );
}

Slider::Slider(
    Vector2D<int> pos,
    const GraphicsManager& graphics_manager,
    double min_val, 
    double max_val, 
    double init_val, 
    std::function< void(double) > fun
) :
    platform_rect_{pos, textures_.platform.get_shape()},
    textures_{
        graphics_manager.get_texture(GraphicsManager::SLIDER_PLATFORM),
        graphics_manager.get_texture(GraphicsManager::SLIDER_MARKED_BUTTON),
        graphics_manager.get_texture(GraphicsManager::SLIDER_UNMARKED_BUTTON)},
    params_{
        std::min( min_val, max_val),
        std::max( min_val, max_val), 
        std::max( min_val, std::min( init_val, max_val ) )},
    fun_{fun},
    marked_{false}
{
    auto [w, h] = platform_rect_.get_shape();
    top_pos_ = platform_rect_.get_pos() + Vector2D<int>{w/2, w/2};
    bot_pos_ = top_pos_ + Vector2D<int>{0, (h - w)};
    cur_pos_.x = platform_rect_.get_centerx();
    cur_pos_.y = bot_pos_.y + (top_pos_.y - bot_pos_.y)*((params_.cur_val - min_val)/(max_val - min_val));
    std::cout << "top: "<< top_pos_.y << "\n";
    std::cout << "bot: "<< bot_pos_.y << "\n";
    std::cout << "cur: "<< cur_pos_.y << "\n";

    button_rect_ptr_ = std::make_unique<SlideRect>(
        Vector2D<int>{}, 
        textures_.unmarked_button.get_shape(),
        SlideRect::Axis::VERTICAL, 
        [this]()
        {return cur_pos_;},
        [this](int arg)
        { cur_pos_.y = std::max( top_pos_.y, std::min( arg, bot_pos_.y ) );} );
}

Rect* Slider::get_colliding_rect_ptr(Vector2D<int> p)
{
    if ( platform_rect_.collide_point(p) )
    {
        if ( button_rect_ptr_->collide_point(p) )
        { return button_rect_ptr_.get(); }
    }
}

double Slider::get_val() const
{ 
    const auto& [min_val, max_val, cur_val] = params_;
    return min_val + (max_val-min_val) * ((cur_pos_.y - bot_pos_.y)/(top_pos_.y - bot_pos_.y)); 
}

void Sliders::add(std::unique_ptr<Slider> slider_ptr)
{ sliders_.push_back(std::move(slider_ptr)); }

void Slider::update()
{}

void Slider::render() const
{
    textures_.platform.render(platform_rect_.get_pos());
    const Texture* button_texture_ptr = marked_ ? &textures_.marked_button : &textures_.unmarked_button;
    if ( button_rect_ptr_ )
    { 
        auto [x, y] = button_rect_ptr_->get_pos();
        button_texture_ptr->render(x, y); 
        //std::cout << x << " : " << y <<"\n";
        //std::cout << "rendersie wykonuje\n";
    }
}

Sliders::Sliders(
    const EventManager& event_manager, 
    const GraphicsManager& graphics_manager
) :
    context_{event_manager, graphics_manager}
{

}

void Sliders::update()
{
    const EventManager& event_manager = context_.event_manager;
    Vector2D<int> mouse_pos = event_manager.mouse_pos();

    if (grabbed_)
    {
        if ( event_manager.left_got_unclicked() )
        { 
            auto& slider = *grabbed_;
            //slider.update();
            grabbed_.clear();
        }
        else if ( event_manager.left_is_clicked() && event_manager.mouse_motion() )
        { grabbed_.update(mouse_pos); }
    }

    else
    {
        for ( auto it = sliders_.rbegin(); it != sliders_.rend(); it++ )
        {
            auto& slider_ptr = *it;
            Rect* grabbed_rect_ptr = slider_ptr->get_colliding_rect_ptr(mouse_pos);
            if ( grabbed_rect_ptr )
            {
                slider_ptr->marked_ = true;
                if (event_manager.left_got_clicked())
                { grabbed_.set_new(slider_ptr.get(), grabbed_rect_ptr, mouse_pos ); }
                break;
            }
            else
            {slider_ptr->marked_ = false;}
        }
    }
}

void Sliders::render() const
{
    for ( const auto& slider_ptr : sliders_ )
    { slider_ptr->render(); }
}

Sliders::GrabbedState::GrabbedState() :
    active_{false},
    obj_ptr_{nullptr},
    rect_ptr_{nullptr}      
{}          

Slider& Sliders::GrabbedState::operator*() const 
{ return *obj_ptr_; }
Sliders::GrabbedState::operator bool() const 
{ return active_; }
void Sliders::GrabbedState::clear() 
{
    active_ = false;
    obj_ptr_ = nullptr;
    rect_ptr_ = nullptr;
}
void Sliders::GrabbedState::set_new(Slider* new_obj_ptr, Rect* new_rect_ptr, Vector2D<int> global_mouse_pos) 
{   
    obj_ptr_ = new_obj_ptr; 
    rect_ptr_ = new_rect_ptr;
    active_ = true;
    offset_ = global_mouse_pos - new_rect_ptr->get_pos();
}
void Sliders::GrabbedState::update(Vector2D<int> global_mouse_pos)
{   
    if (active_)
    { 
        rect_ptr_->set_pos(global_mouse_pos-offset_);
    }
}