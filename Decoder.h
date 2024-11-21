#ifndef DECODER_H
#define DECODER_H

#include <iostream>
#include <algorithm>
#include <utility>
#include "IPMTC.h"
#include <stdexcept>

extern int m;

using namespace std;

//A decodificação :
    //O ordenamento dos genes indica o escalonamento das tarefas nas máquinas. A alocação das
    //máquinas é feita de acordo com o PRIMEIRO algarismo de cada gene, para isso, divide-se 
    //este algarismo pela quantidade de máquinas disponíveis na instância, O
    //resto desta divisão indicará a qual máquina esta tarefa será alocada.

vector <vector <int> > decode (vector<double> solucao_particula,bool& debug){
    try{
        if (solucao_particula.size() != w){
            cout << "ERRO FATAL, PARTICULA NÃO É DO TAMANHO DA QUANTIDADE DE TAREFAS" << endl;
            debugPrintVector("Particula:",solucao_particula);
        }
        //debugPrintVector("Particula:",solucao_particula);

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
    } catch (const runtime_error& e) {
        // Captura a exceção e trata
        cout << "Exceção capturada: " << e.what() << endl;
        debugPrintVector("Particula:",solucao_particula);
        exit(EXIT_FAILURE);
    }
}


double evaluate(vector<double> solucao_particula,bool debug){
    vector <vector <int> > solucao = decode(solucao_particula, debug);

    IPMTC ipmtc;
    
    return ipmtc.funcaoAvaliativa(solucao);
}

vector <double> recode ( vector < vector <int> > solucao,bool debug=false){
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

vector <vector <int> > decode2 (vector<double> solucao_particula , bool debug=false){
    try{
        if (solucao_particula.size() != (w + m)){
            cout << "ERRO FATAL, PARTICULA NÃO É DO TAMANHO DA QUANTIDADE DE TAREFAS + QUANTIDADE DE MAQUINAS" << endl;
            debugPrintVector("Particula:",solucao_particula);
        }
        if(debug){
            debugPrintVector("Particula antes da ordenação:",solucao_particula);
        }

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
                cout << "Pos Orig: " << solucao_particula_pair[i].first << " | " << "Valor: " << solucao_particula_pair[i].second << endl;
            }
        }

        //Ache a maquina e monte a solução
        for (int i = 0; i < solucao_particula.size(); i++){
            //Uma maquina, não tem porque fazer isso
            if(m == 1){
                solucao[i].push_back(solucao_particula_pair[i].first);
            } else

            //Achou uma maquina
            if(solucao_particula_pair[i].first >= w){
                if(debug){
                    cout << "Achei uma maquina sendo ela: " << solucao_particula_pair[i].first << endl;
                }
                int machine_temp = i-1;
                
                if (debug){
                    cout << "Valor anterior: " << solucao_particula_pair[i-1].first << endl;
                }
                //Percorre todos anteriores até acha uma outra maquina
                while(solucao_particula_pair[machine_temp].first < w){
                    if(debug){
                        cout << "Estou tentando acessar o elemento de numero: " << machine_temp << endl;
                    }
                    if(debug){
                        cout << "Maquina " << solucao_particula_pair[i].first - w << " recebera tarefa" << endl;
                    }
                    solucao[solucao_particula_pair[i].first - w].push_back(solucao_particula_pair[machine_temp].first);
                    machine_temp--;
                    //Ciclo pro fim
                    if(machine_temp < 0){
                        machine_temp = solucao_particula.size() - 1;
                        if (debug){
                            cout << "Cliclei, valor atual " << machine_temp << endl;
                        }
                    }
                    //Achei outra maquina, parar imediatamente
                    if(solucao_particula_pair[machine_temp].first>= w){
                        break;
                    }
                }
            }
        }

        return solucao;
    } catch (const runtime_error& e) {
        // Captura a exceção e trata
        cout << "Exceção capturada: " << e.what() << endl;
        debugPrintVector("Particula:",solucao_particula);
        exit(EXIT_FAILURE);
    }
}


double evaluate2(vector<double> solucao_particula,bool debug=false){
    vector <vector <int> > solucao = decode2(solucao_particula, debug);

    IPMTC ipmtc;
    
    return ipmtc.funcaoAvaliativa(solucao);
}

#endif
