#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor; // 🔹 add color attribute

out vec3 fColor; // pass to fragment shader

uniform vec2 screenSize;
uniform float pointSize;
uniform float worldScale;

void main()
{
    vec2 pixelPos = aPos * worldScale;
    vec2 ndc = pixelPos / (screenSize * 0.5);
    ndc.y = -ndc.y;

    gl_Position = vec4(ndc, 0.0, 1.0);
    gl_PointSize = pointSize * worldScale;

    fColor = aColor; // 🔹 pass color to fragment
}
