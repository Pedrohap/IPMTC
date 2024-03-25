#ifndef KTNS_H
#define KTNS_H

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

extern int c;
extern int t;
extern vector < vector <int> > matriz_ferramentas;

vector <int> ferramentas_utilizadas;

//Um vector onde cada ponto do vetor representar uma ferramenta e seu nivel de prioridade
vector <int> ferramenta_prioridade;

//Matriz com as ferramentas necessaria para a conclusão para um conjuento de tarefas especifico
vector <vector <int>> matriz_de_ferramentas_necessarias;

//quantidade de trocas na maquina_carregada
int trocas;

void debugPrintMagazine(vector <int> magazine){
    cout << "Magaize atual: ";
    for(int i = 0; i < magazine.size(); i++){
        cout << magazine[i] << " ";
    }
    cout << endl;
}

//Retorna uma Matriz com as ferramentas necessaria para a conclusão para um conjuento de tarefas especifico
vector <vector <int>> getMatrizDeFerramentasNecessarias(vector <int> maquina_carregada){
    matriz_de_ferramentas_necessarias.assign(maquina_carregada.size(),vector <int>());

    for (int x = 0; x < maquina_carregada.size(); x++){
        for (int i = 0; i < t; i++){
            if(matriz_ferramentas[i][maquina_carregada[x]] == 1){
                matriz_de_ferramentas_necessarias[x].push_back(i);
                ferramentas_utilizadas.push_back(i);
            }
        }
    }

    /*
    //Limpar as ferramentas duplicadas do vector de ferramentas utilizadas
    auto it = unique(ferramentas_utilizadas.begin(), ferramentas_utilizadas.end());
    ferramentas_utilizadas.resize(distance(ferramentas_utilizadas.begin(), it));
    */

    //Retorna a matriz com as ferramentas necessarias
    return matriz_de_ferramentas_necessarias;
}

bool isCarregada(vector <int> magazine,int ferramenta){
    for (int i = 0; i < magazine.size(); i++){
        if (ferramenta == magazine[i]){
            return true;
        }
    } 

    return false;
}

//verifica se as ferramenta necessaria já faz parte do magazine
bool isNecessaria(vector <int> magazine,int ferramenta_necessaria){
    for (int i = 0; i< magazine.size(); i++){
        if(magazine[i] == ferramenta_necessaria){
            return false;
        }
    }
    return true;
}

//Retorna a ferramenta com maior prioridade dentro de um conjunto de candidatos
int getAltaPrioridade(vector <int> candidatos){
    if (candidatos.size() == 1){
        return candidatos[0];
    }

    int tempPriority = ferramenta_prioridade[candidatos[0]];
    int ferramentaComMaiorPrioridade = candidatos[0];
    for (int i = 1; i< candidatos.size(); i++){
        if(tempPriority < ferramenta_prioridade[candidatos[i]]){
            tempPriority = ferramenta_prioridade[candidatos[i]];
            ferramentaComMaiorPrioridade = candidatos[i];
        }
    }

    return ferramentaComMaiorPrioridade;
}

int getBaixaPrioridade (vector <int> magazine,vector <int> ferramentas_necessarias){
    //Inicia com um valor absurdo para garantir a subsituição do valor no laço for
    int tempPriority = 99999;
    int ferramenta= magazine[0];
    for (int i = 0; i< magazine.size(); i++){
        for (int j = 0 ; j < ferramentas_necessarias.size(); j++){
            if(tempPriority >= ferramenta_prioridade[magazine[i]] && isNecessaria(magazine,ferramentas_necessarias[j]) ){
                tempPriority = ferramenta_prioridade[magazine[i]];
                ferramenta = magazine[i];
            }
        }
    }

    return ferramenta;
}


//Verificar se todas as ferramentas necessarias estão carregadas na magazine para o processamento da tarefa
bool isProcessavel(vector <int> magazine, int ordem_da_tarefa_na_maquina){
    int contador = 0;

    for (int i = 0 ; i < matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina].size();i++){
        if(isCarregada(magazine,matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i])){
            contador++;
        }
    }
    if (contador == matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina].size()){
        return true;
    }

    return false;
}

void trocaMagazine(vector <int>& magazine, int ordem_da_tarefa_na_maquina){
    //Verificar quem está faltando no magazine
    vector <int> ferramenta_faltando;
 
    //DEBUG
    debugPrintMagazine(magazine);

    cout << "FAlTA AS FERRAMENTAS: ";

    for (int i = 0 ; i < matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina].size();i++){

        if(!isCarregada(magazine,matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i])){
            ferramenta_faltando.push_back(matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i]);
            cout << matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i] << " ";
        }
    }
    cout << endl;
    //colocar quem falta no magazine removendo os que tem menos prioridade

    //Pega os melhores candidatos a serem removidos e realiza a substituição
    for(int i = 0; i < ferramenta_faltando.size() ; i++){
        int candidatos_remocao = getBaixaPrioridade(magazine,matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina]);
        
        //DEBUG
        cout << "SAIU A FERRAMENTA: " <<  candidatos_remocao << " ENTROU A FERRAMENTA: " << ferramenta_faltando[i] << endl;
        
        magazine.erase(remove(magazine.begin(), magazine.end(), candidatos_remocao), magazine.end());
        //magazine.push_back(ferramenta_faltando[i]);
        //ferramenta_prioridade[ferramenta_faltando[i]]--;
        //trocas++;
    }

    for(int i = 0; i < ferramenta_faltando.size() ; i++){

        magazine.push_back(ferramenta_faltando[i]);
        ferramenta_prioridade[ferramenta_faltando[i]]--;
        trocas++;
    }

    //DEBUG
    if (isProcessavel(magazine,ordem_da_tarefa_na_maquina)){
        cout << "TAREFA " <<  ordem_da_tarefa_na_maquina << " PODE SER EXECUTADA" << endl;
    }
    debugPrintMagazine(magazine);
    cout << "-----------------------------------------------------" << endl;
}

//Recebe o vetor de uma máquina e retorna a quantidade de trocas necessárias
int KTNS(vector <int> maquina_carregada){
    int quantidade_tarefas = maquina_carregada.size();
    trocas = 0;

    vector <vector <int>> matriz_de_ferramentas_necessarias = getMatrizDeFerramentasNecessarias(maquina_carregada);
    vector <int> magazine;

    //Definir a prioridade das ferramentas
    ferramenta_prioridade.assign(t,0);

    for (int i = 0 ; i < ferramentas_utilizadas.size() ; i++){
        ferramenta_prioridade[ferramentas_utilizadas[i]] ++;
    }

    for (int i = 0 ; i < quantidade_tarefas ; i++){
        if (i == 0){
            //Carga inicial de todas as ferramentas necessarias e perda de 1 ponto em prioridade
            for (int j = 0; j < matriz_de_ferramentas_necessarias[i].size() ; j++){
                magazine.push_back(matriz_de_ferramentas_necessarias[i][j]);
                ferramenta_prioridade[matriz_de_ferramentas_necessarias[i][j]]--;
            }
            
            //Se o magazine não estiver cheio, adicionar ferramentas para completar baseado nas tarefas subsequentes
            int temp = 0;
            while (magazine.size() < c){
                temp++;
                vector <int> candidatos;
                for (int j = 0; j < matriz_de_ferramentas_necessarias[i].size() ; j++){
                    //Verifica se a ferramenta já esta carregada no magazine e busca candidatos a incerção
                    if(!isCarregada(magazine,matriz_de_ferramentas_necessarias[temp][j])){
                        //ferramenta encontrada q não está no magazine, adicionar em uma lista de candidatos e escolher a que tiver
                        //maior prioridade
                        candidatos.push_back( matriz_de_ferramentas_necessarias[temp][j]);

                        //magazine.push_back(matriz_de_ferramentas_necessarias[temp][j]);
                    }
                }
                
                if (candidatos.size() == 0){
                    //não houve candidatos
                }
                //Se todos os candidatos cabem no magazine, insira todos
                else if ((magazine.size() + candidatos.size()) <= c){
                    for (int j = 0 ; j < candidatos.size(); j++){
                        magazine.push_back(candidatos[j]);
                    }
                } 
                //Tem mais cadidatos que espaço no magazine, insere até lotar
                else {
                    int espaco_disponivel = c - magazine.size();

                    for(int j = 0; j < espaco_disponivel ; j++){
                        int melhor_candidato = getAltaPrioridade(candidatos);
                        magazine.push_back(melhor_candidato);
                        ferramenta_prioridade[melhor_candidato]--;
                    }
                }
                if ((temp + 1) >=  matriz_de_ferramentas_necessarias[i].size()){
                    //Não tem mais tarefas ou não é mais necessario adicionar ferramentas
                    break;
                } else {
                    temp++;
                }
            }
        }


        //Verificar se as ferramentas necessaria para a prox tarefa estão carregadas
        //Se a tarefa não é possivel de ser processada, precisa realizar trocas
        if(!isProcessavel(magazine,i)){
            trocaMagazine(magazine,i);
        }   
    }

    return trocas;
}


//Defina um nivel de prioridade para todas as ferramentas baseado em uso futuro, a que tiver menor prioridade sai para entrar a
//ferramenta necessaria para o processamento subsequente
#endif