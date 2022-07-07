
#include <SFML/Graphics.hpp>
#include <complex>
#include <cstdio>

using Mitype = double;
using Mtype = std::complex<Mitype>;

int mandelbrot(Mtype c, int maxIterations)
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
int main()
{
    auto constexpr width = 1000;
    auto constexpr height = 1000;
    auto const maxColorValue = 255;
    auto planeXSize = 2.0;
    auto planeYSize = 2.0;
    auto planeXCenter = 0.0;
    auto planeYCenter = 0.0;
    auto maxIterations = 200;
    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot");

    sf::VertexArray pts;
    pts.resize(width * height);
    for (auto x = 0; x < width; ++x) {
        for (auto y = 0; y < height; ++y) {
            pts[x + height * y] = sf::Vertex({ static_cast<float>(x), static_cast<float>(y) }, sf::Color::White);
        };
    }
    while (window.isOpen()) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    planeYCenter -= planeYSize / 10;
                } else if (event.key.code == sf::Keyboard::Down) {
                    planeYCenter += planeYSize / 10;
                    maxIterations--;
                } else if (event.key.code == sf::Keyboard::Left) {
                    planeXCenter -= planeXSize / 10;
                } else if (event.key.code == sf::Keyboard::Right) {
                    planeXCenter += planeXSize / 10;
                } else if (event.key.code == sf::Keyboard::Add) {
                    maxIterations++;
                } else if (event.key.code == sf::Keyboard::Subtract) {
                    maxIterations--;
                } else if (event.key.code == sf::Keyboard::PageDown) {
                    planeXSize *= 0.9;
                    planeYSize *= 0.9;
                } else if (event.key.code == sf::Keyboard::PageUp) {
                    planeXSize *= 1.1;
                    planeYSize *= 1.1;
                } else if (event.type == sf::Event::MouseWheelMoved) {
                    // float dzoom = static_cast<float>(-event.mouseWheel.delta);
                }
            }
        }

        auto mapToSize = [](auto v, auto size, auto planeCenter, auto planeSize) {
            return mapToRange(v, 0.0, static_cast<Mitype>(size), planeCenter - planeSize / 2, planeCenter + planeSize / 2);
        };
        auto getColor = [maxIterations, maxColorValue](int iterations) {
            auto scaled1 = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)maxColorValue, 0.0);
            auto scaled2 = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)100, 0.0);
            auto scaled3 = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)maxColorValue, 0.0);
            return sf::Color(scaled1, scaled2, scaled3);
        };
        for (auto x = 0; x < width; ++x) {
            for (auto y = 0; y < height; ++y) {
                auto xScaled = mapToSize(static_cast<Mitype>(x), static_cast<Mitype>(width), planeXCenter, planeXSize);
                auto yScaled = mapToSize(static_cast<Mitype>(y), static_cast<Mitype>(height), planeYCenter, planeYSize);
                Mtype p { xScaled, yScaled };
                auto iterations = mandelbrot(p, maxIterations);
                // auto scaled = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)maxColorValue, 0.0);
                auto color = getColor(iterations);
                // color = sf::Color(scaled, scaled, 1/2*scaled);
                // printf("(x,y,it): (%0.2f, %0.2f, %d)\n",xScaled,yScaled,iterations);
                pts[x + height * y].color = color;
                // printf("%d: %0.2f\n", i, mandelbrot(i, 5).real());
            }
        }
        // draw it
        window.clear();
        window.draw(pts);
        window.display();
    }
    return 0;
}