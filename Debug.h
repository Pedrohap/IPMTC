#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <vector>

using namespace std;

//Numero de Tarefas
extern int w;

//Numero de Maquinas
extern int m;

//Numero de Ferramentas
extern int t;

//Tempo de Troca de Moldes;
extern int p;

//Capacidade do Magazine das máquinas;
extern int c;

//Matriz de tarefas ferramentas;
extern vector < vector <int> > matriz_ferramentas;

//Vetor de tempo de conclusão
extern vector <int> tempo_tarefa;

class Debug
{
private:
    /* data */
public:
    Debug(/* args */){};
    ~Debug(){};

    void printEntrada(){
        cout << "Quantidade de máquinas: " << m << endl;
        cout << "Quantidade de tarefas: " << w << endl;
        cout << "Quantidade de ferramentas: " << t << endl;
        cout << "Capacidade do Magazine: " << c << endl;
        cout << "Tempo de troca de ferramentas:" << p << endl;

        cout << endl;
        cout << "Vetor de tempo de conclusão: " << endl;
        for (int i = 0; i < w; i++){
            cout << tempo_tarefa[i] << ", ";
        }

        cout << endl;

        cout << endl << "Matriz de ferramentas e tarefas:" << endl;

        for (int i = 0; i < t; i++){
            for (int j = 0; j < w; j++){
                cout << matriz_ferramentas[i][j] << ' ';
            }
            cout << endl;
        }
    }

    void printSolucao(vector <vector <int>> solucao){
        cout << "Solução Gerada:" << endl;

        for(int i = 0; i < m; i++){
            for (int j = 0; j < solucao[i].size(); j++){
                cout << solucao[i][j] << ' ';
            }
            cout << endl;
        }
    }
};

#endif