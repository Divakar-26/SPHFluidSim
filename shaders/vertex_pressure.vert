#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in float aPressure;

uniform vec2 screenSize;
uniform float pointSize;
uniform float worldScale;

out float Pressure;

void main()
{
    // Convert world coordinates to pixels
    vec2 pixelPos = aPos * worldScale;

    // Convert pixel coordinates to NDC (-1 to 1)
    vec2 ndc = pixelPos / (screenSize * 0.5);
    ndc.y = -ndc.y; // flip Y for OpenGL

    gl_Position = vec4(ndc, 0.0, 1.0);
    gl_PointSize = pointSize * worldScale;
    
    // Pass pressure to fragment shader
    Pressure = aPressure;
}