
#include <SFML/Graphics.hpp>
#include <complex>
#include <cstdio>

using Mitype = double;
using Mtype = std::complex<Mitype>;

int mandelbrot(Mtype c, Mitype limit, int maxIterations)
{
    Mtype m = 0;
    auto i = 0;
    for (i = 0; i < maxIterations; ++i) {
        m = m * m + c;
        if (std::abs(m) > limit) {
            return i;
        }
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
    auto planeSize = 2.0;
    auto planeCenter = 0.0;
    auto maxIterations = 10;
    auto const maxValue = 10000;

    auto maxColorValue = 255;
    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot");

    while (window.isOpen()) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    maxIterations++;
                } else if (event.key.code == sf::Keyboard::Down) {
                    maxIterations--;
                } else if (event.key.code == sf::Keyboard::Left) {
                    planeSize--;
                    printf("planeSize: %0.1f\n", planeSize);
                } else if (event.key.code == sf::Keyboard::Right) {
                    planeSize++;
                    printf("planeSize: %0.1f\n", planeSize);
                } else if (event.type == sf::Event::MouseWheelMoved) {
                    float dzoom = static_cast<float>(-event.mouseWheel.delta);
                }
            }
        }

        auto mapToSize = [planeSize](auto v, auto size) { return mapToRange(v, 0.0, static_cast<Mitype>(size), -planeSize, planeSize); };

        sf::VertexArray pts;
        for (auto x = 0; x < width; ++x) {
            for (auto y = 0; y < height; ++y) {
                sf::Color color;
                auto xScaled = mapToSize(static_cast<Mitype>(x), static_cast<Mitype>(width));
                auto yScaled = mapToSize(static_cast<Mitype>(y), static_cast<Mitype>(height));
                Mtype p { xScaled, yScaled };
                if (std::abs(p) > 2) {
                    color = sf::Color::White;
                    // printf("X(x,y): (%0.2f, %0.2f)\n",xScaled,yScaled);
                } else {
                    auto iterations = mandelbrot(p, maxValue, maxIterations);
                    auto scaled = mapToRange((Mitype)iterations, 0.0, (Mitype)maxIterations, (Mitype)maxColorValue, 0.0);
                    color = sf::Color(scaled, scaled, scaled);
                    // printf("(x,y,it): (%0.2f, %0.2f, %d)\n",xScaled,yScaled,iterations);
                }
                pts.append(sf::Vertex({ static_cast<float>(x), static_cast<float>(y) }, color));
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