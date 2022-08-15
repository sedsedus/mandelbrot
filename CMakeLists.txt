cmake_minimum_required(VERSION 3.16)
project(Mandelbrot)

add_executable(mandelbrot)

target_include_directories(mandelbrot PRIVATE /usr/include/SFML)

find_package(SFML 2 COMPONENTS graphics system REQUIRED)

target_link_libraries(mandelbrot sfml-graphics pthread)

target_compile_features(mandelbrot PRIVATE cxx_std_17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")

# Some cards may not be able to fit the color map for the default iteration limit. Decrease it in that case
set(ITERATION_LIMIT 1000)

configure_file(mandelbrotShader.frag.in mandelbrotShader.frag)

configure_file(config.h.in config.h)

# We need to include the build directory to be able to include config.h
target_include_directories(mandelbrot PUBLIC "${PROJECT_BINARY_DIR}")

target_sources(mandelbrot PRIVATE main.cpp mandelbrot.cpp)