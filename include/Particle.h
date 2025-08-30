#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <SDL3/SDL.h>

// Declare these extern so they can be accessed from other files
extern float targetDensity;
extern float pressureMultiplier;

class Particle
{
public:
    Particle(int W, int H);
    ~Particle();

    void update(float dt);
    void OnEvent(SDL_Event &e);

    float &GetRadius() { return radius; }
    std::vector<glm::vec2> &GetPositions() { return position; }

    void MakeGrid();
    float smoothingKernel(float sR, float dst);
    glm::vec2 smoothingKernelGradient(float sr, glm::vec2 vec);
    float calculateDensity(glm::vec2 point);
    float calculateProperty(glm::vec2 point);
    glm::vec2 calculatePressureForce(int particleIndex); // Changed to take particle index
    float convertDensityToPressure(float density);

    // Helper method
    void updateDensities();

    // Add this to your Particle class
    std::vector<float> pressures; // Store pressure for each particle

    // Add this method
    void updatePressures();

    std::vector<float> properties;
    std::vector<float> densities;

    bool running = false;
    float radius = 1.0f;
    float alpha = 1.0f;
    float blurry = 0.0f;
    int numParticles = 500;
    float particleSpacing = 0.0f;
    float smoothingRadius = 0.0f;
    static float targetDensity;
    static float pressureMultiplier;

private:
    std::vector<glm::vec2> position;
    std::vector<glm::vec2> velocite;
    int WINDOW_W, WINDOW_H;
};