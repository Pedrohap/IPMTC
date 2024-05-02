#ifndef PSO_H
#define PSO_H

#include "Particle.h"
#include <vector>
#include <cmath>

int pso_int_bg_final;
int pso_qtd_bg;

extern int w;

using namespace std;

class PSO{
public:
    vector <Particle> particles;
    vector <float> global_best_position;
    double global_best_fitness = numeric_limits<double>::infinity();
    const int qtd_interacos = 100;
    const int qtd_particulas = 100;
    
    // PSO iterações
    Particle startPSO(){
        particles.assign(qtd_particulas,Particle(w));
        Particle bestParcticle(w);
        pso_int_bg_final = 0;
        pso_qtd_bg = 0;

        for (int iter = 0; iter < qtd_interacos; iter++) {
            // Update global best
            for (int i = 0; i < qtd_particulas; i++) {
                if (particles[i].best_fitness < global_best_fitness) {
                    global_best_position = particles[i].best_position;
                    global_best_fitness = particles[i].best_fitness;

                    //Salva a particula como a melhor global
                    bestParcticle = particles[i];
                    pso_qtd_bg++;
                    pso_int_bg_final = iter;
                }
            }
            // Update each particle
            for (int i = 0; i < qtd_particulas; i++) {
                particles[i].atualizarVelocidade(global_best_position);
                particles[i].atualizarPosicao();
            }
        }

        return bestParcticle;
    }

};

#endif