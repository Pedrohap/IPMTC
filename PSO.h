#ifndef PSO_H
#define PSO_H

#include "Particle.h"
#include <vector>
#include <cmath>

int pso_int_bg_final;
int pso_qtd_bg;
int pso_qtd_int;

extern int w;

extern vector <float> pso_all_init_fitness;

extern vector <float> pso_all_final_fitness;

using namespace std;

class PSO{
public:
    vector <Particle> particles;
    vector <float> global_best_position;
    double global_best_fitness = numeric_limits<double>::infinity();
    const int qtd_interacos = 1000;
    const int qtd_particulas = 10*w;
    
    // PSO iterações
    Particle startPSO(){
        for (int i = 0; i < qtd_particulas ; i++){
            particles.push_back(Particle(w));
        }
        Particle bestParcticle(w);
        pso_int_bg_final = 0;
        pso_qtd_bg = 0;
        pso_qtd_int = 0;
        pso_all_init_fitness.clear();
        pso_all_final_fitness.clear();
        
        auto start_tempo_pso = chrono::high_resolution_clock::now();
        
        for (int iter = 0; iter < qtd_interacos; iter++) {
            // Update global best
            for (int i = 0; i < qtd_particulas; i++) {
                if (iter == 0){
                    pso_all_init_fitness.push_back(particles[i].initial_fitness);
                }

                if (particles[i].best_fitness < global_best_fitness) {
                    global_best_position = particles[i].best_position;
                    global_best_fitness = particles[i].best_fitness;

                    //Salva a particula como a melhor global
                    bestParcticle = particles[i];
                    pso_qtd_bg++;
                    pso_int_bg_final = iter;
                }

                if(iter == qtd_interacos-1){
                    pso_all_final_fitness.push_back(particles[i].best_fitness);
                }
            }
            // Update each particle
            for (int i = 0; i < qtd_particulas; i++) {
                particles[i].atualizarVelocidade(global_best_position);
                particles[i].atualizarPosicao();
            }
            auto end_tempo_pso = chrono::high_resolution_clock::now();
            auto duration_tempo_pso = chrono::duration_cast<chrono::duration<double>>(end_tempo_pso - start_tempo_pso);

            pso_qtd_int++;
            if(duration_tempo_pso.count() > 7200){
                cout << "Tempo Limite estourado" << endl;
                break;
            }
        }

        return bestParcticle;
    }

};

#endif