#pragma once

#include <SFML/Graphics.hpp>
#include <complex>
#include <utility>
#include <vector>

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

    int ptToIdx(int x, int y) const;
    int mandelbrot(Mtype c, int maxIterations) const;
    void handleEvent(sf::RenderWindow &window);
    Mitype mapToPlane(Mitype v, Mitype size, Mitype planeCenter, Mitype planeSize) const;
    Mitype mapToPlaneWidth(Mitype v) const;
    Mitype mapToPlaneHeight(Mitype v) const;
    Vector2d getPlaneMouse(sf::RenderWindow &window) const;
    void calcMandelbrot(sf::VertexArray &pts);
    const int mWidth;
    const int mHeight;
    int mMaxIterations = 100;
    sf::Vector2<Mitype> mPlaneSize { 3.0, 3.0 };
    sf::Vector2<Mitype> mPlaneCenter { 0.0, 0.0 };
    static auto constexpr maxColorValue = 255;
    bool mRecalcNeeded{true};
    std::vector<std::pair<int, int>> mPixelPos;
};
