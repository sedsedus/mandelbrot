#ifdef GL_ES
precision highp float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_size;
uniform vec2 u_center;
uniform int u_maxIterations;

// complex number (x + yi) squared is: x^2 - y^2 + 2xyi
vec2 complexPow2(vec2 n)
{
    return vec2(n.x * n.x - n.y * n.y, 2 * (n.x * n.y));
}

float parabola(float x, float k)
{
    return pow(4.0 * x * (1.0 - x), k);
}

// the stop condition is either maxIterations or distance (all mandelbrot points fall in 2.0 circle around center)
int getMandelbrotIterations(vec2 p)
{
    vec2 m = vec2(0.0, 0.0);
    int i = 0;
    // |zn| <= 2
    // |zn| = sqrt(x^2 + y^2)
    for (i = 0; i < u_maxIterations && sqrt(m.x * m.x + m.y * m.y) <= 2.0; ++i) {
        m = complexPow2(m) + p;
    }
    return i;
}

int getIterations()
{
    vec2 sc = gl_FragCoord.xy / u_resolution;
    vec2 p = sc * u_size + u_center - u_size / 2.0;
    return getMandelbrotIterations(p);
}

void main()
{
    float it = (float)getIterations();
    float scaled = (it / u_maxIterations);
    gl_FragColor = vec4(scaled, scaled, scaled, 1.0);
}
