#pragma once

#include <SFML/Graphics.hpp>
#include <complex>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include "config.h"
#include "colormap/colormap.hpp"

class Mandelbrot
{
public:
    Mandelbrot(int width, int height, std::string palleteName, bool palleteReversed);
    int run();

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
    std::string mPallete;
    bool mIsColorMapReversed;
    std::vector<std::string> mPalletes;
    sf::Vector2<float> mPlaneSize { 3.0, 3.0 };
    sf::Vector2<float> mPlaneCenter { -0.6, 0.0 };
    std::array<sf::Glsl::Vec4, CONFIG_ITERATION_LIMIT> mVec4Colors;
    static auto constexpr maxColorValue = 255;
};
