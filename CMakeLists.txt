cmake_minimum_required(VERSION 3.16)
project(mandelbrot)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/SFML)

find_package(SFML 2 COMPONENTS graphics system REQUIRED)


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")

# Some cards may not be able to fit the color map for the default iteration limit. Decrease it in that case
set(ITERATION_LIMIT 1000)

configure_file(mandelbrotShader.frag.in mandelbrotShader.frag)

configure_file(config.h.in config.h)
 
# We need to include the build directory to be able to include config.h
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

# add colormap 
add_subdirectory(colormap)
target_include_directories(${PROJECT_NAME} PUBLIC colormap/include)

target_link_libraries(${PROJECT_NAME} sfml-graphics colormap)

target_sources(${PROJECT_NAME} PRIVATE main.cpp mandelbrot.cpp)