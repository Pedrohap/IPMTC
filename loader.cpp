#include <iostream>
#include <vector>
#include "Debug.h"
#include "IPMTC.h"
#include "KTNS.h"
#include <locale.h>

using namespace std;

//Numero de Tarefas
int w;

//Numero de Maquinas
int m;

//Numero de Ferramentas
int t;

//Tempo de Troca de Moldes;
int p;

//Capacidade do Magazine das máquinas;
int c;

//Matriz de tarefas ferramentas, onde a coluna representa a tarefa;
vector < vector <int> > matriz_ferramentas;

//Vetor de tempo de conclusão
vector <int> tempo_tarefa;

//Valor da solução antes do refinamento
extern int val_sol_inicial;

//Valor da solução após o refinamento
extern int val_sol_pos_ref;

//Quantas iterações o refinamento executou
extern int qtd_ite_ref;

void readFile(){
    cin >> m;
    cin >> w;
    cin >> t;
    cin >> c;
    cin >> p;

    tempo_tarefa.assign(w,0);

    matriz_ferramentas.assign(t, vector<int>(w, 0));
    for (int i = 0; i < w; i++){
        cin >> tempo_tarefa[i];
    }

    for (int i = 0; i < t; i++){
        for (int j = 0; j < w; j++){
            cin >> matriz_ferramentas[i][j];
        }
    }
}

int main (){
    setlocale(LC_ALL, "Portuguese");
    readFile();
    
    //Debug debug;
    IPMTC ipmtc;

    //debug.printEntrada();


    vector <vector <int>> solucao = ipmtc.gerarSolucao();

    double makespan = ipmtc.funcaoAvaliativa(solucao);
    //debug.printSolucao(solucao);

    //cout << "O makespan é de " << makespan << endl;

    cout << val_sol_inicial << "|" << val_sol_pos_ref << "|" << qtd_ite_ref << "|" << endl;


    //A Saida tem que conter as Seguintes informaçoes:
    //* Valor da solução antes do refinamento
    //* Valor da solução após o refinamento
    //* Quantas iterações o refinamento executou
    //* Tempo de execução da HC, tempo de execução do refinamento, e tempo total.

    return 0;
}