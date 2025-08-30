#version 330 core
out vec4 FragColor;

in float Pressure;

uniform float uAlpha;
uniform float targetDensity;
uniform float pressureMultiplier;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    
    if(length(coord) > 0.5)
        discard;
    
    // USE targetDensity in your calculation to prevent optimization
    float maxPressure = targetDensity * pressureMultiplier * 2.0; // Now using targetDensity!
    float normalizedPressure = clamp(Pressure / maxPressure, -1.0, 1.0);
    
    // Alternative: Use targetDensity to determine color thresholds
    float pressureThreshold = targetDensity * pressureMultiplier * 0.1;
    
    vec3 color;
    if (Pressure < -pressureThreshold) {
        // Blue for significant negative pressure (compression)
        float intensity = clamp(abs(Pressure) / (targetDensity * pressureMultiplier), 0.0, 1.0);
        color = mix(vec3(0.0, 0.0, 0.3), vec3(0.0, 0.0, 1.0), intensity);
    } 
    else if (Pressure > pressureThreshold) {
        // Red for significant positive pressure (expansion)
        float intensity = clamp(Pressure / (targetDensity * pressureMultiplier), 0.0, 1.0);
        color = mix(vec3(0.3, 0.0, 0.0), vec3(1.0, 0.0, 0.0), intensity);
    } 
    else {
        // Green for near-target density (neutral pressure)
        color = vec3(0.0, 1.0, 0.0);
    }
    
    FragColor = vec4(color, uAlpha);
}