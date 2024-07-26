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
    //máquinas é feita de acordo com o PRIMEIRO algarismo de cada gene, para isso, divide-se 
    //este algarismo pela quantidade de máquinas disponíveis na instância, O
    //resto desta divisão indicará a qual máquina esta tarefa será alocada.

vector <vector <int> > decode (vector<double> solucao_particula,bool& debug){
    //Inicia a matriz de solução
    vector <vector <int> > solucao(m,vector <int>());

    //Gera um vetor de pair onde first é sua posição original e second o seu valor de indice da particula
    vector< pair <int,double> > solucao_particula_pair;

    for (int i = 0; i < solucao_particula.size(); i++){
        solucao_particula_pair.push_back(pair<int,double>(i,solucao_particula[i]));
    }

    //Ordena a solução da particula
    sort(solucao_particula_pair.begin(), solucao_particula_pair.end(),sortBySecondDouble);

    if(debug){
        cout << "Os dados da particula após sua ordenação são:" << endl;
        for (int i = 0; i < solucao_particula_pair.size(); i++){
            cout << "Pos Orig: " << solucao_particula_pair[i].first << " | " << "Valor: " << solucao_particula_pair[i].second << " | Vai pra máquina: " <<getFirstDigit(solucao_particula_pair[i].second) <<endl;
        }
    }

    //Como o tamanho da particula é do tamanho da quantidade de tarefas, realiza o calculo da maquina
    for (int i = 0; i < solucao_particula_pair.size(); i++){
        int temp_maquina = getFirstDigit(solucao_particula_pair[i].second);
        solucao[temp_maquina].push_back(solucao_particula_pair[i].first);
    }

    return solucao;
}


double evaluate(vector<double> solucao_particula,bool debug){
    vector <vector <int> > solucao = decode(solucao_particula, debug);

    IPMTC ipmtc;
    
    return ipmtc.funcaoAvaliativa(solucao);
}

vector <double> recode ( vector < vector <int> > solucao,bool& debug){
    double maxFloat = 1 - 0.000001;

    vector <double> particula(w,0);

    for (int i = 0 ; i < w ; i++){
        double randomTemp = randomDouble(0,maxFloat);
        while(isInVectorDouble(particula,randomTemp)){
            randomTemp = randomDouble(0,maxFloat);
        }
        particula[i] = randomTemp;
    }

    sort(particula.begin(), particula.end());

    for (int i = 0 ; i < w ; i++){
        for (int j = i+1 ; j < w ; j++){
            if (particula[i] == particula[j]){
                cout << "Particula Duplicada 1: " << particula[i] << endl;
                for(int jo = 0 ; jo < particula.size();jo++){
                    cout << particula[jo] << " ";
                }
                cout << endl;
            }
        }
    }
    int lineSizeCount = 0;
    for(int i = 0 ; i < solucao.size();i++){
        for (int j = 0 ; j < solucao[i].size(); j++){
            if(i > 0){
                particula[lineSizeCount+j]+= (i + 0.0);
            }
        }
        lineSizeCount += solucao[i].size();
    }


    for (int i = 0 ; i < w ; i++){
        for (int j = i+1 ; j < w ; j++){
            if (particula[i] == particula[j]){
                cout << "Particula Duplicada 2: " << particula[i] << endl;
                particula[j]+= 0.00001;
                sort(particula.begin(), particula.end());
                for(int jo = 0 ; jo < particula.size();jo++){
                    cout << particula[jo] << " ";
                }
                cout << endl;
            }
        }
    }

    int tempCounter = 0;
    vector <double> particulaDone(w,0);

    for(int i = 0 ; i < solucao.size();i++){
        for (int j = 0 ; j < solucao[i].size(); j++){
            particulaDone[solucao[i][j]] = particula[tempCounter];
            tempCounter++;
        }
    }

    
    for (int i = 0 ; i < w ; i++){
        for (int j = i+1 ; j < w ; j++){
            if (particulaDone[i] == particulaDone[j]){
                cout << "Particula Duplicada 3 DOne: " << particulaDone[i] << endl;
                for(int jo = 0 ; jo < particulaDone.size();jo++){
                    cout << particulaDone[jo] << " ";
                }
                cout << endl;
            }
        }
    }

    if (debug){
        cout << "PARTICULA: ";
        for(int i = 0 ; i < particula.size();i++){
            cout << particula[i] << " ";
        }
        cout << endl;
    }
    if (debug){
        cout << "PARTICULA DONE: ";
        for(int i = 0 ; i < particulaDone.size();i++){
            cout << particulaDone[i] << " ";
        }
        cout << endl;
    }

    return particulaDone;
}

#endif