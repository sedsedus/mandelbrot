#include <SFML/Graphics.hpp>
#include <complex>
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

    const int mWidth;
    const int mHeight;
    int mMaxIterations = 35;
    sf::Vector2<double> mPlaneSize { 3.0, 3.0 };
    sf::Vector2<double> mPlaneCenter { 0.0, 0.0 };
    static auto constexpr maxColorValue = 255;
};