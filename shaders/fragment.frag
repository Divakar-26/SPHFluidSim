#version 330 core
in vec3 fColor; // from vertex shader
out vec4 FragColor;

uniform float uAlpha;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    if(length(coord) <= 0.5)
        FragColor = vec4(fColor, uAlpha); // 🔹 use fColor here
    else
        discard;
}
