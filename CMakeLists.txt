

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

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

#---------------------------------------------------------

add_library(base_module_library INTERFACE)

target_include_directories(base_module_library INTERFACE 
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Python_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/external/boost_headers"
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


add_executable(GameMakerApp 
  main.cpp 
  src/SDL_Management.cpp
  src/App.cpp
  src/EventManager.cpp
  src/GraphicsManager.cpp
  src/EditorMode.cpp
  src/GameMode.cpp
  src/Button.cpp
  src/EditorDataManager.cpp
  src/EditorMenu.cpp
  src/CanvaTile.cpp
  src/CanvaObject.cpp
  src/Animation.cpp
  src/PersistentState.cpp
  src/MainMenuMode.cpp
  src/Entity.cpp
  src/Land.cpp
  src/JsonLevelFormat.cpp
  src/WaterCanvaObject.cpp
  src/FileManagement.cpp
  src/WaterObject.cpp
  src/ChebFunc.cpp
  src/WaveSimulation.cpp )
target_link_libraries(GameMakerApp PRIVATE base_module_library)

set_target_properties(GameMakerApp PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE") #./build/GameMakerApp.exe

set(SDL2_DLLS
    "${SDL2_PATH}/lib/x64/SDL2.dll"
    "${SDL2_PATH}/lib/x64/SDL2_image.dll"
    "${SDL2_PATH}/lib/x64/SDL2_ttf.dll"
    "${SDL2_PATH}/lib/x64/SDL2_mixer.dll"
)

foreach(dll ${SDL2_DLLS})
    add_custom_command(TARGET GameMakerApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${dll}"
        $<TARGET_FILE_DIR:GameMakerApp>
        COMMENT "Kopiowanie ${dll} do folderu build..."
    )
endforeach()

add_executable(LandFilesLoader 
  land_files.cpp 
  src/FileManagement.cpp )
target_link_libraries(LandFilesLoader PRIVATE base_module_library)

set_target_properties(LandFilesLoader PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")  #./build/LandFilesLoader.exe

foreach(dll ${SDL2_DLLS})
    add_custom_command(TARGET LandFilesLoader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${dll}"
        $<TARGET_FILE_DIR:LandFilesLoader>
        COMMENT "Kopiowanie ${dll} do folderu build..."
    )
endforeach()

#---------------------------------------------------------

# pybind11_add_module(module_uno modules/module_uno.cpp src/MatrixBuilders.cpp)
# target_link_libraries(module_uno PRIVATE base_module_library)

# #---------------------------------------------------------

# pybind11_add_module(module_dos modules/module_dos.cpp src/WaveEquation.cpp src/MatrixBuilders.cpp) 
# target_link_libraries(module_dos PRIVATE base_module_library)

#---------------------------------------------------------

