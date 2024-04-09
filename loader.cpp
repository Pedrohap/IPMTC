#include <iostream>
#include <vector>
#include "Debug.h"
#include "IPMTC.h"
#include "KTNS.h"
#include <locale.h>
#include <chrono>

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

//Quantas trocas teve a solução inicial
extern int qtd_trocas_ini;

//Quantas trocas teve a solução refinada
extern int qtd_trocas_pos_ref;

//Tempo de execução da Heuristica
extern chrono::duration<double> duration_HC;

//Tempo de execução do refinamento
extern chrono::duration<double> duration_REF;

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
    setlocale(LC_ALL, "pt_BR.UTF-8");
    readFile();
    
    //Debug debug;
    IPMTC ipmtc;

    //debug.printEntrada();

    auto start_tempo_total = chrono::high_resolution_clock::now();

    vector <vector <int>> solucao = ipmtc.gerarSolucao(true);

    double makespan = ipmtc.funcaoAvaliativa(solucao);
    //debug.printSolucao(solucao);

    auto end_tempo_total = chrono::high_resolution_clock::now();

    auto duration_tempo_total = chrono::duration_cast<chrono::duration<double>>(end_tempo_total - start_tempo_total);

    //cout << "O makespan é de " << makespan << endl;

    cout << val_sol_inicial << "|" << qtd_trocas_ini << "|" << val_sol_pos_ref << "|" << qtd_trocas_pos_ref  << "|" << qtd_ite_ref << "|" << duration_HC.count() << "|" << duration_REF.count() <<"|" <<duration_tempo_total.count()<< endl;


    //A Saida tem que conter as Seguintes informaçoes:
    //* Valor da solução antes do refinamento
    //* Valor da solução após o refinamento
    //* Quantas iterações o refinamento executou
    //* Tempo de execução da HC, tempo de execução do refinamento, e tempo total.
    //* TODO Quantidade de trocas total de todas as maquinas

    return 0;
}