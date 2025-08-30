#include "Particle.h"
#include <math.h>
#include <random>
#include <algorithm>
#include <SDL3/SDL.h>

const float GRAVITY = 9.8f;

// Initialize these global variables

// Initialize static members
float Particle::targetDensity = 2.0f;
float Particle::pressureMultiplier = 10.0f;

Particle::Particle(int W, int H) : WINDOW_W(W), WINDOW_H(H)
{
    radius = 0.01f;
    particleSpacing = 0.0f;
    smoothingRadius = 0.5f;

    float halfW = (W / 2.0f) / 100.0f;
    float halfH = (H / 2.0f) / 100.0f;

    srand(time(NULL));
    for (int i = 0; i < numParticles; i++)
    {
        float rx = static_cast<float>(rand()) / RAND_MAX;
        float ry = static_cast<float>(rand()) / RAND_MAX;

        float x = (rx * 2.0f - 1.0f) * halfW;
        float y = (ry * 2.0f - 1.0f) * halfH;

        position.push_back({x, y});
        velocite.push_back({0.0f, 0.0f});
        properties.push_back(1.0f);
    }

    // Pre-calculate initial densities
    updateDensities();
}

Particle::~Particle()
{
}

void Particle::update(float dt)
{
    if (running)
    {
        // 1. First update densities based on current positions
        updateDensities();
        updatePressures();

        // 2. Calculate and apply pressure forces
        for (int i = 0; i < numParticles; i++)
        {
            glm::vec2 pressureForce = calculatePressureForce(i); // Use particle index
            velocite[i] += (pressureForce / densities[i]) * dt;
        }

        // 3. Apply gravity and other external forces
        // for (int i = 0; i < numParticles; i++)
        // {
        //     velocite[i].y += GRAVITY * dt;

        //     // Optional: Add some damping to prevent excessive movement
        //     velocite[i] *= 0.99f;
        // }

        // 4. Update positions
        for (int i = 0; i < numParticles; i++)
        {
            position[i] += velocite[i] * dt;
        }

        // Define world boundaries
        float worldLeft = -((float)WINDOW_W / 2.0f) / 100.0f;
        float worldRight = ((float)WINDOW_W / 2.0f) / 100.0f;
        float worldBottom = -((float)WINDOW_H / 2.0f) / 100.0f;
        float worldTop = ((float)WINDOW_H / 2.0f) / 100.0f;

        // Collision margin to prevent sticking
        float margin = radius;

        // Loop through all particles
        for (int i = 0; i < numParticles; i++)
        {
            // Bottom
            if (position[i].y - margin < worldBottom)
            {
                position[i].y = worldBottom + margin;
                velocite[i].y *= -0.5f; // Bounce with damping
                velocite[i].x *= 0.8f;  // Friction
            }

            // Top
            if (position[i].y + margin > worldTop)
            {
                position[i].y = worldTop - margin;
                velocite[i].y *= -0.5f;
                velocite[i].x *= 0.8f;
            }

            // Left
            if (position[i].x - margin < worldLeft)
            {
                position[i].x = worldLeft + margin;
                velocite[i].x *= -0.5f;
                velocite[i].y *= 0.8f;
            }

            // Right
            if (position[i].x + margin > worldRight)
            {
                position[i].x = worldRight - margin;
                velocite[i].x *= -0.5f;
                velocite[i].y *= 0.8f;
            }
        }
    }
}

void Particle::updateDensities()
{
    densities.resize(numParticles);
    for (int i = 0; i < numParticles; i++)
    {
        densities[i] = calculateDensity(position[i]);
    }
}

void Particle::OnEvent(SDL_Event &e)
{
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        float mx, my;
        SDL_GetMouseState(&mx, &my);

        // Correct Y coordinate conversion (OpenGL has Y going up)
        float worldX = (mx - WINDOW_W / 2.0f) / 100.0f;
        float worldY = (WINDOW_H / 2.0f - my) / 100.0f;

        std::cout << "Screen: (" << mx << ", " << my << ") ";
        std::cout << "World: (" << worldX << ", " << worldY << ")" << std::endl;

        float z = calculateDensity({worldX, worldY});
        std::cout << "Density: " << z << std::endl;

        // Also show pressure at that point
        float pressure = convertDensityToPressure(z);
        std::cout << "Pressure: " << pressure << std::endl;
    }
}

void Particle::MakeGrid()
{
    position.clear();
    velocite.clear();
    properties.clear();

    int ppr = (int)std::sqrt(numParticles);
    int ppc = (numParticles - 1) / ppr + 1;
    float spacing = 2.0f * radius + particleSpacing;

    for (int i = 0; i < numParticles; i++)
    {
        float x = (i % ppr - ppr / 2.0f + 0.5f) * spacing;
        float y = (i / ppr - ppc / 2.0f + 0.5f) * spacing;

        position.push_back({x, y});
        velocite.push_back({0.0f, 0.0f});
        properties.push_back(1.0f);
    }

    // Update densities after making grid
    updateDensities();
}

float Particle::smoothingKernel(float sr, float dst)
{
    if (dst >= sr)
        return 0.0f;

    float normalizedDistance = dst / sr;
    float normalizedValue = 1.0f - normalizedDistance * normalizedDistance;
    float normalization = 4.0f / (M_PI * pow(sr, 4));

    return normalization * normalizedValue * normalizedValue * normalizedValue;
}

glm::vec2 Particle::smoothingKernelGradient(float sr, glm::vec2 vec)
{
    float dst = glm::length(vec);
    if (dst <= 0.0f || dst >= sr)
        return glm::vec2(0.0f);

    float term = (1.0f - (dst * dst) / (sr * sr));
    float coeff = -24.0f / (M_PI * pow(sr, 6));
    float dWdr = coeff * dst * term * term;

    return (dWdr / dst) * vec;
}

float Particle::calculateDensity(glm::vec2 samplePoint)
{
    float mass = 1.0f;
    float density = 0.0f;
    float sr = std::max(0.1f, smoothingRadius);

    for (size_t i = 0; i < position.size(); i++)
    {
        glm::vec2 vec = position[i] - samplePoint;
        float dst = glm::length(vec);

        if (dst < sr)
        {
            density += mass * smoothingKernel(sr, dst);
        }
    }

    return density;
}

float Particle::calculateProperty(glm::vec2 point)
{
    float mass = 1.0f;
    float property = 0.0f;

    for (int i = 0; i < numParticles; i++)
    {
        glm::vec2 vec = point - position[i];
        float dst = glm::length(vec);

        if (densities[i] > 0.0f && dst < smoothingRadius)
        {
            float weight = smoothingKernel(smoothingRadius, dst);
            property += -(properties[i] * (mass / densities[i])) * weight;
        }
    }

    return property;
}

glm::vec2 Particle::calculatePressureForce(int particleIndex)
{
    glm::vec2 pressureForce(0.0f);
    float mass = 1.0f;
    glm::vec2 samplePoint = position[particleIndex];

    for (int i = 0; i < numParticles; i++)
    {
        if (i == particleIndex)
            continue; // Skip self

        glm::vec2 vec = samplePoint - position[i];
        float dst = glm::length(vec);

        if (dst > 0.0f && dst < smoothingRadius && densities[i] > 0.0f)
        {
            glm::vec2 gradW = smoothingKernelGradient(smoothingRadius, vec);

            // Calculate shared pressure
            float sharedPressure = (convertDensityToPressure(densities[particleIndex]) +
                                    convertDensityToPressure(densities[i])) /
                                   2.0f;

            pressureForce += -(sharedPressure * (mass / densities[i]) * gradW);
        }
    }

    return pressureForce;
}

float Particle::convertDensityToPressure(float density)
{
    float densityError = density - targetDensity;
    float pressure = densityError * pressureMultiplier;
    return pressure;
}

void Particle::updatePressures()
{
    pressures.resize(numParticles);
    for (int i = 0; i < numParticles; i++)
    {
        pressures[i] = convertDensityToPressure(densities[i]);
    }
}