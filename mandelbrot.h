#pragma once

#include <SFML/Graphics.hpp>
#include <complex>
#include <utility>
#include <vector>
#include <array>
#include "config.h"

class Mandelbrot
{
public:
    Mandelbrot(int width, int height);
    int run();
    static sf::Color getColor(int iterations, int maxIterations);
    static sf::Color getColorOld(int iterations, int maxIterations);

private:
    using Mitype = double;
    using Mtype = std::complex<Mitype>;
    using Vector2d = sf::Vector2<double>;

    void handleEvent(sf::RenderWindow &window);
    Mitype mapToPlane(Mitype v, Mitype size, Mitype planeCenter, Mitype planeSize) const;
    Mitype mapToPlaneWidth(Mitype v) const;
    Mitype mapToPlaneHeight(Mitype v) const;
    Vector2d getPlaneMouse(sf::RenderWindow &window) const;
    void setMaxIterations(int maxIterations);
    void updateColorMap();

    const int mWidth;
    const int mHeight;
    int mMaxIterations = 100;
    sf::Vector2<float> mPlaneSize { 3.0, 3.0 };
    sf::Vector2<float> mPlaneCenter { -0.6, 0.0 };
    std::array<sf::Glsl::Vec4, CONFIG_ITERATION_LIMIT> mVec4Colors;
    static auto constexpr maxColorValue = 255;
};
