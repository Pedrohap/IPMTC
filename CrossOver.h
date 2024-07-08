#include <vector>
#include "Particle.h"
#ifndef CROSSOVER_H
#define CROSSOVER_H

using namespace std;

//Selecionar 15% das piores particulas
//Deletelas
//Substituir por 10% realizando crossover
//Selecionando 2 particulas aleatorias, a cada posição jogar uma moeda, epega um pedaço do pai ou da mãe e gera uma nova
//O resto gerar mutante;
void crossOver(vector <Particle>& particles){
    //Vector de pair onde tem o fitness de todas as paticulas, onde first é a posição da particula e o second o fitness
    vector <pair <int,float>> all_particles_fitness;
    int qtd_particulas = particles.size();

    //Limpa, Preenche e ordena pelo maior para selecionar 15% para a remoção
    all_particles_fitness.clear();
    int porcentagem15 = qtd_particulas * 0.15;
    int porcentagem10 = qtd_particulas * 0.1;
    int porcentagem5 = qtd_particulas * 0.05;
    
    #pragma omp parallel for
    for (int i = 0; i < qtd_particulas; i++) {
        all_particles_fitness.push_back(pair <int,float> (i , particles[i].fitness));
    }

    sort(all_particles_fitness.begin(), all_particles_fitness.end(),sortBySecondFloatCrecente);
    
    //Selecionar 15% das piores particulas
    //For baseado na quantidade da porcentagem de 10%
    #pragma omp parallel for
    for (int i = 0 ; i < porcentagem15 ; i++){
        if (i < porcentagem10){
            int paiPos = randomInt(porcentagem15,qtd_particulas);
            int maePos = randomInt(porcentagem15,qtd_particulas);

            while (paiPos == maePos){
                int maePos = randomInt(porcentagem15,qtd_particulas);
            }

            Particle pai = particles[all_particles_fitness[paiPos].first];
            Particle mae = particles[all_particles_fitness[maePos].first];

            vector<float> filhoPos;
            filhoPos.clear();

            for (int j = 0; j < particles[all_particles_fitness[i].first].best_position.size(); j++){
                if(randomInt(0,2) == 1){
                    filhoPos.push_back(mae.best_position[j]);
                } else {
                    filhoPos.push_back(pai.best_position[j]);
                }
            }

            Particle filho = Particle(filhoPos);
            #pragma omp critical
            {
                particles[all_particles_fitness[i].first] = filho;
            }
        } else {
        //Os outros 5% se tornaram mutantes aleatorios
        #pragma omp critical
        {
            particles[all_particles_fitness[i].first]=Particle(w);
        }
        }
    }
}

#endif