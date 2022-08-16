#include "mandelbrot.h"
#include <cstdio>

int main()
{
    Mandelbrot m(1000, 1000, "jet", true);
    return m.run();
}