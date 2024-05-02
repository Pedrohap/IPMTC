#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <vector>

#include "Utilities.h"
#include "Decoder.h"

const double xmin = 0.0;
const double xmax = 1.0;
const double c1 = 1.0;
const double c2 = 1.0;

//Inercia ou o "w estranho"
const double omega = 1.0;

extern int w;
using namespace std;

class Particle{
public:
    vector<float> position;
    vector<float> velocity;
    vector<float> best_position;
    double fitness;
    double best_fitness;
    
    Particle(int tamanho_particula) {
        position.resize(tamanho_particula);
        velocity.resize(tamanho_particula);

        for (int i = 0; i < tamanho_particula; i++) {
            position[i] = randomFloat(xmin,xmax);
            velocity[i] = 0.0;
        }
        fitness = evaluate(position,false);
        best_position = position;
        best_fitness = fitness;
    }

    void atualizarVelocidade(vector<float>& global_best_position) {
        for (int i = 0; i < w; i++) {
            float r1 = randomFloat(xmin,xmax);
            float r2 = randomFloat(xmin,xmax);
            velocity[i] = (omega * velocity[i]) + (c1 * r1) * (best_position[i] - position[i]) + (c2 * r2) * (global_best_position[i] - position[i]);
        }
    }

    void atualizarPosicao() {
        for (int i = 0; i < w; i++) {
            position[i] += velocity[i];
            if (position[i] < xmin)
                position[i] = xmin;
            else if (position[i] > xmax)
                position[i] = xmax;
        }
        fitness = evaluate(position,false);
        if (fitness < best_fitness) {
            best_position = position;
            best_fitness = fitness;
        }
    }
};

#endif