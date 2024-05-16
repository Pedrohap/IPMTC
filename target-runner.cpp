#include <iostream>
#include <vector>
#include "Debug.h"
#include "IPMTC.h"
#include "KTNS.h"
#include "PSO.h"
#include <locale.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>

using namespace std;
namespace fs = filesystem;

//Metodo utilizado para resolução do problema
    //RH = Heurisitca Randomica ou tradicional
    //PSO = Particle Swarm Optimization
const string METODO = "PSO";

//Quantidade de Execuções do PSO
const int EXECUCOES = 1;

//Nome da pasta principal que carregara as intacias
const string PASTA_PRINCIPAL = "5% Test";

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

//Se verdaeiro, informa que ocorreu uma melhora durante o refinameno
extern bool houve_melhora;

//Interação que achou o melhor global
extern int pso_int_bg_final;

//Quantas vezes foi encontrado um melhor global
extern int pso_qtd_bg;

//Todos os fitness das particulas Inicias
vector <float> pso_all_init_fitness;

//Todos os fitness das particulas Finais
vector <float> pso_all_final_fitness;

//Quantidade de interações que o PSO executou
extern int pso_qtd_int;

//iRace
double c1;
double c2;
double omega;

void processFile(const fs::path& filePath) {
    // Abre o arquivo para leitura
    ifstream file(filePath);
    
    // Verifica se o arquivo foi aberto corretamente
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filePath << endl;
        exit(EXIT_FAILURE);
    }

    file >> m;
    file >> w;
    file >> t;
    file >> c;
    file >> p;

    tempo_tarefa.assign(w,0);

    matriz_ferramentas.assign(t, vector<int>(w, 0));
    for (int i = 0; i < w; i++){
        file >> tempo_tarefa[i];
    }

    for (int i = 0; i < t; i++){
        for (int j = 0; j < w; j++){
            file >> matriz_ferramentas[i][j];
        }
    }

    IPMTC ipmtc;

    for (int exec = 0 ; exec < EXECUCOES ; exec++){
        auto start_tempo_total = chrono::high_resolution_clock::now();
        if (METODO == "RH"){
            vector <vector <int>> solucao = ipmtc.gerarSolucao(false);
            //vector <vector <int>> solucao = ipmtc.gerarSolucaoAleatoria(true);
            double makespan = ipmtc.funcaoAvaliativa(solucao);

            auto end_tempo_total = chrono::high_resolution_clock::now();
            auto duration_tempo_total = chrono::duration_cast<chrono::duration<double>>(end_tempo_total - start_tempo_total);

            cout << val_sol_inicial << "|" << qtd_trocas_ini << "|" << val_sol_pos_ref << "|" << qtd_trocas_pos_ref  << "|" << qtd_ite_ref << "|" << boolToString(houve_melhora) << "|" << duration_HC.count() << "|" << duration_REF.count() <<"|" <<duration_tempo_total.count()<< endl;
        } else if (METODO == "PSO"){
            PSO pso;
            Particle bestSolution = pso.startPSO();

            cout << bestSolution.fitness << endl;
        }
    
    }

    // Fecha o arquivo
    file.close();
}



void readFile(string filePath){
    fs::path file = filePath; // Substitua pelo caminho da sua pasta principal
    
    if (!fs::exists(filePath)) {
        cerr << "Arquivo não encontrado!" << endl;
        exit(EXIT_FAILURE);
    }

    processFile(file);
}

int main (int argc, char *argv[]){
    setlocale(LC_ALL, "pt_BR.UTF-8");

    string filePath = argv[4];

    double omega = stod(argv[6]);
    double c1 = stod(argv[8]);
    double c2 = stod(argv[10]);

    readFile(filePath);
    
    //A Saida tem que conter as Seguintes informaçoes:
    //* Valor da solução antes do refinamento
    //* Valor da solução após o refinamento
    //* Quantas iterações o refinamento executou
    //* Tempo de execução da HC, tempo de execução do refinamento, e tempo total.
    //* Quantidade de trocas total de todas as maquinas

    //Pegar o vetor de tarefas e mete um shuffle (aleatorizar)
    //Ordernar a tarefa das maquinas mais desocupadas (A tarefa vai pra maquina com menor tempo de excução)

    //TODO
    //Implementar o PSO (Olhar a planilha);
    //Particulas 100;
    //Interações 100; (Atualizar todas as pairticulas = 1 iteração)
    //Saida deve mostrar em qual iteração rolou a melhor global e quantos melhores existiram
    //Salvar todos os fitness de todas as pariculas da primeira itenração e dps da ultima
    //Paralelizar o "for" que calcula a velocidade e a posição, o calcuo do fitness NÃO É PARALELIZADO

    return 0;
}