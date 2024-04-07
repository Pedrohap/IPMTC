#ifndef KTNS_H
#define KTNS_H

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include "Utilities.h"

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
    //cout << "Magaize atual: ";
    for(int i = 0; i < magazine.size(); i++){
        //cout << magazine[i] << " ";
    }
    //cout << endl;
}

void debugPrintFerramentsNecessarias(int ordem_da_tarefa_na_maquina){
    //cout << "NECESSÁRIO PARA A CONCLUSÃO:" << endl;
    for (int i = 0 ; i < matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina].size();i++){
        //cout << matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i] << " ";
    }
    //cout << endl;
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
            return true;
        }
    }
    return false;
}

//Verifica se a ferramenta é necessaria para a conclusão do conjunto de ferramentas necessarias
bool isCadidataNecessaria(int ferramenta_magazine, vector <int> ferramentas_necessaria){
    for (int i = 0 ; i < ferramentas_necessaria.size() ; i++){
        if(ferramentas_necessaria[i] == ferramenta_magazine){
            return true;
        }
    }
    return false;
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
            if(tempPriority >= ferramenta_prioridade[magazine[i]] && !isCadidataNecessaria(magazine[i],ferramentas_necessarias) ){
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
    //debugPrintMagazine(magazine);

    //cout << "FALTA AS FERRAMENTAS: ";

    for (int i = 0 ; i < matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina].size();i++){

        if(!isCarregada(magazine,matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i])){
            ferramenta_faltando.push_back(matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i]);
            //cout << matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i] << " ";
        }
    }
    //cout << endl;
    //colocar quem falta no magazine removendo os que tem menos prioridade

    //Pega os melhores candidatos a serem removidos e realiza a substituição
    for(int i = 0; i < ferramenta_faltando.size() ; i++){
        int candidatos_remocao = getBaixaPrioridade(magazine,matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina]);
        
        //DEBUG
        //cout << "SAIU A FERRAMENTA: " <<  candidatos_remocao << " ENTROU A FERRAMENTA: " << ferramenta_faltando[i] << endl;
        
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
        //cout << "TAREFA " <<  ordem_da_tarefa_na_maquina << " PODE SER EXECUTADA" << endl;
    } else {
        cout << "ERRO FATAL A MÁQUINA NÃO TEM TODAS AS FERRAMENTAS NECESSÁRIAS!" << endl;
        //debugPrintMagazine(magazine);
        debugPrintFerramentsNecessarias(ordem_da_tarefa_na_maquina);
        exit(EXIT_FAILURE);
    }
    //debugPrintMagazine(magazine);
    //cout << "-----------------------------------------------------" << endl;
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
            //cout << "CARGA INICIAL" << endl;

            //Carga inicial de todas as ferramentas necessarias e perda de 1 ponto em prioridade
            for (int j = 0; j < matriz_de_ferramentas_necessarias[i].size() ; j++){
                magazine.push_back(matriz_de_ferramentas_necessarias[i][j]);
                ferramenta_prioridade[matriz_de_ferramentas_necessarias[i][j]]--;
            }
            //Se o magazine não estiver cheio, adicionar ferramentas para completar baseado nas tarefas subsequentes
            if (maquina_carregada.size() > 1){
                int temp = 0;

                while (magazine.size() <c) {
                    temp++;
                    if(temp >= matriz_de_ferramentas_necessarias.size()){
                        break;
                    }
                    vector <int> candidatos;
                    //debugPrintMagazine(magazine);
                    //debugPrintMatriz("A matriz de ferramentas necessárias é:",  matriz_de_ferramentas_necessarias);
                    //cout << "CADIDATOS INICIAS: " << endl;


                    for (int j = 0; j < matriz_de_ferramentas_necessarias[temp].size() ; j++){
                        //Verifica se a ferramenta já esta carregada no magazine e busca candidatos a incerção

                        if(!isCarregada(magazine,matriz_de_ferramentas_necessarias[temp][j])){
                            //ferramenta encontrada q não está no magazine, adicionar em uma lista de candidatos e escolher a que tiver
                            //maior prioridade
                            candidatos.push_back( matriz_de_ferramentas_necessarias[temp][j]);
                            //cout << matriz_de_ferramentas_necessarias[temp][j] << " ";
                        }
                    }
                    //cout << endl;

                    //Se todos os candidatos cabem no magazine, insira todos
                    if ((magazine.size() + candidatos.size()) <= c && (candidatos.size() != 0)){
                        for (int j = 0 ; j < candidatos.size(); j++){
                            magazine.push_back(candidatos[j]);
                        }
                    } 
                    //Tem mais cadidatos que espaço no magazine, insere até lotar
                    else if (!candidatos.empty()){
                        
                        int espaco_disponivel = c - magazine.size();

                        for(int j = 0; j < espaco_disponivel ; j++){
                            int melhor_candidato = getAltaPrioridade(candidatos);

                            //cout << "ADICIONEI " << melhor_candidato << endl;
                            
                            magazine.push_back(melhor_candidato);

                            candidatos.erase(remove(candidatos.begin(), candidatos.end(), melhor_candidato), candidatos.end());

                            ferramenta_prioridade[melhor_candidato]--;
                        }
                    }
                }
            }
            //cout << "MAGAZINE INICIAL" << endl;
            //debugPrintMagazine(magazine);
            //cout << "O TAMANHO DO MAGAZINE FOI: " << magazine.size() << endl;
            //cout << "-----------------------------------------------------" << endl;

        }


        //Verificar se as ferramentas necessaria para a prox tarefa estão carregadas
        //Se a tarefa não é possivel de ser processada, precisa realizar trocas
        if(!isProcessavel(magazine,i)){
            trocaMagazine(magazine,i);
        }   
    }
    return trocas;
}

//Retorna a trocas no magazine Marcando elas na solução com '-1'
void trocaMagazineMarcandoTrocas(vector <int>& magazine, int ordem_da_tarefa_na_maquina,vector <int>& maquina_com_trocas,vector<int> solucao_maquina){
    //Verificar quem está faltando no magazine
    vector <int> ferramenta_faltando;
 
    //DEBUG
    //debugPrintMagazine(magazine);

    //cout << "FALTA AS FERRAMENTAS: ";

    for (int i = 0 ; i < matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina].size();i++){

        if(!isCarregada(magazine,matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i])){
            ferramenta_faltando.push_back(matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i]);
            //cout << matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina][i] << " ";
        }
    }
    //cout << endl;
    //colocar quem falta no magazine removendo os que tem menos prioridade

    //Pega os melhores candidatos a serem removidos e realiza a substituição
    for(int i = 0; i < ferramenta_faltando.size() ; i++){
        int candidatos_remocao = getBaixaPrioridade(magazine,matriz_de_ferramentas_necessarias[ordem_da_tarefa_na_maquina]);
        
        //DEBUG
        //cout << "SAIU A FERRAMENTA: " <<  candidatos_remocao << " ENTROU A FERRAMENTA: " << ferramenta_faltando[i] << endl;
        
        magazine.erase(remove(magazine.begin(), magazine.end(), candidatos_remocao), magazine.end());
        //magazine.push_back(ferramenta_faltando[i]);
        //ferramenta_prioridade[ferramenta_faltando[i]]--;
        //trocas++;
    }
    int cont_trocas = 0 ;
    for(int i = 0; i < ferramenta_faltando.size() ; i++){

        magazine.push_back(ferramenta_faltando[i]);
        ferramenta_prioridade[ferramenta_faltando[i]]--;

        cont_trocas++;
        trocas++;
    }

    if (cont_trocas>0){
        maquina_com_trocas.push_back(-1*cont_trocas);
    }

    //DEBUG
    if (isProcessavel(magazine,ordem_da_tarefa_na_maquina)){
        //cout << "TAREFA " <<  ordem_da_tarefa_na_maquina << " PODE SER EXECUTADA" << endl;
         maquina_com_trocas.push_back(solucao_maquina[ordem_da_tarefa_na_maquina]);
    } else {
        //cout << "ERRO FATAL A MÁQUINA NÃO TEM TODAS AS FERRAMENTAS NECESSÁRIAS!" << endl;
        //debugPrintMagazine(magazine);
        debugPrintFerramentsNecessarias(ordem_da_tarefa_na_maquina);
        exit(EXIT_FAILURE);
    }
    //debugPrintMagazine(magazine);
    //cout << "-----------------------------------------------------" << endl;
}


//Recebe o vetor de uma máquina e retorna as trocas na solução como '-1'
vector <int> KTNSMarcandoTrocas(vector <int> maquina_carregada){
    int quantidade_tarefas = maquina_carregada.size();
    trocas = 0;

    vector <int> maquina_com_trocas;

    vector <vector <int>> matriz_de_ferramentas_necessarias = getMatrizDeFerramentasNecessarias(maquina_carregada);
    vector <int> magazine;

    //Definir a prioridade das ferramentas
    ferramenta_prioridade.assign(t,0);
    for (int i = 0 ; i < ferramentas_utilizadas.size() ; i++){
        ferramenta_prioridade[ferramentas_utilizadas[i]] ++;
    }
    for (int i = 0 ; i < quantidade_tarefas ; i++){
        if (i == 0){
            //cout << "CARGA INICIAL" << endl;

            maquina_com_trocas.push_back(maquina_carregada[i]);

            //Carga inicial de todas as ferramentas necessarias e perda de 1 ponto em prioridade
            for (int j = 0; j < matriz_de_ferramentas_necessarias[i].size() ; j++){
                magazine.push_back(matriz_de_ferramentas_necessarias[i][j]);
                ferramenta_prioridade[matriz_de_ferramentas_necessarias[i][j]]--;
            }
            //Se o magazine não estiver cheio, adicionar ferramentas para completar baseado nas tarefas subsequentes
            if (maquina_carregada.size() > 1){
                int temp = 0;

                while (magazine.size() <c) {
                    temp++;
                    if(temp >= matriz_de_ferramentas_necessarias.size()){
                        break;
                    }
                    vector <int> candidatos;
                    //debugPrintMagazine(magazine);
                    //debugPrintMatriz("A matriz de ferramentas necessárias é:",  matriz_de_ferramentas_necessarias);
                    //cout << "CADIDATOS INICIAS: " << endl;


                    for (int j = 0; j < matriz_de_ferramentas_necessarias[temp].size() ; j++){
                        //Verifica se a ferramenta já esta carregada no magazine e busca candidatos a incerção

                        if(!isCarregada(magazine,matriz_de_ferramentas_necessarias[temp][j])){
                            //ferramenta encontrada q não está no magazine, adicionar em uma lista de candidatos e escolher a que tiver
                            //maior prioridade
                            candidatos.push_back( matriz_de_ferramentas_necessarias[temp][j]);
                            //cout << matriz_de_ferramentas_necessarias[temp][j] << " ";
                        }
                    }
                    //cout << endl;

                    //Se todos os candidatos cabem no magazine, insira todos
                    if ((magazine.size() + candidatos.size()) <= c && (candidatos.size() != 0)){
                        for (int j = 0 ; j < candidatos.size(); j++){
                            magazine.push_back(candidatos[j]);
                        }
                    } 
                    //Tem mais cadidatos que espaço no magazine, insere até lotar
                    else if (!candidatos.empty()){
                        
                        int espaco_disponivel = c - magazine.size();

                        for(int j = 0; j < espaco_disponivel ; j++){
                            int melhor_candidato = getAltaPrioridade(candidatos);

                            //cout << "ADICIONEI " << melhor_candidato << endl;
                            
                            magazine.push_back(melhor_candidato);

                            candidatos.erase(remove(candidatos.begin(), candidatos.end(), melhor_candidato), candidatos.end());

                            ferramenta_prioridade[melhor_candidato]--;
                        }
                    }
                }
            }
            //cout << "MAGAZINE INICIAL" << endl;
            //debugPrintMagazine(magazine);
            //cout << "O TAMANHO DO MAGAZINE FOI: " << magazine.size() << endl;
            //cout << "-----------------------------------------------------" << endl;

        }


        //Verificar se as ferramentas necessaria para a prox tarefa estão carregadas
        //Se a tarefa não é possivel de ser processada, precisa realizar trocas
        if(!isProcessavel(magazine,i)){
            trocaMagazineMarcandoTrocas(magazine,i,maquina_com_trocas,maquina_carregada);
        }   
    }
    return maquina_com_trocas;
}



//Defina um nivel de prioridade para todas as ferramentas baseado em uso futuro, a que tiver menor prioridade sai para entrar a
//ferramenta necessaria para o processamento subsequente
#endif