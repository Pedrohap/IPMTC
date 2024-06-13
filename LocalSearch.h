#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include <iostream>
#include <vector>
#include <utility>
#include "Utilities.h"
#include "Particle.h"

//Houve melhore na busca local do 2APT
extern bool melhora_twoapt;

//Houve melhore na busca local do 2Swap
extern bool melhora_twoswap;

//Media de melhora 2APT
extern float media_melhora_twoapt;

//Media de melhora 2Swap
extern float media_melhora_twoswap;

using namespace std;

//Fazer o 2 APT, que pega uma particula (vetor de float), seleciona um 
//intervalo de 2 pontos e inverte a ordem nesses 2 pontos

void twoAPT (Particle &particula){
    //Pegar o tamanho da particula
    int tamanho_particula = particula.position.size();
    
    //Cria um vetor com todos os intervalos
    vector <pair <int,int>> posicoes;
    for (int i = 0; i < tamanho_particula ; i++){
        for (int j = (i + 1) ; j < tamanho_particula; j++){
            posicoes.push_back(pair <int,int> (i,j));
        }
    }

    //Mistura esse vetor e seleciona 10% deles
    shuffleVec(posicoes);
    int porcentagem = posicoes.size() * 0.1;
    
    //Realiza as interações, se alguma houver melhora substituia a original e vida que segue
    Particle new_particula = particula;
    for (int i = 0; i < porcentagem; i++){
        inverterIntervalo(new_particula.position , posicoes[i].first, posicoes[i].second);
        inverterIntervalo(new_particula.velocity , posicoes[i].first, posicoes[i].second);
        new_particula.recalcularParticle();
        if (new_particula.fitness < particula.fitness){
            if (particula.best_fitness > new_particula.fitness){
                media_melhora_twoapt += (particula.best_fitness - new_particula.fitness);
                melhora_twoapt = true;
            }
            particula = new_particula;
        } else{
            new_particula = particula;
        }
    }
}

void twoSwap (Particle &particula){
    //Pegar o tamanho da particula
    int tamanho_particula = particula.position.size();
    
    //Cria um vetor com todos os pontos selecionaveis
    vector <pair <int,int>> posicoes;
    for (int i = 0; i < tamanho_particula ; i++){
        for (int j = (i + 1) ; j < tamanho_particula; j++){
            posicoes.push_back(pair <int,int> (i,j));
        }
    }

    //Mistura esse vetor e seleciona 10% deles
    shuffleVec(posicoes);
    int porcentagem = posicoes.size() * 0.1;
    
    //Realiza as interações, se alguma houver melhora substituia a original e vida que segue
    Particle new_particula = particula;
    for (int i = 0; i < porcentagem; i++){
        swap(new_particula.position[posicoes[i].first], new_particula.position[posicoes[i].second]);
        swap(new_particula.velocity[posicoes[i].first], new_particula.velocity[posicoes[i].second]);
        new_particula.recalcularParticle();
        if (new_particula.fitness < particula.fitness){
            if (particula.best_fitness > new_particula.fitness){
                media_melhora_twoswap += (particula.best_fitness - new_particula.fitness);
                melhora_twoswap = true;
            }
            particula = new_particula;
        } else{
            new_particula = particula;
        }
    }
}
#endif