#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <vector>
#include <cstdio>

#include "Utilities.h"
#include "Decoder.h"


const double xmin = 0.0;
double xmax;
//const double c1 = 2.0;
//const double c2 = 2.0;

//Inercia ou o "w estranho"
//const double omega = 1.0;

//iRace Mode
extern double c1;
extern double c2;
extern double omega;

extern int m;
extern int w;
using namespace std;

class Particle{
public:
    double initial_fitness;
    vector<float> initial_position;
    vector<float> position;
    vector<float> velocity;
    vector<float> best_position;
    double fitness;
    double best_fitness;
    
    Particle(int tamanho_particula) {
        xmax = m - 0.000001;
        position.resize(tamanho_particula);
        velocity.resize(tamanho_particula);

        for (int i = 0; i < tamanho_particula; i++) {
            position[i] = randomFloat(xmin,xmax);
            velocity[i] = 0.0;
        }
        initial_position = position;
        fitness = evaluate(position,false);
        initial_fitness = fitness;
        best_position = position;
        best_fitness = fitness;
    }

    void atualizarVelocidade(vector<float>& global_best_position) {
        for (int i = 0; i < w; i++) {
            float r1 = randomFloat(0,1);
            float r2 = randomFloat(0,1);
            velocity[i] = (omega * velocity[i]) + (c1 * r1) * (best_position[i] - position[i]) + (c2 * r2) * (global_best_position[i] - position[i]);
            if (false){
                cout << "Velocidade [" << i << "] = (" << omega << " * " << velocity[i] << ") + (" << c1 << " * " << r1 << ") * (" << best_position[i] << " - " << position[i] << ") + (" << c2 << " * " << r2 << ") * (" << global_best_position[i] << " - " << position[i] << ") \n";
            }
        }

        
    }

    void atualizarPosicao() {
        for (int i = 0; i < w; i++) {
            position[i] += velocity[i];
            while (position[i] < xmin){
                position[i] += xmax;
            }
            while (position[i] > xmax){
                position[i] -= xmax;
            }

            if (position[i] > xmax || position[i] < xmin){
                cout << "ERRO FATAL O LOOP DE ARREDONDAMENTO FALHOU" << endl;
                exit(EXIT_FAILURE);
            }
        }
        fitness = evaluate(position,false);
        if (fitness < best_fitness) {
            best_position = position;
            best_fitness = fitness;
        }
    }

    string toStringCsv(vector<float> particle_position){
        string saida = "";
        for (int i = 0 ; i < particle_position.size(); i++){
            saida += to_string(particle_position[i])  + "|";
        }
        saida += "\n";

        return saida;
    }

    vector < vector <int> > getSolution(vector<float> particle_position){
        bool debugs = false;
        return decode(particle_position, debugs);
    }
};

#endif