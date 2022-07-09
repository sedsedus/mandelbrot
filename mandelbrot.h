#include <SFML/Graphics.hpp>
#include <complex>
#include <unordered_map>

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

    int ptToIdx(int x, int y) const;
    int mandelbrot(Mtype c, int maxIterations) const;
    void handleEvent(sf::RenderWindow &window);
    Mitype mapToPlane(Mitype v, Mitype size, Mitype planeCenter, Mitype planeSize) const;
    void reloadLUT();
    std::unordered_map<int, Mtype> pointToMPoint;
    const int mWidth;
    const int mHeight;
    int mMaxIterations = 100;
    sf::Vector2<double> mPlaneSize { 3.0, 3.0 };
    sf::Vector2<double> mPlaneCenter { 0.0, 0.0 };
    static auto constexpr maxColorValue = 255;

};