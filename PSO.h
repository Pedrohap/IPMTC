#ifndef PSO_H
#define PSO_H

#include "Particle.h"
#include "LocalSearch.h"
#include "CrossOver.h"
#include <vector>
#include <cmath>
#include <omp.h>

//Cap de quantidade de particulas
extern const int CAP_PARTICULAS;

extern const bool USING_LS;
extern const string LS_MODE;

int pso_int_bg_final;
int pso_qtd_bg;
int pso_qtd_int;

extern int w;

extern vector <double> pso_all_init_fitness;

extern vector <double> pso_all_final_fitness;

extern double media_melhora_twoapt;
extern double media_melhora_twoswap;
extern double media_melhora_insertion;
extern double media_melhora_exchange;
extern int ls_qtd_melhora_global;
extern int val_sol_inicial;

using namespace std;

class PSO{
public:
    vector <Particle> particles;
    vector <double> global_best_position;

    //Vector de pair onde tem o fitness de todas as paticulas, one first é a posição da particula e o second o fitness
    //vector <pair <int,float>> all_particles_fitness;

    double global_best_fitness = numeric_limits<double>::infinity();
    const int qtd_interacos = 1000;
    int qtd_particulas = 15*w;
    
    // PSO iterações
    Particle startPSO(){
        //Cap de qtd de particulas
        if (qtd_particulas > CAP_PARTICULAS){
            qtd_particulas = CAP_PARTICULAS;
        }
        
        vector <int> posicoes_particulas (qtd_particulas);

        for (int i = 0; i < qtd_particulas ; i++){
            if (DECODER_VER == 1){
                particles.push_back(Particle(w));
            } else if (DECODER_VER == 2){
                particles.push_back(Particle(w+m));
            }
            
            posicoes_particulas[i] = i;
        }

        for (int i = 0; i < qtd_particulas ; i++){
            if (!particles[i].isInAllMachines()) {
                cout << "PARTICULA ESCAPOU!" << endl;
                for (int j = 0 ; j < particles[i].best_position.size(); j++){
                    cout << particles[i].best_position[j] << " " << endl;
                }
                cout << endl;
            }
        }
        Particle bestParcticle(w);

        if (DECODER_VER == 1){
            bestParcticle = Particle(w);
        } else if (DECODER_VER == 2){
            bestParcticle = Particle(w+m);
        }
        pso_int_bg_final = 0;
        pso_qtd_bg = 0;
        pso_qtd_int = 0;
        pso_all_init_fitness.clear();
        pso_all_final_fitness.clear();
        bool pso_have_melhora = false;
        int ls_counter = 0;

        media_melhora_twoapt = 0;
        media_melhora_twoswap = 0;
        ls_qtd_melhora_global = 0;
        
        auto start_tempo_pso = chrono::high_resolution_clock::now();
        
        for (int iter = 0; iter < qtd_interacos; iter++) {
            // Update global best
            for (int i = 0; i < qtd_particulas; i++) {
                if (iter == 0){
                    pso_all_init_fitness.push_back(particles[i].initial_fitness);

                    if (particles[i].best_fitness < global_best_fitness) {
                        global_best_position = particles[i].best_position;
                        global_best_fitness = particles[i].best_fitness;
                        //Salva a particula como a melhor global
                        bestParcticle = particles[i];
                        pso_have_melhora = true;
                        pso_int_bg_final = iter;

                        val_sol_inicial = global_best_fitness;
                    }
                } else {
                    break;
                }
            }

            if (pso_have_melhora){
                pso_qtd_bg++;
                pso_have_melhora = false;
            }

            //Movimenta a particula
            #pragma omp parallel for
            for (int i = 0; i < qtd_particulas; i++)
            {
                particles[i].atualizarVelocidade(global_best_position);
                particles[i].atualizarPosicao();
            }

            if(USING_LS){
                //Elite mode
                if(LS_MODE == "ELITE"){
                    //Vetor de pair que contem a posição no first e o fitness no second
                    vector <pair <int, double>> particulas_e_fintess;
                    for (int i = 0 ; i < qtd_particulas ; i++){
                        particulas_e_fintess.push_back(pair <int,double> (i,particles[i].fitness));
                    }

                    //Ordena o vetor
                    sort(particulas_e_fintess.begin(), particulas_e_fintess.end(),sortBySecondDouble);
                    int qtd_particulas_ls = qtd_particulas * 0.05;

                    #pragma omp parallel for
                    for (int i = 0; i < qtd_particulas_ls; i++){
                        insertion(particles[particulas_e_fintess[i].first]);
                        twoSwap(particles[particulas_e_fintess[i].first]);
                    }
                    ls_counter += qtd_particulas_ls;
                } else if(LS_MODE == "NORMAL"){
                    //Selciona 5% das particulas
                    int qtd_particulas_ls = qtd_particulas * 0.05;
                    shuffleVec(posicoes_particulas);

                    #pragma omp parallel for
                    for (int i = 0; i < qtd_particulas_ls; i++){
                        insertion(particles[i]);
                        twoSwap(particles[i]);
                    }
                    ls_counter += qtd_particulas_ls;
                } else{
                    cout << "Falha de codigo, busca local está ligada mas não equivale a nenhum dos modos" << endl;
                }
            }
            
            //crossOver(particles);

            //Atualiza o Melhor Global
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
                #pragma omp parallel for
                for (int i = 0; i < qtd_particulas; i++) {
                    #pragma omp critical
                    {
                        pso_all_final_fitness.push_back(particles[i].best_fitness);
                    }
                }
                cout << "Tempo Limite estourado" << endl;
                break;
            }
        }

        /*if(USING_LS){
            media_melhora_insertion = 0;
            media_melhora_exchange = 0;
            ++ls_counter;
            insertion(bestParcticle);
            exchange(bestParcticle);
        }*/

        //media_melhora_twoapt = media_melhora_twoapt/((qtd_particulas*0.1)*pso_qtd_int);
        media_melhora_twoswap = media_melhora_twoswap/((qtd_particulas*0.1)*pso_qtd_int);
        media_melhora_insertion = media_melhora_insertion/ls_counter;
        //media_melhora_exchange = media_melhora_exchange/ls_counter;

        return bestParcticle;
    }

};

#endif