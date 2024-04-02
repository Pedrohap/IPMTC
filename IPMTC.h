#ifndef IPMTC_H
#define IPMTC_H

#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

#include "KTNS.h"

#include "Utilities.h"

using namespace std;

extern int w;
extern int m;
extern int p;
extern int t;

extern vector <int> tempo_tarefa;
extern vector < vector <int> > matriz_ferramentas;

//<0>: representa a tarefa,<1>: o tempo e <2>: quantidade de ferramentas
vector <tuple <int,int,int>> tempo_quantidade_tarefas;


class IPMTC{

private:
public:
    IPMTC(/* args */){};
    ~IPMTC(){};

    //Função EXCLSUIVA de comparação para o sort dos tuple
    static bool comparacaoNoSegundoTuple(const tuple<int, int, int>& a, const tuple<int, int, int>& b) {
        // Comparando o segundo elemento
        if (get<2>(a) == get<2>(b)) {
            // Se os segundos elementos são iguais, use o primeiro elemento para desempatar
            return get<1>(a) < get<1>(b);
        }
        // Se os segundos elementos não são iguais, ordene normalmente por eles
        return get<2>(a) < get<2>(b);
    }

    void debugPrintaVetorTuple(){
        for (int i = 0 ; i < tempo_quantidade_tarefas.size(); i++){
            cout << "Tarefa: " << get<0>(tempo_quantidade_tarefas[i]) << endl;
            cout << "Tempo: " << get<1>(tempo_quantidade_tarefas[i]) << endl;
            cout << "Ferramenta: " << get<2>(tempo_quantidade_tarefas[i]) << endl;
        }
    }

    //Calcula a quantidade de ferramentas por tarefa e preenche o vetor de tempo_quantidade_tarefas
    void preencheEOrdenaVetorDeTempoEQuantidade(){
        tempo_quantidade_tarefas.assign(w, tuple <int,int,int>(0,0,0));
        for (int i = 0 ; i < t; i++){
            for(int j = 0 ; j < w; j++){
                get<0>(tempo_quantidade_tarefas[j]) = j;
                get<1>(tempo_quantidade_tarefas[j]) = tempo_tarefa[j];
                if(matriz_ferramentas[i][j] == 1){
                    get<2>(tempo_quantidade_tarefas[j])++;
                }
            }
        }
        sort(tempo_quantidade_tarefas.begin(), tempo_quantidade_tarefas.end(), comparacaoNoSegundoTuple);
    }

    //Função para remover um elemento recenbendo a sua posição e vector de tuple como paramentro;
    void removePosVectorTuple(vector <tuple <int,int,int>>& vetorTuple, int pos){
        if (!vetorTuple.empty() && pos >= 0 && pos < vetorTuple.size()) {
            vetorTuple.erase(vetorTuple.begin() + pos);
        }
    }

    //Retorna o tempo da solução de uma unica maquina
    int getTempoUnico(vector <int> solucaoDaMaquina){
        int qtdTrocas =  KTNS(solucaoDaMaquina);
        int tempoMaquina;
        for (int i = 0; i < solucaoDaMaquina.size() ; i++){
                tempoMaquina += tempo_tarefa[solucaoDaMaquina[i]];
        }
        tempoMaquina += (qtdTrocas * p);

        return tempoMaquina;
    }

    //Retorna a maquina com o menor tempo de processamento
    int getMenorMaquina(vector <vector <int>> solucao){
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = KTNS(solucao[i]);

            for (int j = 0; j < solucao[i].size() ; j++){
                tempoMaquinas[i] += tempo_tarefa[solucao[i][j]];
            }
            tempoMaquinas[i] += (trocaMaquinas[i] * p);
        }
        int menorTempoMaquina = tempoMaquinas[0];
        int menorMaquinaTempo = 0;
        for (int i = 1; i < solucao.size(); i++){
            if(menorTempoMaquina > tempoMaquinas[i]){
                menorTempoMaquina = tempoMaquinas[i];
                menorMaquinaTempo = i;
            }
        }

        return menorMaquinaTempo;
    }

    int getTempoSolucaoComAdd(vector <int> solucaoDaMaquina, int novaTarefa){
        vector <int> tempVec = solucaoDaMaquina;
        tempVec.push_back(novaTarefa);
        return getTempoUnico(tempVec);
    }

    void etapaDeRefinamento(vector <vector <int>> solucao){
        //Etapa de refinamento

        //Bloco = conjunto sequencial de tarefas sem trocas de ferramentas no magazine.
        //1,2,3, XX 4,5, X 6, X 7
        //Pegar a pior máquina (critica), pegar o bloco iniciado no maior número e troca de 
        //ferramentas e mover o bloco para a máquina com o menor tempo de processamento.

        //Gerando uma solução marcada
        for (int i = 0; i < m; i++){
            solucao[i] = KTNSMarcandoTrocas(solucao[i]);
        }

        debugPrintMatriz("Matriz de solucao com trocas marcadas", solucao);
    }

    vector < vector <int> > gerarSolucao(){
        //Implementar uma heuristica aqui
        vector <vector <int>> solucao(m,vector <int>());

        //Primeira tarefa de cada máquina: Tarefa com menor tempo de processamento 
        //entre as que requisitarem o maior número de ferramentas.
        preencheEOrdenaVetorDeTempoEQuantidade();

        //Com o vetor ordernado, fazer a alocação inicial
        for (int i = 0 ; i < m; i++){
            solucao[i].push_back(get<0>(tempo_quantidade_tarefas[i]));
        }

        
        //Remova da lista de tarefas as tarefas alocadas;
        for (int i = 0 ; i < m; i++){
            removePosVectorTuple(tempo_quantidade_tarefas,0);
        }


        //Depois que todas as máquinas receberam uma tarefa, 
        //seleciona-se a máquina com o menor tempo de processamento para receber a próxima tarefa.
        //Proxima tarefa é a que resultar no menor completion time da máquina, ou seja, tempo de 
        //processamento das tarefas mais numero de trocas vezes quantidade de trocas

        //Enquanto todas as tarefas não tiverem sido todas alocadas
        while(!tempo_quantidade_tarefas.empty()){
            //Pega a maquina com menor tempo de trabalho;
            int menorMaquina = getMenorMaquina(solucao);
            
            //Verificar quais das tarefas encaixa melhor na maquina
            int melhorCenario = getTempoSolucaoComAdd(solucao[menorMaquina],get<0>(tempo_quantidade_tarefas[0]));
            int cenarioAtual;

            int melhorTarefa = 0;
            for (int i = 0; i < tempo_quantidade_tarefas.size(); i++){

                cenarioAtual = getTempoSolucaoComAdd(solucao[menorMaquina],get<0>(tempo_quantidade_tarefas[i]));
                if (melhorCenario < cenarioAtual){
                    melhorCenario = cenarioAtual;
                    melhorTarefa = i;
                }
            }
            //Inisiro a tarefa no vetor de solução na maquina desejada e removo a mesma do vetor de tarefas
            solucao[menorMaquina].push_back(get<0>(tempo_quantidade_tarefas[melhorTarefa]));
            removePosVectorTuple(tempo_quantidade_tarefas,melhorTarefa);
        }

        etapaDeRefinamento(solucao);

        /*//Famosa Heuristica TDC;

        for(int i = 0; i < w; i++){
            solucao[randomInt(0,m)].push_back(i);
        }*/

        
        /*//Solução Mocada
        vector <vector <int>> solucao(m,vector <int>());

        solucao[0] = {0,2,4,5,6,7};
        solucao[1] = {1,3};*/

        return solucao;
    }

    double funcaoAvaliativa(vector <vector <int>> solucao){
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = KTNS(solucao[i]);

            for (int j = 0; j < solucao[i].size() ; j++){
                tempoMaquinas[i] += tempo_tarefa[solucao[i][j]];
            }
            tempoMaquinas[i] += (trocaMaquinas[i] * p);
        }

        double makespan =  tempoMaquinas[0];
        
        //DEBUG
        cout << "Quantidade de trocas na máquina: " << 0 << " é " << trocaMaquinas[0] << endl;
        cout << "O tempo de processamento da maquina " << 0 << " é: " << tempoMaquinas[0] << endl;

        for(int i = 1; i < m ; i++){
            if (makespan < tempoMaquinas[i]){
                makespan = tempoMaquinas[i];
            }
            
            cout << "Quantidade de trocas na máquina: " << i << " é " << trocaMaquinas[i] << endl;
            cout << "O tempo de processamento da maquina " << i << " é: " << tempoMaquinas[i] << endl;

        }

        return makespan;
    }
};

#endif