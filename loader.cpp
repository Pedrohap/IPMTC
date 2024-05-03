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

    auto start_tempo_total = chrono::high_resolution_clock::now();

    if (METODO == "RH"){
        vector <vector <int>> solucao = ipmtc.gerarSolucao(false);
        //vector <vector <int>> solucao = ipmtc.gerarSolucaoAleatoria(true);
        double makespan = ipmtc.funcaoAvaliativa(solucao);

        auto end_tempo_total = chrono::high_resolution_clock::now();
        auto duration_tempo_total = chrono::duration_cast<chrono::duration<double>>(end_tempo_total - start_tempo_total);
        
        cout << val_sol_inicial << "|" << qtd_trocas_ini << "|" << val_sol_pos_ref << "|" << qtd_trocas_pos_ref  << "|" << qtd_ite_ref << "|" << boolToString(houve_melhora) << "|" << duration_HC.count() << "|" << duration_REF.count() <<"|" <<duration_tempo_total.count()<< endl;
    } else if (METODO == "PSO"){
        //Criação da pasta com os dados da particula
        string caminho_do_arquivo = filePath.generic_string();
        vector<string> fullFilePath = separarString(caminho_do_arquivo);

        for (int i = 0; i < fullFilePath.size(); i++){
            cout << fullFilePath[i] << " ";
        }
        cout << endl;
        if (!fs::exists("Particles")){
            fs::create_directory("Particles");
        }
        if (!fs::exists("Particles/"+fullFilePath[1])){
            fs::create_directory("Particles/"+fullFilePath[1]);
        }

        ofstream particleData("Particles/"+fullFilePath[1]+"/"+fullFilePath[2]);

        PSO pso;
        Particle bestSolution = pso.startPSO();

        for (int i = 0; i < pso_all_init_fitness.size() ; i++){
            particleData << pso_all_init_fitness[i];
            if (i < pso_all_init_fitness.size() - 1){
                particleData << "|";
            }
        }

        particleData << endl;

        for (int i = 0; i < pso_all_final_fitness.size() ; i++){
            particleData << pso_all_final_fitness[i];
            if (i < pso_all_final_fitness.size() - 1){
                particleData << "|";
            }
        }
        particleData << endl;

        vector <float> temp_postion_particle = bestSolution.best_position;
        vector <vector <int>> temp_solution = bestSolution.getSolution(temp_postion_particle);

        particleData << debugStringMatriz("A melhor solução desse PSO é:",temp_solution);
        particleData << ipmtc.detalheSolucao(temp_solution);

        particleData.close();

        auto end_tempo_total = chrono::high_resolution_clock::now();
        auto duration_tempo_total = chrono::duration_cast<chrono::duration<double>>(end_tempo_total - start_tempo_total);

        if (!fs::exists("solucoes")){
            fs::create_directory("solucoes");
        }

        ofstream resultData("solucoes/resultPSO.csv", ios::app);
        resultData << fullFilePath[2] << "|" << bestSolution.fitness << "|" << pso_int_bg_final << "|" << pso_qtd_bg <<"|" << duration_tempo_total.count()<< endl;

        resultData.close();

    }
    
    // Fecha o arquivo
    file.close();
}

void traverseDirectory(const fs::path& directoryPath, const function<void(const fs::path&)>& processFunc) {
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        const auto& path = entry.path();
        if (fs::is_regular_file(path)) {
            processFunc(path);
        } else if (fs::is_directory(path)) {
            traverseDirectory(path, processFunc); // recursivamente navega em subdiretórios
        }
    }
}

void readFiles(){
    fs::path mainDirectory = "testGroup"; // Substitua pelo caminho da sua pasta principal
    
    if (!fs::exists(mainDirectory) || !fs::is_directory(mainDirectory)) {
        cerr << "Pasta principal não encontrada!" << endl;
        exit(EXIT_FAILURE);
    }

    traverseDirectory(mainDirectory, processFile);
}

int main (){
    setlocale(LC_ALL, "pt_BR.UTF-8");

    if(METODO == "PSO"){
        ofstream resultData("solucoes/resultPSO.csv");

        resultData << "NOME_INSTANCIA|MAKESPAN|INTER_MELHOR_GLOBAL|QTD_ALTER_MELHOR_GLOBAL|TEMPO_EXEC\n";

        resultData.close();
    }

    readFiles();
    
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