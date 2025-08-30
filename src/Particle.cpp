#include "Particle.h"

const float GRAVITY = 9.8f; // units per second^2

Particle::Particle(){
    position.push_back({0.0f, 0.0f});
    position.push_back({1.0f, 0.0f});
    radius = 1.0f;

    velocite.push_back({0.0f, 0.0f});
    velocite.push_back({0.0f, 0.0f});
}

Particle::~Particle(){

}

void Particle::update(float dt){
    if(running){
        for(int i = 0; i < position.size(); i++){
            // 1. Apply gravity to velocity
            velocite[i].y += GRAVITY * dt;
    
            // 2. Update position using velocity
            position[i].y += velocite[i].y * dt;
        }    
    }
}
