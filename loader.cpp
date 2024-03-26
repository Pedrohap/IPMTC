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
    
    Debug debug;
    IPMTC ipmtc;

    debug.printEntrada();
    vector <vector <int>> solucao = ipmtc.gerarSolucao();

    debug.printSolucao(solucao);

    double makespan = ipmtc.funcaoAvaliativa(solucao);

    cout << "O makespan é de " << makespan << endl;


    return 0;
}