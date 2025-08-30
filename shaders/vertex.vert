#version 330 core
layout(location = 0) in vec2 aPos;

uniform vec2 screenSize;   // width and height in pixels
uniform float pointSize;
uniform float worldScale;  // how many pixels per world unit

void main()
{
    // Convert world coordinates to pixels
    vec2 pixelPos = aPos * worldScale;

    // Convert pixel coordinates to NDC (-1 to 1)
    vec2 ndc = pixelPos / (screenSize * 0.5);
    ndc.y = -ndc.y; // flip Y for OpenGL

    gl_Position = vec4(ndc, 0.0, 1.0);
    gl_PointSize = pointSize * worldScale;
}
