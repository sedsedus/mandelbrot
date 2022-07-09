#include "mandelbrot.h"
#include <cstdio>

int main()
{
    Mandelbrot m(500, 500);
    return m.run();
    for (int i = 0; i < 31; ++i) {
        auto c = m.getColor(i, 30);
        printf("%-2d | %-3d, %-3d, %-3d\n",i,  c.r, c.g, c.b);
    }
}