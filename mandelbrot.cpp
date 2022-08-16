// mandelbrot -- ineractive mandelbrot set explorer
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
#include <cstdio>
#include "mandelbrot.h"
#include <functional>
#include <utility>
#include "profile.h"
#include "colormap/palettes.hpp"

template <typename T>
T constexpr mapToRange(T v, T vMin, T vMax, T toMin, T toMax)
{
    return (v - vMin) / (vMax - vMin) * (toMax - toMin) + toMin;
}

static_assert(mapToRange(1, 0, 1, 2, 3) == 3);
static_assert(mapToRange(800, 0, 800, -1, 1) == 1);
static_assert(mapToRange(0, 0, 800, -1, 1) == -1);
// static_assert(mapToRange(0, 400, 800, -1, 1) == 0.5);

sf::Vector2f GetWorldMouse(sf::RenderWindow &window)
{
    auto mouse = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mouse);
    return worldPos;
}

Mandelbrot::Mandelbrot(int width, int height, std::string palleteName, bool palleteReversed)
    : mWidth(width), mHeight(height), mPallete(palleteName), mIsColorMapReversed(palleteReversed)
{
    updateColorMap();
    for (auto [p, _] : colormap::palettes) {
        mPalletes.push_back(p);
    }
}

void Mandelbrot::handleEvent(sf::RenderWindow &window)
{
    sf::Event event;

    static bool isLeftPressed = false;
    static bool isRightPressed = false;
    static bool isMiddlePressed = false;
    static Vector2d mousePosWhenPres;
    auto static selectedPallete = 0;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                mPlaneCenter.y -= mPlaneSize.y / 10;
            } else if (event.key.code == sf::Keyboard::Down) {
                mPlaneCenter.y += mPlaneSize.y / 10;
            } else if (event.key.code == sf::Keyboard::Left) {
                mPlaneCenter.x -= mPlaneSize.x / 10;
            } else if (event.key.code == sf::Keyboard::Right) {
                mPlaneCenter.x += mPlaneSize.x / 10;
            } else if (event.key.code == sf::Keyboard::Add) {
                setMaxIterations(std::clamp((int)(mMaxIterations * 1.1), mMaxIterations + 1, CONFIG_ITERATION_LIMIT - 1));
            } else if (event.key.code == sf::Keyboard::Subtract) {
                setMaxIterations(std::clamp((int)(mMaxIterations * 0.9), 1, mMaxIterations - 1));
            } else if (event.key.code == sf::Keyboard::PageDown) {
                mPlaneSize.x *= 0.9;
                mPlaneSize.y *= 0.9;
            } else if (event.key.code == sf::Keyboard::PageUp) {
                mPlaneSize.x *= 1.1;
                mPlaneSize.y *= 1.1;
            } else if (event.key.code == sf::Keyboard::Numpad8) {
                selectedPallete = (selectedPallete + 1) % mPalletes.size();
                mPallete = mPalletes[selectedPallete];
                updateColorMap();
            } else if (event.key.code == sf::Keyboard::Numpad2) {
                selectedPallete = (selectedPallete - 1) % mPalletes.size();
                mPallete = mPalletes[selectedPallete];
                updateColorMap();
            } else if (event.key.code == sf::Keyboard::R) {
                mIsColorMapReversed ^= true;
                updateColorMap();
            }
        } else if (event.type == sf::Event::MouseWheelMoved) {
            float dzoom = static_cast<float>(-event.mouseWheel.delta) / 10;
            auto mpoint = getPlaneMouse(window);
            mPlaneSize.x *= (1 + dzoom);
            mPlaneSize.y *= (1 + dzoom);
            auto mpointz = getPlaneMouse(window);
            auto diff = mpoint - mpointz;
            mPlaneCenter.x += diff.x;
            mPlaneCenter.y += diff.y;
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                mousePosWhenPres = getPlaneMouse(window);
                isLeftPressed = true;
            } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                isRightPressed = true;
            } else if (event.mouseButton.button == sf::Mouse::Button::Middle) {
                isMiddlePressed = true;
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                isLeftPressed = false;
            } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                isRightPressed = false;
            } else if (event.mouseButton.button == sf::Mouse::Button::Middle) {
                isMiddlePressed = false;
            }
        } else if (event.type == sf::Event::MouseMoved) {
            // printf("Move world (%f, %f) mapped (%f, %f)\n", event.mouseMove.x, event.mouseMove.y);
            if (isLeftPressed) {
                auto mousePos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
                auto mouse = window.mapPixelToCoords(mousePos);
                Vector2d curPos = { mapToPlaneWidth(mouse.x), mapToPlaneHeight(mouse.y) };
                auto diff = mousePosWhenPres - curPos;
                mPlaneCenter.x += diff.x;
                mPlaneCenter.y += diff.y;
                mousePosWhenPres = getPlaneMouse(window);
            }
        }
    }
}

Mandelbrot::Mitype Mandelbrot::mapToPlane(Mitype v, Mitype size, Mitype planeCenter, Mitype planeSize) const
{
    return mapToRange(v, 0.0, static_cast<Mitype>(size), planeCenter - planeSize / 2, planeCenter + planeSize / 2);
}

Mandelbrot::Mitype Mandelbrot::mapToPlaneWidth(Mitype x) const
{
    return mapToPlane(static_cast<Mitype>(x), static_cast<Mitype>(mWidth), mPlaneCenter.x, mPlaneSize.x);
}

Mandelbrot::Mitype Mandelbrot::mapToPlaneHeight(Mitype y) const
{
    return mapToPlane(static_cast<Mitype>(y), static_cast<Mitype>(mHeight), mPlaneCenter.y, mPlaneSize.y);
}

Mandelbrot::Vector2d Mandelbrot::getPlaneMouse(sf::RenderWindow &window) const
{
    auto wmouse = GetWorldMouse(window);
    return { mapToPlaneWidth(wmouse.x), mapToPlaneHeight(wmouse.y) };
}

void Mandelbrot::setMaxIterations(int maxIterations)
{
    if (maxIterations >= CONFIG_ITERATION_LIMIT || maxIterations <= 1 || maxIterations == mMaxIterations) {
        return;
    }
    mMaxIterations = maxIterations;
    updateColorMap();
}

void Mandelbrot::updateColorMap()
{
    auto const &colorMap = colormap::palettes.at(mPallete);

    for (auto i = 0; i <= mMaxIterations; ++i) {
        auto ratio = (double)i / mMaxIterations;
        auto v = colorMap(mIsColorMapReversed ? 1 - ratio : ratio);
        mVec4Colors[i] = sf::Color(v.getRed().getValue(), v.getGreen().getValue(), v.getBlue().getValue());
    }
    printf("Using colormap '%s'%s max iterations: %d\n", mPallete.c_str(), mIsColorMapReversed ? "(reversed)" : "", mMaxIterations);
}

int Mandelbrot::run()
{
    sf::RenderWindow window(sf::VideoMode(mWidth, mHeight), "Mandelbrot");
    auto sizeV = sf::Vector2f { (float)mWidth, (float)mHeight };
    sf::Shader shader;
    sf::RectangleShape r(sizeV);

    if (!shader.loadFromFile("mandelbrotShader.frag", sf::Shader::Fragment)) {
        printf("Error loading shader");
    }

    if (!shader.isAvailable()) {
        printf("Shaders not available");
    }

    shader.setUniform("u_resolution", sizeV);
    while (window.isOpen()) {
        handleEvent(window);
        window.clear();

        shader.setUniform("u_size", mPlaneSize);
        shader.setUniform("u_center", sf::Vector2f(mPlaneCenter.x, -mPlaneCenter.y)); // shaders have inverted x in respect to sfml
        shader.setUniform("u_maxIterations", mMaxIterations);
        shader.setUniformArray("u_colors", mVec4Colors.data(), CONFIG_ITERATION_LIMIT);

        window.draw(r, &shader);
        window.display();
    }
    return 0;
}
