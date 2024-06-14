#ifndef PSO_H
#define PSO_H

#include "Particle.h"
#include "LocalSearch.h"
#include <vector>
#include <cmath>

//Cap de quantidade de particulas
const int CAP_PARTICULAS = 1000;

int pso_int_bg_final;
int pso_qtd_bg;
int pso_qtd_int;

extern int w;

extern vector <float> pso_all_init_fitness;

extern vector <float> pso_all_final_fitness;

extern float media_melhora_twoapt;
extern float media_melhora_twoswap;
extern int ls_qtd_melhora_global;

using namespace std;

class PSO{
public:
    vector <Particle> particles;
    vector <float> global_best_position;

    //Vector de pair onde tem o fitness de todas as paticulas, one first é a posição da particula e o second o fitness
    vector <pair <int,float>> all_particles_fitness;

    double global_best_fitness = numeric_limits<double>::infinity();
    const int qtd_interacos = 1000;
    int qtd_particulas = 10*w;
    
    // PSO iterações
    Particle startPSO(){
        //Cap de qtd de particulas
        if (qtd_particulas > CAP_PARTICULAS){
            qtd_particulas = CAP_PARTICULAS;
        }

        for (int i = 0; i < qtd_particulas ; i++){
            particles.push_back(Particle(w));
        }
        Particle bestParcticle(w);
        pso_int_bg_final = 0;
        pso_qtd_bg = 0;
        pso_qtd_int = 0;
        pso_all_init_fitness.clear();
        pso_all_final_fitness.clear();
        bool pso_have_melhora = false;

        media_melhora_twoapt = 0;
        media_melhora_twoswap = 0;
        ls_qtd_melhora_global = 0;
        
        auto start_tempo_pso = chrono::high_resolution_clock::now();
        
        for (int iter = 0; iter < qtd_interacos; iter++) {
            // Update global best
            for (int i = 0; i < qtd_particulas; i++) {
                if (iter == 0){
                    pso_all_init_fitness.push_back(particles[i].initial_fitness);
                    //Rodar busca local para 50% das particulas
                }

                if (particles[i].best_fitness < global_best_fitness) {
                    global_best_position = particles[i].best_position;
                    global_best_fitness = particles[i].best_fitness;

                    //Salva a particula como a melhor global
                    bestParcticle = particles[i];
                    pso_have_melhora = true;
                    pso_int_bg_final = iter;
                }

                if(iter == qtd_interacos-1){
                    pso_all_final_fitness.push_back(particles[i].best_fitness);
                }
            }

            if (pso_have_melhora){
                pso_qtd_bg++;
                pso_have_melhora = false;
            }
            // Update each particle
            for (int i = 0; i < qtd_particulas; i++) {
                particles[i].atualizarVelocidade(global_best_position);
                particles[i].atualizarPosicao();
            }
            
            /* Pra liberar a busca local só remover esse intervalo de comentario
            //Limpa, Preenche e ordena pelo maior para selecionar 10% para a melhora
            //Inicio da Busca local
            all_particles_fitness.clear();
            int porcentagem = qtd_particulas * 0.1;
            for (int i = 0; i < qtd_particulas; i++) {
                all_particles_fitness.push_back(pair <int,float> (i , particles[i].fitness));
            }
            sort(all_particles_fitness.begin(), all_particles_fitness.end(),sortBySecondFloatDecrecente);
            
            for (int i = 0 ; i < porcentagem ; i++){
                twoOPT(particles[all_particles_fitness[i].first]);
                twoSwap(particles[all_particles_fitness[i].first]);
            }

            //Reatualiza o Melhor Global
            for (int i = 0; i < qtd_particulas; i++) {
                //Adicionar a quantidade de melhoras na melhor global baseado na busca local
                if (particles[i].best_fitness < global_best_fitness) {
                    ls_qtd_melhora_global++;
                    global_best_position = particles[i].best_position;
                    global_best_fitness = particles[i].best_fitness;

                    //Salva a particula como a melhor global
                    bestParcticle = particles[i];
                    pso_have_melhora = true;
                    pso_int_bg_final = iter;
                }

                if(iter == qtd_interacos-1){
                    pso_all_final_fitness.push_back(particles[i].best_fitness);
                }
            }*/

            auto end_tempo_pso = chrono::high_resolution_clock::now();
            auto duration_tempo_pso = chrono::duration_cast<chrono::duration<double>>(end_tempo_pso - start_tempo_pso);

            pso_qtd_int++;
            if(duration_tempo_pso.count() > 7200){
                for (int i = 0; i < qtd_particulas; i++) {
                    pso_all_final_fitness.push_back(particles[i].best_fitness);
                }
                cout << "Tempo Limite estourado" << endl;
                break;
            }
        }

        media_melhora_twoapt = media_melhora_twoapt/((qtd_particulas*0.1)*pso_qtd_int);
        media_melhora_twoswap = media_melhora_twoswap/((qtd_particulas*0.1)*pso_qtd_int);

        return bestParcticle;
    }

};

#endif