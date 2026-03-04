
#include <cmath>
#include <utility>

#include "Animation.hpp"




Animation::Animation( std::shared_ptr<std::vector<Texture>> textures, int texture_duration, bool loop ):
    textures_{textures},
    texture_duration_{texture_duration},
    loop_{loop},
    done_{false},
    current_frame_{0}
{
    animation_duration_ = texture_duration * textures->size();     
}   

const Texture* Animation::get_texture() const 
{ return &textures_->at( static_cast<int>( current_frame_/texture_duration_ ) ); };

Animation Animation::copy() const 
{ return Animation( textures_, texture_duration_, loop_ ); }

void Animation::update( float dt )
{
    if( !done_ && animation_duration_ )
    {
        current_frame_ = std::fmod( current_frame_ + 60*dt, animation_duration_ );

        if( !loop_ && current_frame_ - 1 > animation_duration_ ) 
        { done_ = true; }
    }
}