#ifndef IPMTC_H
#define IPMTC_H

#include <iostream>
#include <vector>

using namespace std;

extern int w;
extern int m;

class IPMTC{

private:
public:
    IPMTC(/* args */){};
    ~IPMTC(){};

    vector < vector <int> > gerarSolucao(){
        //Implementar uma heuristica aqui

        //Solução mokada para para o KTNS baseada no easyInput
        vector <vector <int>> solucao(m,vector <int>());

        solucao[0] = {0, 4, 6, 7};
        solucao[1] = {1, 2, 3, 5};

        return solucao;
    }
};

#endif