#include "Particle.h"
#include <math.h>
#include <random>
#include <algorithm>
#include <SDL3/SDL.h>

float Particle::targetDensity = 2.0f;
float Particle::pressureMultiplier = 10.0f;

bool leftMouseDown = false;
bool rightMouseDown = false;

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
        predictedPosition.push_back({x, y});
    }

    updateDensities(position);
    speed.resize(numParticles, 0.0f);
    cellSize = smoothingRadius;
}

Particle::~Particle()
{
}

void Particle::update(float dt)
{
    if (running)
    {
        int iterations = 2;
        float sub_dt = dt / iterations;

        for (int iter = 0; iter < iterations; iter++)
        {
            for (int i = 0; i < numParticles; i++)
            {
                predictedPosition[i] = position[i] + velocite[i] * sub_dt;
            }

            buildSpatialGrid(predictedPosition);
            updateDensities(predictedPosition);
            updatePressures();

            applyContinuousMousePressure();

            for (int i = 0; i < numParticles; i++)
            {
                glm::vec2 pressureForce = calculatePressureForce(i);
                velocite[i] += (pressureForce / (densities[i] + 1e-6f)) * sub_dt;
            }

            for (int i = 0; i < numParticles; i++)
            {
                position[i] += velocite[i] * sub_dt;
            }
        }

        for (int i = 0; i < numParticles; i++)
        {
            velocite[i].y += GRAVITY * dt;
        }
        for (int i = 0; i < numParticles; i++)
        {
            speed[i] = glm::length(velocite[i]);
        }

        float worldLeft = -((float)WINDOW_W / 2.0f) / 100.0f;
        float worldRight = ((float)WINDOW_W / 2.0f) / 100.0f;
        float worldBottom = -((float)WINDOW_H / 2.0f) / 100.0f;
        float worldTop = ((float)WINDOW_H / 2.0f) / 100.0f;

        float margin = radius;

        for (int i = 0; i < numParticles; i++)
        {
            if (position[i].y - margin < worldBottom)
            {
                position[i].y = worldBottom + margin;
                velocite[i].y *= -0.3f;
            }
            if (position[i].y + margin > worldTop)
            {
                position[i].y = worldTop - margin;
                velocite[i].y *= -0.3f;
            }
            if (position[i].x - margin < worldLeft)
            {
                position[i].x = worldLeft + margin;
                velocite[i].x *= -0.3f;
            }
            if (position[i].x + margin > worldRight)
            {
                position[i].x = worldRight - margin;
                velocite[i].x *= -0.3f;
            }
        }
    }
}

void Particle::updateDensities(std::vector<glm::vec2> predictedPosition)
{
    densities.resize(numParticles);
    for (int i = 0; i < numParticles; i++)
    {
        densities[i] = calculateDensity(predictedPosition[i]);
    }
}

void Particle::OnEvent(SDL_Event &e)
{
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    float worldX = (mx - WINDOW_W / 2.0f) / 100.0f;
    float worldY = (WINDOW_H / 2.0f - my) / 100.0f;
    glm::vec2 mouseWorldPos = {worldX, -worldY};

    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (e.button.button == SDL_BUTTON_LEFT)
        {
            leftMouseDown = true;
            applyMousePressure(mouseWorldPos, 10.0f, 1.0f);
        }
        else if (e.button.button == SDL_BUTTON_RIGHT)
        {
            rightMouseDown = true;
            applyMousePressure(mouseWorldPos, -10.0f, 1.0f);
        }
        else if (e.button.button == SDL_BUTTON_MIDDLE)
        {
            float z = calculateDensity(mouseWorldPos);
            std::cout << "Density: " << z << " | Pressure: " << convertDensityToPressure(z) << std::endl;
        }
    }
    else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        if (e.button.button == SDL_BUTTON_LEFT)
        {
            leftMouseDown = false;
        }
        else if (e.button.button == SDL_BUTTON_RIGHT)
        {
            rightMouseDown = false;
        }
    }
    else if (e.type == SDL_EVENT_MOUSE_MOTION)
    {
        if (e.motion.state & SDL_BUTTON_LMASK)
        {
            applyMousePressure(mouseWorldPos, 1.0f, 0.8f);
        }
        else if (e.motion.state & SDL_BUTTON_RMASK)
        {
            applyMousePressure(mouseWorldPos, -1.0f, 0.8f);
        }
    }
}

void Particle::applyContinuousMousePressure()
{
    if (!leftMouseDown && !rightMouseDown)
        return;

    float mx, my;
    SDL_GetMouseState(&mx, &my);

    float worldX = (mx - WINDOW_W / 2.0f) / 100.0f;
    float worldY = (WINDOW_H / 2.0f - my) / 100.0f;
    glm::vec2 mouseWorldPos = {worldX, -worldY};

    if (leftMouseDown)
    {
        applyMousePressure(mouseWorldPos, 1.0f, 0.8f);
    }
    else if (rightMouseDown)
    {
        applyMousePressure(mouseWorldPos, -1.0f, 0.8f);
    }
}

void Particle::MakeGrid()
{
    position.clear();
    velocite.clear();
    properties.clear();
    predictedPosition.clear();

    int ppr = (int)std::sqrt(numParticles);
    int ppc = (numParticles - 1) / ppr + 1;
    float spacing = 2.0f * radius + particleSpacing;

    for (int i = 0; i < numParticles; i++)
    {
        float x = (i % ppr - ppr / 2.0f + 0.5f) * spacing;
        float y = (i / ppr - ppc / 2.0f + 0.5f) * spacing;

        position.push_back({x, y});
        predictedPosition.push_back({x, y});
        velocite.push_back({0.0f, 0.0f});
        properties.push_back(1.0f);
    }

    updateDensities(position);
    speed.resize(numParticles, 0.0f);
}

// float Particle::smoothingKernel(float sr, float dst)
// {
//     if (dst >= sr)
//         return 0.0f;

//     float volume = M_PI * pow(sr, 4) / 4.0f;
//     return ((sr - dst) * (sr - dst)) / volume;
// }

// glm::vec2 Particle::smoothingKernelGradient(float sr, glm::vec2 vec)
// {
//     float r = glm::length(vec);
//     if (r <= 0.0f || r >= sr)
//         return glm::vec2(0.0f);

//     float coeff = -12.0f / (M_PI * pow(sr, 4));
//     return coeff * (sr - r) * (vec / r);
// }

// Poly6 kernel for 2D
    float Particle::smoothingKernel(float sr, float r)
    {
        if (r >= sr)
            return 0.0f;
        float coef = 4.0f / (M_PI * pow(sr, 8));
        float diff = (sr * sr - r * r);
        return coef * diff * diff * diff; 
    }

    glm::vec2 Particle::smoothingKernelGradient(float sr, glm::vec2 vec)
    {
        float r = glm::length(vec);
        if (r <= 0.0f || r >= sr)
            return glm::vec2(0.0f);
        float coef = -30.0f / (M_PI * pow(sr, 5));
        float factor = (sr - r) * (sr - r);
        return coef * factor * (vec / r);
    }

float Particle::calculateDensity(glm::vec2 samplePoint)
{
    float mass = 1.0f;
    float density = 0.0f;
    float sr = std::max(0.1f, smoothingRadius);

    std::vector<int> neighbors = getNeighbors(samplePoint);

    for (int particleIndex : neighbors)
    {
        glm::vec2 vec = predictedPosition[particleIndex] - samplePoint;
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
    glm::vec2 samplePoint = predictedPosition[particleIndex];

    std::vector<int> neighbors = getNeighbors(samplePoint);

    for (int i : neighbors)
    {
        if (i == particleIndex)
            continue;

        glm::vec2 vec = samplePoint - predictedPosition[i];
        float dst = glm::length(vec);

        if (dst > 0.0f && dst < smoothingRadius)
        {
            glm::vec2 gradW = smoothingKernelGradient(smoothingRadius, vec);

            float pressure_i = convertDensityToPressure(densities[particleIndex]);
            float pressure_j = convertDensityToPressure(densities[i]);

            float sharedPressure = (pressure_i + pressure_j) / 2.0f;
            pressureForce += -mass * mass * sharedPressure * (1.0f / densities[i] + 1.0f / densities[particleIndex]) * gradW;
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

int Particle::getCellHash(glm::vec2 position)
{
    int cellX = static_cast<int>(position.x / cellSize);
    int cellY = static_cast<int>(position.y / cellSize);
    return getCellHash(cellX, cellY);
}

int Particle::getCellHash(int x, int y)
{
    const int prime1 = 73856093;
    const int prime2 = 19349663;
    return (x * prime1) ^ (y * prime2);
}

void Particle::buildSpatialGrid(std::vector<glm::vec2> predictedPos)
{
    spatialGrid.clear();
    cellSize = smoothingRadius;
    for (int i = 0; i < numParticles; i++)
    {
        int hash = getCellHash(predictedPos[i]);
        spatialGrid[hash].push_back(i);
    }
}

std::vector<int> Particle::getNeighbors(glm::vec2 samplePoint)
{
    std::vector<int> neighbors;

    int centerCellX = static_cast<int>(samplePoint.x / cellSize);
    int centerCellY = static_cast<int>(samplePoint.y / cellSize);

    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            int cellX = centerCellX + dx;
            int cellY = centerCellY + dy;
            int hash = getCellHash(cellX, cellY);

            if (spatialGrid.find(hash) != spatialGrid.end())
            {
                for (int particleIndex : spatialGrid[hash])
                {
                    if (particleIndex >= 0 && particleIndex < predictedPosition.size())
                    {
                        float distance = glm::length(predictedPosition[particleIndex] - samplePoint);
                        if (distance < smoothingRadius)
                        {
                            neighbors.push_back(particleIndex);
                        }
                    }
                }
            }
        }
    }

    return neighbors;
}

void Particle::applyMousePressure(glm::vec2 mousePos, float pressureStrength, float radius)
{
    for (int i = 0; i < numParticles; i++)
    {
        glm::vec2 toParticle = position[i] - mousePos;
        float distance = glm::length(toParticle);

        if (distance < radius && distance > 0.0f)
        {
            glm::vec2 direction = toParticle / distance;

            float forceFactor = 1.0f - (distance / radius);

            if (pressureStrength > 0)
            {

                velocite[i] += direction * pressureStrength * forceFactor;
            }
            else
            {

                velocite[i] -= direction * abs(pressureStrength) * forceFactor;
            }
        }
    }
}