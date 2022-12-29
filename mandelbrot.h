#pragma once

// mandelbrot -- interactive mandelbrot set explorer
// Copyright (C) 2022 sedsedus
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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
