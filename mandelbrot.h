#include <SFML/Graphics.hpp>
#include <complex>
class Mandelbrot
{
public:
    Mandelbrot(int width, int height);
    int run();

private:
    using Mitype = double;
    using Mtype = std::complex<Mitype>;

    int ptToIdx(int x, int y) const;
    int mandelbrot(Mtype c, int maxIterations) const;

    sf::Color getColor(int iterations, int maxIterations) const;
    const int mWidth;
    const int mHeight;
    int mMaxIterations = 200;
    sf::Vector2<double> mPlaneSize { 2.0, 2.0 };
    sf::Vector2<double> mPlaneCenter { 0.0, 0.0 };
    static auto constexpr maxColorValue = 255;
};