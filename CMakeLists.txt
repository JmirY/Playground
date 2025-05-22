cmake_minimum_required(VERSION 3.10)
project(playground C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    /opt/homebrew/include
    ${CMAKE_SOURCE_DIR}/include
)

# Source files
file(GLOB MAIN_SRC src/main.cpp)
file(GLOB PLAYGROUND_SRC src/playground/*)
file(GLOB PHYSICS_SRC src/physics/*)
file(GLOB GRAPHICS_SRC src/graphics/*)
file(GLOB GUI_SRC src/gui/*)

add_executable(playground
    ${MAIN_SRC}
    ${PLAYGROUND_SRC}
    ${PHYSICS_SRC}
    ${GRAPHICS_SRC}
    ${GUI_SRC}
)

# Definitions
target_compile_definitions(playground PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Link directories and libraries
link_directories(/opt/homebrew/lib)
find_package(glfw3 REQUIRED)
target_link_libraries(playground
    glfw
    "-framework OpenGL"
)
