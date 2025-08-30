#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

class Particle
{

public:
    Particle(int W, int H);
    ~Particle();

    void update(float dt);

    float & GetRadius(){return radius;}
    std::vector<glm::vec2> & GetPositions() {return position;}

    void MakeGrid();
    float smoothingKernel(float sR, float dst);
    float calculateDensity(glm::vec2 point);

    bool running = false;
    float radius = 1.0f;
    float alpha = 1.0f;
    float blurry = 0.0f;
    int numParticles = 50;
    float particleSpacing = 0.0f;
    float smoothingRadius = 0.0f;
    
private:
    std::vector<glm::vec2> position;
    std::vector<glm::vec2> velocite;

    int WINDOW_W, WINDOW_H;
};