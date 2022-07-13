#ifdef GL_ES
precision highp float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_size;
uniform vec2 u_center;
uniform int u_maxIterations;

vec2 complexPow2(vec2 n)
{
    return vec2(n.x * n.x - n.y * n.y, n.x * n.y + n.y * n.x);
}

float parabola(float x, float k)
{
    return pow(4.0 * x * (1.0 - x), k);
}

int getIterations()
{
    vec2 sc = gl_FragCoord.xy / u_resolution;
    vec2 p = sc * u_size + u_center - u_size / 2.0;
    vec2 m = vec2(0.0, 0.0);
    int i = 0;
    for (i = 0; i < u_maxIterations && sqrt(pow(m.x, 2.0) + pow(m.y, 2.0)) < 2.0; ++i) {
        m = complexPow2(m) + p;
    }
    return i;
}

void main()
{
    float it = (float)getIterations();
    float scaled = (it / u_maxIterations);
    gl_FragColor = vec4(scaled, scaled, scaled, 1.0);
}
