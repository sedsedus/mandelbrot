
#include <SFML/Graphics.hpp>
#include <complex>
#include <cstdio>
#include <unordered_map>
#include "mandelbrot.h"
#include <functional>
#include <utility>

using Mitype = double;
using Mtype = std::complex<Mitype>;

int Mandelbrot::mandelbrot(Mtype c, int maxIterations) const
{
    Mtype m = 0;
    auto i = 0;
    for (i = 0; i < maxIterations && std::abs(m) < 2; ++i) {
        m = m * m + c;
    }
    return i;
}
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

void SetZoomAt(sf::RenderWindow &window, sf::Vector2i pos, float zoom)
{
    const sf::Vector2f beforeCoord { GetWorldMouse(window) };
    sf::View view { window.getDefaultView() };
    view.zoom(zoom);
    window.setView(view);

    const sf::Vector2f afterCoord { GetWorldMouse(window) };
    const sf::Vector2f offsetCoords { beforeCoord - afterCoord };
    view.move(offsetCoords);
    window.setView(view);
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
    auto getMapping = [iterations](const auto &mapping) {
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
    auto findAB = [](auto x1, auto y1, auto x2, auto y2) {
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
}

int Mandelbrot::run()
{
    sf::RenderWindow window(sf::VideoMode(mWidth, mHeight), "Mandelbrot");
    sf::VertexArray pts;
    std::unordered_map<int, Mtype> pointToMPoint;
    pts.resize(mWidth * mHeight);

    auto mapToSize = [](auto v, auto size, auto planeCenter, auto planeSize) {
        return mapToRange(v, 0.0, static_cast<Mitype>(size), planeCenter - planeSize / 2, planeCenter + planeSize / 2);
    };
    for (auto x = 0; x < mWidth; ++x) {
        for (auto y = 0; y < mHeight; ++y) {
            pts[ptToIdx(x, y)] = sf::Vertex({ static_cast<float>(x), static_cast<float>(y) }, sf::Color::White);
        }
    }
    auto loadLUT = [this, mapToSize, &pointToMPoint]() {
        for (auto x = 0; x < mWidth; ++x) {
            for (auto y = 0; y < mHeight; ++y) {
                auto xScaled = mapToSize(static_cast<Mitype>(x), static_cast<Mitype>(mWidth), mPlaneCenter.x, mPlaneSize.x);
                auto yScaled = mapToSize(static_cast<Mitype>(y), static_cast<Mitype>(mHeight), mPlaneCenter.y, mPlaneSize.y);
                Mtype p { xScaled, yScaled };
                pointToMPoint[ptToIdx(x, y)] = p;
            };
        }
    };
    loadLUT();
    while (window.isOpen()) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    mPlaneCenter.y -= mPlaneSize.y / 10;
                } else if (event.key.code == sf::Keyboard::Down) {
                    mPlaneCenter.y += mPlaneSize.y / 10;
                    mMaxIterations--;
                } else if (event.key.code == sf::Keyboard::Left) {
                    mPlaneCenter.x -= mPlaneSize.x / 10;
                } else if (event.key.code == sf::Keyboard::Right) {
                    mPlaneCenter.x += mPlaneSize.x / 10;
                } else if (event.key.code == sf::Keyboard::Add) {
                    mMaxIterations++;
                } else if (event.key.code == sf::Keyboard::Subtract) {
                    mMaxIterations--;
                } else if (event.key.code == sf::Keyboard::PageDown) {
                    mPlaneSize.x *= 0.9;
                    mPlaneSize.y *= 0.9;
                } else if (event.key.code == sf::Keyboard::PageUp) {
                    mPlaneSize.x *= 1.1;
                    mPlaneSize.y *= 1.1;
                } else if (event.type == sf::Event::MouseWheelMoved) {
                    // float dzoom = static_cast<float>(-event.mouseWheel.delta);
                }
                loadLUT();
            }
        }
        for (auto x = 0; x < mWidth; ++x) {
            for (auto y = 0; y < mHeight; ++y) {
                auto idx = ptToIdx(x, y);
                auto p = pointToMPoint[idx];
                auto iterations = mandelbrot(p, mMaxIterations);
                pts[idx].color = getColor(iterations, mMaxIterations);
            }
        }
        // draw it
        window.clear();
        window.draw(pts);
        window.display();
    }
    return 0;
}

int Mandelbrot::ptToIdx(int x, int y) const
{
    return x + y * mHeight;
}
