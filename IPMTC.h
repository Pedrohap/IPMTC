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

    //Retorna a maquina com o maior tempo de processamento
    int getMaiorMaquina(vector <vector <int>> solucao){
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = KTNS(solucao[i]);

            for (int j = 0; j < solucao[i].size() ; j++){
                tempoMaquinas[i] += tempo_tarefa[solucao[i][j]];
            }
            tempoMaquinas[i] += (trocaMaquinas[i] * p);
        }
        int maiorTempoMaquina = tempoMaquinas[0];
        int maiorMaquinaTempo = 0;
        for (int i = 1; i < solucao.size(); i++){
            if(maiorTempoMaquina < tempoMaquinas[i]){
                maiorTempoMaquina = tempoMaquinas[i];
                maiorMaquinaTempo = i;
            }
        }

        return maiorMaquinaTempo;
    }

    //Retorna um vetor ordenardo com todos elementos negativos (trocas) de uma solução, onde first é posição
    //second o valor
    vector <pair <int,int>> getElementosNegativos(vector <int>& solucaoDaMaquina){
        vector <pair <int,int>> elementosNegativos;
        for (int i = 0 ; i < solucaoDaMaquina.size(); i++){
            if (solucaoDaMaquina[i] < 0){
                elementosNegativos.push_back(pair <int,int>(i,solucaoDaMaquina[i]));
            }
        }

        //Ordena o vetor de pais em ordem crescente baseado no valor
        sort(elementosNegativos.begin(), elementosNegativos.end(), sortBySecond);

        return elementosNegativos;
    }

    int getTempoSolucaoComAdd(vector <int> solucaoDaMaquina, int novaTarefa){
        vector <int> tempVec = solucaoDaMaquina;
        tempVec.push_back(novaTarefa);
        return getTempoUnico(tempVec);
    }

    int getPosMenorElementoVector(vector <int>& vetor){
        // Encontra o iterador para o menor elemento no vetor
        auto menor_elemento = min_element(vetor.begin(), vetor.end());
        
        // Calcula a posição do menor elemento
        int posicao = distance(vetor.begin(), menor_elemento);

        return posicao;
    }

    vector < vector <int> > etapaDeRefinamento(vector <vector <int>> solucao){
        vector < vector <int> > solucao_marcada(m,vector <int>());

        //Nova solucao que sera alterada durante a etapa de refinamento
        vector < vector <int> > nova_solucao = solucao;

        //Etapa de refinamento

        //Bloco = conjunto sequencial de tarefas sem trocas de ferramentas no magazine.
        //1,2,3, XX 4,5, X 6, X 7
        //Pegar a pior máquina (critica), pegar o bloco iniciado no maior número e troca de 
        //ferramentas e mover o bloco para a máquina com o menor tempo de processamento.

        //Gerando uma solução marcada
        for (int i = 0; i < m; i++){
            solucao_marcada[i] = KTNSMarcandoTrocas(solucao[i]);
        }


        //Agora que temos o vetor marcado, procurar a posição com o menor valor
        int maquina_critica = getMaiorMaquina(solucao);

        //Criar um array para identificar todos os possiveis blocos onde first é a posição e second o valor
        vector <pair <int,int>> momentosDeTroca = getElementosNegativos(solucao[maquina_critica]);

        vector <vector <int>> melhor_soulucao = solucao;
        for (int i = 0 ; i < momentosDeTroca.size(); i++){
            //Pegar o bloco iniciado com maior numero de trocas:
            int ponteiro_marcado = momentosDeTroca[i].first;
            
            //Preencher um vetor com todo o bloco até a troca
            vector <int> bloco;
            

            while (solucao_marcada[maquina_critica][ponteiro_marcado] > 0){
                bloco.push_back(solucao_marcada[maquina_critica][ponteiro_marcado]);
                removeDoVector(nova_solucao[maquina_critica],solucao_marcada[maquina_critica][ponteiro_marcado]);
                ponteiro_marcado++;
            }

            int maquina_folgada = getMenorMaquina(solucao);
            
            for (int j = 0 ; j < bloco.size(); j++){
                nova_solucao[maquina_folgada].push_back(bloco[j]);
            }

            if(funcaoAvaliativa(nova_solucao) < funcaoAvaliativa(melhor_soulucao)){
                melhor_soulucao = nova_solucao;
                break;
            }
        }



        return nova_solucao;
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

        bool melhora;

        do {
            double actual_makespan = funcaoAvaliativa(solucao);
            vector <vector <int> > new_solution = etapaDeRefinamento(solucao);
            double new_makespan = funcaoAvaliativa(new_solution);

            if (new_makespan < actual_makespan){
                melhora = true;
                solucao = new_solution;
            } else {
                melhora = false;
            }


        } while(melhora);

        /*//Famosa Heuristica TDC;

        for(int i = 0; i < w; i++){
            solucao[randomInt(0,m)].push_back(i);
        }*/

        
        //Solução Mocada
        /*vector <vector <int>> solucao(m,vector <int>());

        solucao[0] = {0,1,3,4};
        solucao[1] = {7,2,6,5};*/

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