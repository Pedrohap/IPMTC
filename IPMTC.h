#ifndef IPMTC_H
#define IPMTC_H

#include <iostream>
#include <vector>
#include "KTNS.h"

#include "Utilities.h"

using namespace std;

extern int w;
extern int m;
extern int p;

extern vector <int> tempo_tarefa;

class IPMTC{

private:
public:
    IPMTC(/* args */){};
    ~IPMTC(){};

    vector < vector <int> > gerarSolucao(){
        //Implementar uma heuristica aqui
        //Famosa Heuristica TDC;
        vector <vector <int>> solucao(m,vector <int>());

        for(int i = 0; i < w; i++){
            solucao[randomInt(0,m)].push_back(i);
        }

        return solucao;
    }

    double funcaoAvaliativa(vector <vector <int>> solucao){
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = KTNS(solucao[i]);

            //DEBUG
            cout << "Quantidade de trocas na máquina: " << i << " é " << trocaMaquinas[i] << endl;
            
            for (int j = 0; j < solucao[i].size() ; j++){
                tempoMaquinas[i] += tempo_tarefa[solucao[i][j]];
            }
            tempoMaquinas[i] += (trocaMaquinas[i] * p);

            cout << "O tempo de processamento da maquina " << i << " é: " << tempoMaquinas[i] << endl;
        }

        double makespan =  tempoMaquinas[0];
        for(int i = 1; i < m ; i++){
            if (makespan < tempoMaquinas[i]){
                makespan = tempoMaquinas[i];
            }
        }

        return makespan;
    }
};

#endif