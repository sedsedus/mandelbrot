cmake_minimum_required(VERSION 3.16)
project(Mandelbrot)

# add_custom_target(mandelbrot)

add_executable(mandelbrot)

target_include_directories(mandelbrot PRIVATE /usr/include/SFML)

find_package(SFML 2 COMPONENTS graphics system REQUIRED)

target_link_libraries(mandelbrot sfml-graphics sfml-audio)

target_compile_features(mandelbrot PRIVATE cxx_std_17)

target_sources(mandelbrot PRIVATE main.cpp)