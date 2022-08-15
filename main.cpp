#include "mandelbrot.h"
#include <cstdio>

int main()
{
    Mandelbrot m(1000, 1000);
    return m.run();
}