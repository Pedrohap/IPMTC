#ifndef DECODER_H
#define DECODER_H

#include <iostream>
#include <algorithm>
#include <utility>
#include "IPMTC.h"

extern int m;

using namespace std;

//A decodificação :
    //O ordenamento dos genes indica o escalonamento das tarefas nas máquinas. A alocação das
    //máquinas é feita de acordo com o último algarismo de cada gene, para isso, divide-se 
    //este algarismo pela quantidade de máquinas disponíveis na instância, O
    //resto desta divisão indicará a qual máquina esta tarefa será alocada.

vector <vector <int> > decode (vector<float> solucao_particula,bool& debug){
    //Inicia a matriz de solução
    vector <vector <int> > solucao(m,vector <int>());

    //Gera um vetor de pair onde first é sua posição original e second o seu valor de indice da particula
    vector< pair <int,float> > solucao_particula_pair;

    for (int i = 0; i < solucao_particula.size(); i++){
        solucao_particula_pair.push_back(pair<int,float>(i,solucao_particula[i]));
    }

    //Ordena a solução da particula
    sort(solucao_particula_pair.begin(), solucao_particula_pair.end(),sortBySecondFloat);

    if(debug){
        cout << "Os dados da particula após sua ordenação são:" << endl;
        for (int i = 0; i < solucao_particula_pair.size(); i++){
            cout << "Pos Orig: " << solucao_particula_pair[i].first << " | " << "Valor: " << solucao_particula_pair[i].second << " | Vai pra máquina: " <<getLastDigit(solucao_particula_pair[i].second)%m <<endl;
        }
    }

    //Como o tamanho da particula é do tamanho da quantidade de tarefas, realiza o calculo da maquina
    for (int i = 0; i < solucao_particula_pair.size(); i++){
        int temp_maquina = getLastDigit(solucao_particula_pair[i].second)%m;
        solucao[temp_maquina].push_back(solucao_particula_pair[i].first);
    }

    return solucao;
}


double evaluate(vector<float> solucao_particula,bool debug){
    vector <vector <int> > solucao = decode(solucao_particula, debug);

    IPMTC ipmtc;
    
    return ipmtc.funcaoAvaliativa(solucao);
}

#endif