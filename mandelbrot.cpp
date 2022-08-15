
#include <SFML/Graphics.hpp>
#include <complex>
#include <cstdio>
#include "mandelbrot.h"
#include <functional>
#include <utility>
#include "profile.h"

using Mitype = double;
using Mtype = std::complex<Mitype>;

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

sf::Color Mandelbrot::getColorOld(int iterations, int maxIterations)
{
    auto scaled1 = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)maxColorValue, 0.0);
    auto scaled2 = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)100, 0.0);
    auto scaled3 = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)maxColorValue, 0.0);
    return sf::Color(scaled1, scaled2, scaled3);
}

sf::Color Mandelbrot::getColor(int iterations, int maxIterations)
{
    struct mapping {
        sf::Vector2i range;
        std::function<int(int)> fun;
    };
    auto const getMapping = [iterations](const auto &mapping) {
        for (auto m : mapping) {
            if ((m.range.x <= iterations) && (iterations <= m.range.y)) {
                return m.fun(iterations);
            }
        }
        return 0;
    };

    // a = (y2-y1)/(x2-x1)
    // y = ax + b
    // b = y1 - a*x1
    auto const findAB = [](auto x1, auto y1, auto x2, auto y2) {
        auto a = (y2 - y1) / (x2 - x1);
        auto b = y1 - a * x1;
        return std::pair { a, b };
    };
    auto const first = sf::Vector2i { 0, maxIterations / 3 };
    auto const second = sf::Vector2i { maxIterations / 3, 2 * maxIterations / 3 };
    auto const third = sf::Vector2i { 2 * maxIterations / 3, maxIterations };

    auto const getLn = [findAB](auto range, bool rising) {
        auto y1 = rising ? 0.0 : maxColorValue;
        auto y2 = rising ? maxColorValue : 0.0;
        auto ab = findAB(range.x, y1, range.y, y2);
        return [ab](int i) { return ab.first * i + ab.second; };
    };
    std::vector<mapping> redColor = { { first,
                                        [](int i) {
                                            (void)i;
                                            return 0;
                                        } },
                                      { second, [second, getLn](int i) { return getLn(second, true)(i); } },
                                      { third, [](int i) {
                                           (void)i;
                                           return maxColorValue;
                                       } } };

    std::vector<mapping> greenColor = { { first, [first, getLn](int i) { return getLn(first, true)(i); } },
                                        { second,
                                          [](int i) {
                                              (void)i;
                                              return maxColorValue;
                                          } },
                                        { third, [third, getLn](int i) { return getLn(third, false)(i); } } };

    std::vector<mapping> blueColor = { { first,
                                         [](int i) {
                                             (void)i;
                                             return maxColorValue;
                                         } },
                                       { second, [second, getLn](int i) { return getLn(second, false)(i); } },
                                       { third, [](int i) {
                                            (void)i;
                                            return 0;
                                        } } };
    return sf::Color(getMapping(redColor), getMapping(greenColor), getMapping(blueColor));
};

Mandelbrot::Mandelbrot(int width, int height) : mWidth(width), mHeight(height)
{
    updateColorMap();
}

void Mandelbrot::handleEvent(sf::RenderWindow &window)
{
    sf::Event event;

    static bool isLeftPressed = false;
    static bool isRightPressed = false;
    static bool isMiddlePressed = false;
    static Vector2d mousePosWhenPres;
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
                setMaxIterations(std::clamp((int)(mMaxIterations * 1.1), mMaxIterations + 1, CONFIG_ITERATION_LIMIT));
            } else if (event.key.code == sf::Keyboard::Subtract) {
                setMaxIterations(std::clamp((int)(mMaxIterations * 0.9), 1, mMaxIterations - 1));
            } else if (event.key.code == sf::Keyboard::PageDown) {
                mPlaneSize.x *= 0.9;
                mPlaneSize.y *= 0.9;
            } else if (event.key.code == sf::Keyboard::PageUp) {
                mPlaneSize.x *= 1.1;
                mPlaneSize.y *= 1.1;
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

Mitype Mandelbrot::mapToPlane(Mitype v, Mitype size, Mitype planeCenter, Mitype planeSize) const
{
    return mapToRange(v, 0.0, static_cast<Mitype>(size), planeCenter - planeSize / 2, planeCenter + planeSize / 2);
}

Mitype Mandelbrot::mapToPlaneWidth(Mitype x) const
{
    return mapToPlane(static_cast<Mitype>(x), static_cast<Mitype>(mWidth), mPlaneCenter.x, mPlaneSize.x);
}

Mitype Mandelbrot::mapToPlaneHeight(Mitype y) const
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
    if (maxIterations >= CONFIG_ITERATION_LIMIT || maxIterations <= 1) {
        return;
    }
    mMaxIterations = maxIterations;
    updateColorMap();
}

void Mandelbrot::updateColorMap()
{
    for (auto i = 0; i <= mMaxIterations; ++i) {
        mVec4Colors[i] = getColor(i, mMaxIterations);
    }
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
