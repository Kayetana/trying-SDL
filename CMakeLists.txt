cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(ProjectName sdl2)
project(${ProjectName})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

set(SOURCES src/engine/Engine.hpp
            src/engine/CollisionSystem.hpp
            src/engine/MovementSystem.hpp
            src/engine/Components.hpp
            src/engine/Entity.hpp
            src/main.cpp)

add_executable(${ProjectName} ${SOURCES})
target_link_libraries(${ProjectName} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_compile_options(-Wall -Werror -Wextra -Wpedantic)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
# set(CMAKE_BUILD_TYPE Debug)