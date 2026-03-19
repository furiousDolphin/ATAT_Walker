
#include "GraphicsManager.hpp"

#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;


void GraphicsManager::init(SDL_Renderer* renderer, std::string base_path)
{
    renderer_ = renderer;
    base_path_ = base_path;
    fs::path root(base_path_);

    for ( const auto& [folder_path, key] : animations_as_path_key_map )
    { 
        std::vector<std::string> paths;
        fs::path full_folder_path = root / folder_path;

        for ( const auto& entry : fs::directory_iterator(full_folder_path) )
        {
            if ( entry.is_regular_file() && entry.path().extension() == ".png" )
            { paths.emplace_back(entry.path().string()); }
        }
        std::sort(paths.begin(), paths.end());

        auto shared_textures = std::make_shared<std::vector<Texture>>();

        for ( const auto& path : paths )
        { shared_textures->emplace_back(renderer, path); }

        animations_as_map_.emplace(key, Animation{shared_textures, 5, true}); 
    }


    for ( const auto& [folder_path, key] : vectorized_textures_as_path_key_map )
    { 
        fs::path full_folder_path = root / folder_path;
        vectorized_textures_as_map_.emplace(key, VectorizedTextures{renderer, full_folder_path.string()}); 
    }

    for ( const auto& [file_path, key] : singular_textures_as_path_key_map )
    { textures_as_map_[key] = Texture{renderer, (root / file_path).string()}; }


    fonts_.emplace( FontKey::MINECRAFT_18, FontManager( (root / "data/fonts/MinecraftBold-nMK1.otf").string(), 18 ) );
    fonts_.emplace( FontKey::MINECRAFT_24, FontManager( (root / "data/fonts/MinecraftBold-nMK1.otf").string(), 24 ) );
    fonts_.emplace( FontKey::MINECRAFT_36, FontManager( (root / "data/fonts/MinecraftBold-nMK1.otf").string(), 36 ) ); 
}   

Animation GraphicsManager::copy_animation( AnimationKey key ) const
{ return animations_as_map_.at(key); }

const Texture& GraphicsManager::get_texture( SingularTextureKey key ) const
{ return textures_as_map_.at(key); }

const GraphicsManager::VectorizedTextures& GraphicsManager::get_vectorized_textures( VectorizedTextureKey key) const
{ return vectorized_textures_as_map_.at(key); }

const GraphicsManager::TextButtonTextures* GraphicsManager::get_text_button_textures_ptr( const std::string& text, FontKey font_key) const
{
    std::pair<std::string, FontKey> key{text, font_key};

    auto it = text_buttons_textures_as_map_.find(key);
    if (it != text_buttons_textures_as_map_.end()) {
        return &(it->second);
    }

    auto& font_manager = fonts_.at(font_key);
    const TTF_Font* raw_font = font_manager.get_font(); 

    TextButtonTextures text_buttons_textures{ 
        Texture(renderer_, text, SDL_Color{ 0x00, 0x00, 0xff }, raw_font),
        Texture(renderer_, text, SDL_Color{ 0x00, 0xff, 0x00 }, raw_font)
    };

    auto [inserted_it, success] = text_buttons_textures_as_map_.emplace(key, std::move(text_buttons_textures));
    return &(inserted_it->second);
}

const Texture* GraphicsManager::get_dynamic_texture_ptr(const std::string& file_path) const
{
    auto it = dynamic_textures_.find(file_path);
    if ( it != dynamic_textures_.end() )
    { return &(it->second); }

    try 
    {
        Texture texture{renderer_, base_path_ + file_path};
        auto [inserted_it, success] =  dynamic_textures_.emplace(file_path, std::move(texture));
        return &(inserted_it->second);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return nullptr;
    }
}

GraphicsManager::FoundItems GraphicsManager::find_items_by_path(const std::string& path) const
{
    if ( const auto* tex = this->get_ptr_by_path(path, singular_textures_as_path_key_map, textures_as_map_) )
    { return tex; }
    if ( const auto* vec = this->get_ptr_by_path(path, vectorized_textures_as_path_key_map, vectorized_textures_as_map_) )
    { return vec; }
    
    auto animation_it = animations_as_path_key_map.find(path);
    if ( animation_it != animations_as_path_key_map.end() )
    {
        const auto& [_, key] = *animation_it;
        const auto& animation = animations_as_map_.at(key);
        return animation.copy(); 
    }
    return std::monostate{};
}

GraphicsManager::VectorizedTextures::VectorizedTextures(SDL_Renderer* renderer, const std::string& folder_path)
{
    
    std::vector<fs::path> paths;
    for ( const auto& entry : fs::directory_iterator(folder_path) )
    {
        if ( entry.is_regular_file() && entry.path().extension() == ".png" )
        { paths.push_back(entry.path()); }
    }

    std::sort(paths.begin(), paths.end());

    int index = 0;
    for ( const fs::path& path : paths )
    {
        std::string stem = path.stem().string();
        textures_.emplace_back(renderer, path.string());
        name_to_index_translator_[stem] = index++;
    }
}

const Texture& GraphicsManager::VectorizedTextures::operator[](int index) const
{ return textures_[index]; }

std::optional<int> GraphicsManager::VectorizedTextures::get_index_from_name(const std::string& texture_name) const
{
    auto it = name_to_index_translator_.find(texture_name);
    return (it != name_to_index_translator_.end()) ? std::optional<int>(it->second) : std::nullopt;
}

std::vector<std::string> get_file_names_from_folder(const std::string& folder_path)
{
    std::vector<std::string> result;

    try
    {  
        for( const auto& entry : fs::directory_iterator( folder_path ) )
        {
            if( entry.is_regular_file() && entry.path().extension() == ".png" )
            { result.push_back( entry.path().filename().string() ); }
        }
    }
    catch(const std::exception& e)
    { std::cerr << e.what() << '\n'; }

    std::sort(
        result.begin(), 
        result.end(), 
        [](const std::string& a, const std::string& b) 
        {
            return std::lexicographical_compare(
                a.begin(), 
                a.end(), 
                b.begin(), 
                b.end(),
                [](char c1, char c2)
                { return std::tolower(c1) < std::tolower(c2); });
        });

    return result;  
}
