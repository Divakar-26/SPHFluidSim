#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <SDL3/SDL.h>
#include <unordered_map>
#include <functional>

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
    glm::vec2 calculatePressureForce(int particleIndex);
    float convertDensityToPressure(float density);

    void updateDensities(std::vector<glm::vec2> predictedPosition);
    void recalculateSRConstant();


    std::vector<float> pressures;

    void updatePressures();

    std::vector<float> properties;
    std::vector<float> densities;

    bool running = false;
    float radius = 1.0f;
    float alpha = 1.0f;
    float blurry = 0.0f;
    int numParticles = 500;
    float particleSpacing = 0.0f;
    float smoothingRadius = 0.17f;
    static float targetDensity;
    static float pressureMultiplier;
    float mass = 1.0f;

    float GRAVITY = 7.23f;

    std::vector<float> speed;
    std::vector<glm::vec2> predictedPosition;

    void buildSpatialGrid(std::vector<glm::vec2> predictedPos);
    std::vector<int> getNeighbors(glm::vec2 position);

    void applyMousePressure(glm::vec2 mousePos, float pressureStrength, float radius);
    void applyContinuousMousePressure();


private:
    std::vector<glm::vec2> position;
    std::vector<glm::vec2> velocite;
    int WINDOW_W, WINDOW_H;

    float cellSize;
    std::unordered_map<int, std::vector<int>> spatialGrid;

    int getCellHash(glm::vec2 position);
    int getCellHash(int x, int y);

    float coefKernel;
    float coefGradient;
};