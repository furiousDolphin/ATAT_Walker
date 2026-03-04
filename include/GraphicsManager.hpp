#ifndef GRAPHICS_MANAGER_HPP_
#define GRAPHICS_MANAGER_HPP_

#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>
#include <set>
#include <variant>



#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "Texture.hpp"
#include "Animation.hpp"
#include "Settings.hpp"





class GraphicsManager
{
    public:
        GraphicsManager( SDL_Renderer* renderer );
        ~GraphicsManager() = default;

        enum VectorizedTextureKey
        {

        };

        enum SingularTextureKey
        {
            SLIDER_MARKED_BUTTON,
            SLIDER_UNMARKED_BUTTON,
            SLIDER_PLATFORM,
            ATAT_LEG
        };

        enum AnimationKey
        {

        };

        enum TextButtonKey
        {
            GAME,
            MENU
        };

        enum FontKey
        {
            MINECRAFT_18,
            MINECRAFT_24,
            MINECRAFT_36
        };

        struct TextButtonTextures
        {
            Texture marked;
            Texture unmarked;
        };

        
        class VectorizedTextures
        {
            public:
                VectorizedTextures(SDL_Renderer* renderer, const std::string& folder_path);
                const Texture& operator[](int index) const;
                std::optional<int> get_index_from_name(const std::string& texture_name) const;
            private:
                std::vector< Texture > textures_;
                std::unordered_map< std::string, int > name_to_index_translator_;
        };

        Animation copy_animation( AnimationKey key ) const;

        const Texture& get_texture( SingularTextureKey key ) const;
        const VectorizedTextures& get_vectorized_textures( VectorizedTextureKey key) const;

        const TextButtonTextures* get_text_button_textures_ptr( const std::string& text, FontKey font_key) const;
        const Texture* get_dynamic_texture_ptr(const std::string& file_path) const;

        using FoundItems = std::variant<std::monostate, const Texture*, const VectorizedTextures*, Animation>;

        FoundItems find_items_by_path(const std::string& path) const;

        
        
    private:
        inline static std::unordered_map<std::string, VectorizedTextureKey> vectorized_textures_as_path_key_map
        {    

        };

        inline static std::unordered_map<std::string, SingularTextureKey> singular_textures_as_path_key_map
        {
            {"data/graphics/slider/marked.png", SLIDER_MARKED_BUTTON},
            {"data/graphics/slider/unmarked.png", SLIDER_UNMARKED_BUTTON},
            {"data/graphics/slider/platform.png", SLIDER_PLATFORM},
            {"data/graphics/ATAT/leg.png", ATAT_LEG}
        };  

        inline static std::unordered_map<std::string, AnimationKey> animations_as_path_key_map
        {

        };

        std::unordered_map< AnimationKey, Animation > animations_as_map_;
        std::unordered_map< SingularTextureKey, Texture > textures_as_map_;
        std::unordered_map< VectorizedTextureKey, VectorizedTextures > vectorized_textures_as_map_;

        mutable std::unordered_map< std::pair<std::string, FontKey>, TextButtonTextures, PairHash > text_buttons_textures_as_map_;

        mutable std::unordered_map<std::string, Texture> dynamic_textures_;

        SDL_Renderer* renderer_;
        std::unordered_map<FontKey, FontManager> fonts_;

        template <typename KeyMap, typename StorageMap>
        auto get_ptr_by_path(const std::string& path, const KeyMap& key_map, const StorageMap& storage_map) const 
            -> const typename StorageMap::mapped_type*
        {
            auto it_key = key_map.find(path);
            if (it_key != key_map.end()) 
            {
                auto it_storage = storage_map.find(it_key->second);
                if (it_storage != storage_map.end()) 
                {
                    return &(it_storage->second);
                }
            }
            return nullptr;
        }

};

std::vector<std::string> get_file_names_from_folder(const std::string& folder_path);



#endif