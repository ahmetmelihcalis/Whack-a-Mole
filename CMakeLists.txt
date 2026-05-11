cmake_minimum_required(VERSION 3.16)

project(WhackAMole CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} 
    src/main.cpp 
    src/Game.cpp
    src/Mole.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL2::SDL2main 
    SDL2::SDL2 
    -lSDL2_image
    -lSDL2_ttf
)