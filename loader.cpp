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
#include <string>

using namespace std;
namespace fs = filesystem;

//Metodo utilizado para resolução do problema
    //RH = Heurisitca Randomica ou tradicional
    //PSO = Particle Swarm Optimization
const string METODO = "PSO";

//Quantidade de Execuções do PSO
const int EXECUCOES = 10;

//Nome da pasta principal que carregara as intacias
const string PASTA_PRINCIPAL = "5% Test SmallJobs";

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

//Houve melhore na busca local do 2APT
bool melhora_twoapt;

//Houve melhore na busca local do 2Swap
bool melhora_twoswap;

//Media de melhora 2APT
float media_melhora_twoapt;

//Media de melhora 2Swap
float media_melhora_twoswap;

//Quantidade de melhoras causada pela busca local no MELHOR Global
int ls_qtd_melhora_global;

//Vetor de todas as intancias que já foram executadas;
vector <string> runned_instances;

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
            //Criação da pasta com os dados da particula
            string caminho_do_arquivo = filePath.generic_string();
            vector<string> fullFilePath = separarString(caminho_do_arquivo);
            cout << "Executando agora: ";
            for (int i = 0; i < fullFilePath.size()-1; i++){
                cout << fullFilePath[i] << " ";
            }
            cout << exec;
            cout << endl;
            if (!fs::exists("Particles")){
                fs::create_directory("Particles");
            }
            if (!fs::exists("Particles/"+fullFilePath[1])){
                fs::create_directory("Particles/"+fullFilePath[1]);
            }

            ofstream particleData("Particles/"+fullFilePath[1]+"/"+fullFilePath[2]+"_" + to_string(exec));

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

            particleData << "Iteração que foi encontrado o Melhor Global: " << pso_int_bg_final << endl;
            particleData << "Quantas vezes o Melhor Global foi alterado (Lembrando que a verificação na inicialização acumula nesse contador): " << pso_qtd_bg << endl;
            particleData << "Particula Inicial do melhor global:\n" << bestSolution.toStringCsv(bestSolution.initial_position);
            particleData << "Particula Final do melhor global:\n" << bestSolution.toStringCsv(bestSolution.best_position);

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
            resultData << fullFilePath[2] << "_" << exec << "|" << bestSolution.fitness << "|" << pso_qtd_int << "|" << pso_int_bg_final << "|" << pso_qtd_bg <<"|" << media_melhora_twoapt <<"|" << media_melhora_twoswap <<"|" << ls_qtd_melhora_global << "|" << duration_tempo_total.count() << endl;

            resultData.close();

        }
    
    }

    // Fecha o arquivo
    file.close();
}

void traverseDirectory(const fs::path& directoryPath, const function<void(const fs::path&)>& processFunc) {
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        runned_instances.clear();
        ifstream runing_intances("runing.txt");
        string temp_linha;
        if (runing_intances.is_open()){
            while (getline(runing_intances, temp_linha)){
                runned_instances.push_back(temp_linha);
            }
        }
        runing_intances.close();
        const auto& path = entry.path();
        if (fs::is_regular_file(path)) {
            string caminho_do_arquivo = path.generic_string();
            vector<string> fullFilePath = separarString(caminho_do_arquivo);
            auto iterator = find(runned_instances.begin(),runned_instances.end(),fullFilePath[2]);
            if (!(iterator != runned_instances.end())) {
                ofstream runing_intances("runing.txt", ios::app);
                runing_intances << fullFilePath[2] << "\n";
                runing_intances.close();
                processFunc(path);
            }
        } else if (fs::is_directory(path)) {
            traverseDirectory(path, processFunc); // recursivamente navega em subdiretórios
        }
    }
}

void readFiles(){
    fs::path mainDirectory = PASTA_PRINCIPAL; // Substitua pelo caminho da sua pasta principal
    
    if (!fs::exists(mainDirectory) || !fs::is_directory(mainDirectory)) {
        cerr << "Pasta principal não encontrada!" << endl;
        exit(EXIT_FAILURE);
    }

    traverseDirectory(mainDirectory, processFile);
}

int main (){
    setlocale(LC_ALL, "pt_BR.UTF-8");

    if(METODO == "PSO"){
        omega = 0.0;
        c1 = 2.0;
        c2 = 2.0;
        ofstream resultData("solucoes/resultPSO.csv");

        melhora_twoapt = false;
        melhora_twoswap = false;

        resultData << "NOME_INSTANCIA|MAKESPAN|QTD_ITERACOES|INTER_MELHOR_GLOBAL|QTD_ALTER_MELHOR_GLOBAL|MEDIA_MELHORA_2APT|MEDIA_MELHORA_2SWAP|QTD_MELHORAS_LS_MELHOR_GLOBAL|TEMPO_EXEC\n";

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