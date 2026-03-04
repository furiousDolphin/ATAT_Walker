
#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_

#include <vector>
#include <memory>

#include "Texture.hpp"

class Animation
{
    public:
        Animation() = default;
        Animation( std::shared_ptr<std::vector<Texture>> textures, int texture_duration, bool loop = false );
   
        
        Animation copy() const;
        
        void update( float dt );
        const Texture* get_texture() const;
        
    private:
        std::shared_ptr<std::vector<Texture>> textures_;
        int texture_duration_;
        int animation_duration_;
        float current_frame_;  

        bool loop_;
        bool done_;
};

#endif