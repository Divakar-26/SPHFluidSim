#version 330 core
out vec4 FragColor;

uniform float uAlpha;   // 🔹 Your input alpha (0.0 to 1.0)

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);

    if(length(coord) <= 0.5)
        FragColor = vec4(1.0, 0.0, 0.0, uAlpha); // use uAlpha
    else
        discard;
}
