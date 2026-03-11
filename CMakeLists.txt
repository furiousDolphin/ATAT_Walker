

cmake_minimum_required(VERSION 3.15)
project(GameMaker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

#set(SDL2_PATH "E:/SDL2_VC/SDL2-2.32.4")
#set(CMAKE_PREFIX_PATH ${SDL2_PATH})

set(SDL2_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2")
set(CMAKE_PREFIX_PATH ${SDL2_PATH})


include(FetchContent)
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 5.0.0  
)
FetchContent_MakeAvailable(eigen)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
add_subdirectory(external/pybind11)

set(SDL2_IMAGE_PATH ${SDL2_PATH})
set(SDL2_TTF_PATH ${SDL2_PATH})
set(SDL2_MIXER_PATH ${SDL2_PATH})
set(SDL2_IMAGE_ROOT ${SDL2_PATH})

find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)
# find_package(SDL2_ttf REQUIRED)
# find_package(SDL2_mixer REQUIRED)

#---------------------------------------------------------

add_library(base_module_library INTERFACE)

target_include_directories(base_module_library INTERFACE 
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Python_INCLUDE_DIRS}
    "E:/boost_math_cpp/boost_1_88_0"
    "${SDL2_PATH}/include"
    "${SDL2_PATH}/include/SDL2"
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(base_module_library INTERFACE 
    eigen 
    Python::Module
    nlohmann_json
    "${SDL2_PATH}/lib/x64/SDL2.lib"
    "${SDL2_PATH}/lib/x64/SDL2main.lib"
    "${SDL2_PATH}/lib/x64/SDL2_image.lib"
    "${SDL2_PATH}/lib/x64/SDL2_ttf.lib"
    "${SDL2_PATH}/lib/x64/SDL2_mixer.lib"
    imm32.lib winmm.lib version.lib setupapi.lib
)

target_compile_definitions(base_module_library INTERFACE WITHOUT_NUMPY SDL_MAIN_HANDLED)

pybind11_add_module(module_uno 
    modules/module_uno.cpp
    src/SDL_Management.cpp
    src/App.cpp
    src/EventManager.cpp
    src/GraphicsManager.cpp
    src/GameMode.cpp
    src/Button.cpp
    src/PersistentState.cpp
    src/MainMenuMode.cpp
    src/FileManagement.cpp
    src/Animation.cpp
    src/Slider.cpp
    src/AT_AT.cpp
    src/ChebFunc.cpp
    src/SecondOrderSystem.cpp)

set(SDL2_DLLS
    "${SDL2_PATH}/lib/x64/SDL2.dll"
    "${SDL2_PATH}/lib/x64/SDL2_image.dll"
    "${SDL2_PATH}/lib/x64/SDL2_ttf.dll"
    "${SDL2_PATH}/lib/x64/SDL2_mixer.dll"
)

foreach(dll ${SDL2_DLLS})
    add_custom_command(TARGET module_uno POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${dll}"
        $<TARGET_FILE_DIR:module_uno>
        COMMENT "Kopiowanie ${dll} do folderu build..."
    )
endforeach()

target_link_libraries(module_uno PRIVATE base_module_library)


