#include "Particle.h"
#include <math.h>
#include <random>

const float GRAVITY = 9.8f; // units per second^2

int randomParticles = 200;

Particle::Particle(int W, int H)
{
    radius = 0.1f;
    particleSpacing = 0.0f;

    float halfW = (W / 2.0f) / 100.0f;
    float halfH = (H / 2.0f) / 100.0f;

    srand(time(NULL));
    for (int i = 0; i < randomParticles; i++)
    {
        float rx = static_cast<float>(rand()) / RAND_MAX; // [0,1]
        float ry = static_cast<float>(rand()) / RAND_MAX; // [0,1]

        // Map [0,1] → [-1,1] and then scale to fit inside window
        float x = (rx * 2.0f - 1.0f) * halfW;
        float y = (ry * 2.0f - 1.0f) * halfH;

        position.push_back({x, y});
        velocite.push_back({0.0f, 0.0f});
    }
}


Particle::~Particle()
{
}

void Particle::update(float dt)
{
    if (running)
    {
        for (int i = 0; i < position.size(); i++)
        {
            // 1. Apply gravity to velocity
            velocite[i].y += GRAVITY * dt;

            // 2. Update position using velocity
            position[i].y += velocite[i].y * dt;
        }
    }
}

void Particle::MakeGrid()
{
    position.clear();
    velocite.clear();

    int ppr = (int)std::sqrt(numParticles);          // particles per row
    int ppc = (numParticles - 1) / ppr + 1;          // particles per column
    float spacing = 2.0f * radius + particleSpacing; // touch when spacing=0

    for (int i = 0; i < numParticles; i++)
    {
        float x = (i % ppr - ppr / 2.0f + 0.5f) * spacing;
        float y = (i / ppr - ppc / 2.0f + 0.5f) * spacing;

        position.push_back({x, y});
        velocite.push_back({0.0f, 0.0f});
        std::cout << x << " " << y << std::endl;
    }
}


float Particle::smoothingKernel()