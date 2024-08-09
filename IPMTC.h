#ifndef IPMTC_H
#define IPMTC_H

#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <chrono>


#include "KTNS.h"

#include "Utilities.h"

using namespace std;

extern int w;
extern int m;
extern int p;
extern int t;

extern vector <int> tempo_tarefa;
extern vector < vector <int> > matriz_ferramentas;

//Valor da solução antes do refinamento
int val_sol_inicial;

//Valor da solução após o refinamento
int val_sol_pos_ref;

//Quantas iterações o refinamento executou
int qtd_ite_ref;

//Quantas trocas teve a solução inicial
int qtd_trocas_ini;

//Quantas trocas teve a solução refinada
int qtd_trocas_pos_ref;

bool houve_melhora;

chrono::duration<double> duration_HC;

chrono::duration<double> duration_REF;

class IPMTC{

private:
public:
    KTNS ktns;
    //<0>: representa a tarefa,<1>: o tempo e <2>: quantidade de ferramentas
    vector <tuple <int,int,int>> tempo_quantidade_tarefas;
    
    IPMTC(/* args */){};
    ~IPMTC(){};

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
        } else {
            cout << "ERRO FATAL EM removePosVectorTuple" << endl;
            exit(EXIT_FAILURE);
        }
    }

    //Função para remover um elemento do vector de tuple baseado no valor da primeira posição do tuple;
    void removeElementoVectorTuple(vector <tuple <int,int,int>>& vetorTuple, int elemento){
        vetorTuple.erase(remove_if(vetorTuple.begin(), vetorTuple.end(), [&](const tuple<int, int, int>& elem) {
            return get<0>(elem) == elemento;
        }), vetorTuple.end());
    }

    //Retorna o tempo da solução de uma unica maquina
    int getTempoUnico(vector <int> solucaoDaMaquina){
        int qtdTrocas =  ktns.doKTNS(solucaoDaMaquina);
        int tempoMaquina = 0;
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
            trocaMaquinas[i] = ktns.doKTNS(solucao[i]);

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

    //Retorna a tarefa com maior tempo de conclusão
    int getTarefaMaisDemorada(vector <int>& solucao_da_maquina){
        if (solucao_da_maquina.empty()){
            cout << "MAQUINA VAZIA" << endl;
            return -1;
        }
        int temp = solucao_da_maquina[0];
        for (int i = 1; i < solucao_da_maquina.size(); i++){
            if(tempo_tarefa[temp]<tempo_tarefa[solucao_da_maquina[i]]){
                temp = solucao_da_maquina[i];
            }
        }

        return temp;
    }

    //Retorna a tarefa com menor tempo de conclusão
    int getTarefaMaisRapida(vector <int>& solucao_da_maquina){
        if (solucao_da_maquina.empty()){
            cout << "MAQUINA VAZIA" << endl;
            return -1;
        }
        int temp = solucao_da_maquina[0];
        for (int i = 1; i < solucao_da_maquina.size(); i++){
            if(tempo_tarefa[temp]>tempo_tarefa[solucao_da_maquina[i]]){
                temp = solucao_da_maquina[i];
            }
        }

        return temp;
    }

    //Retorna a maquina com o maior tempo de processamento
    int getMaiorMaquina(vector <vector <int>> solucao){
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = ktns.doKTNS(solucao[i]);

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

        //Ordena o vetor de pair em ordem crescente baseado no valor
        sort(elementosNegativos.begin(), elementosNegativos.end(), sortBySecond);

        return elementosNegativos;
    }

    int getTempoSolucaoComAdd(vector <int> solucaoDaMaquina, int novaTarefa){
        vector <int> tempVec = solucaoDaMaquina;
        tempVec.push_back(novaTarefa);
        return getTempoUnico(tempVec);
    }

    int getQuantidadTrocasComAdd(vector <int> solucaoDaMaquina, int novaTarefa){
        vector <int> tempVec = solucaoDaMaquina;
        tempVec.push_back(novaTarefa);
        return ktns.doKTNS(tempVec);
    }

    int getPosMenorElementoVector(vector <int>& vetor){
        // Encontra o iterador para o menor elemento no vetor
        auto menor_elemento = min_element(vetor.begin(), vetor.end());
        
        // Calcula a posição do menor elemento
        int posicao = distance(vetor.begin(), menor_elemento);

        return posicao;
    }

    vector < vector <int> > etapaDeRefinamento(vector <vector <int>> solucao,bool debug){
        if (debug){
            printLinha();
            cout << "Etapa de Refinamento:" << endl;
        }

        vector < vector <int> > solucao_marcada(m,vector <int>());

        //Nova solucao que sera alterada durante a etapa de refinamento
        vector < vector <int> > nova_solucao;

        //Etapa de refinamento

        //Bloco = conjunto sequencial de tarefas sem trocas de ferramentas no magazine.
        //1,2,3, XX 4,5, X 6, X 7
        //Pegar a pior máquina (critica), pegar o bloco iniciado no maior número e troca de 
        //ferramentas e mover o bloco para a máquina com o menor tempo de processamento.

        //Gerando uma solução marcada
        for (int i = 0; i < m; i++){
            solucao_marcada[i] = ktns.KTNSMarcandoTrocas(solucao[i]);
        }

        //Agora que temos o vetor marcado, procurar a posição com o maior valor
        int maquina_critica = getMaiorMaquina(solucao);

        //Criar um array para identificar todos os possiveis blocos onde first é a posição e second o valor
        vector <pair <int,int>> momentosDeTroca = getElementosNegativos(solucao_marcada[maquina_critica]);

        if (debug){
            debugPrintMatriz("Matriz de solução com trocas marcadas:",solucao_marcada);
            cout << "Sendo a máquina " << maquina_critica << " critica" << endl;
            debugPrintVectorPair("Com os seguintes possiveis blocos:",momentosDeTroca);
        }

        //Se não tiver nenhuma troca na maquina critica, joga pra maquina ociosa
        //a tarefa com maior tempo, se piorar, joga a menor
        if (momentosDeTroca.empty()){
            nova_solucao = solucao;
            int maquina_ociosa = getMenorMaquina(solucao);
            int tarefa_mais_demorada = getTarefaMaisDemorada(solucao[maquina_critica]);

            nova_solucao[maquina_ociosa].push_back(tarefa_mais_demorada);
            removeDoVector(nova_solucao[maquina_critica],tarefa_mais_demorada);

            if(debug){
                cout << "Nenhuma troca existe na maquina critica" << endl;
                cout << "Assim, mandamos a maior tarefa, sendo ela " << tarefa_mais_demorada << " da máquina critica " << maquina_critica << " para a maquina ociosa " << maquina_ociosa << endl;
                debugPrintMatriz("Resultando na seguinte solução:",nova_solucao);
                cout << "Com um makespan de: " << funcaoAvaliativa(nova_solucao) << endl;
            }

            if(funcaoAvaliativa(nova_solucao) < funcaoAvaliativa(solucao)){
                if(debug){
                    cout << "Nova solução melhorada encontrada!" << endl;
                    cout << "Fim da etapa de refinamento" << endl;
                    printLinha();
                }
                return nova_solucao;
            } else {
                //Reseta a nova solucao
                nova_solucao = solucao;
                int tarefa_menos_demorada = getTarefaMaisRapida(solucao[maquina_critica]);
                nova_solucao[maquina_ociosa].push_back(tarefa_menos_demorada);
                removeDoVector(nova_solucao[maquina_critica],tarefa_menos_demorada);

                if(debug){
                    cout << "Assim, mandamos a menor tarefa, sendo ela " << tarefa_menos_demorada << " da máquina critica " << maquina_critica << " para a maquina ociosa " << maquina_ociosa << endl;
                    debugPrintMatriz("Resultando na seguinte solução:",nova_solucao);
                    cout << "Com um makespan de: " << funcaoAvaliativa(nova_solucao) << endl;
                }

                if(funcaoAvaliativa(nova_solucao) < funcaoAvaliativa(solucao)){
                    if(debug){
                        cout << "Nova solução melhorada encontrada!" << endl;
                        cout << "Fim da etapa de refinamento" << endl;
                        printLinha();
                    }
                    return nova_solucao;
                }
            }

        }

        for (int i = 0 ; i < momentosDeTroca.size(); i++){
            nova_solucao = solucao;
            //Pegar o bloco iniciado com maior numero de trocas:
            int ponteiro_marcado_guide = momentosDeTroca[i].first;
            int ponteiro_marcado = momentosDeTroca[i].first +1;
            
            //Preencher um vetor com todo o bloco até a troca
            vector <int> bloco;
            

            while (solucao_marcada[maquina_critica][ponteiro_marcado] >= 0 && ponteiro_marcado < solucao_marcada[maquina_critica].size()){
                bloco.push_back(solucao_marcada[maquina_critica][ponteiro_marcado]);
                removeDoVector(nova_solucao[maquina_critica],solucao_marcada[maquina_critica][ponteiro_marcado]);
                ponteiro_marcado++;
            }

            if(debug){
                cout << "O ponteiro de inicio do bloco foi o " << ponteiro_marcado_guide << endl;
                debugPrintVector("Assim o bloco é:",bloco);
            }

            int maquina_folgada = getMenorMaquina(solucao);
            
            for (int j = 0 ; j < bloco.size(); j++){
                nova_solucao[maquina_folgada].push_back(bloco[j]);
            }

            if(debug){
                cout << "Jogando o bloco na maquina mais desocupada que é a máquina: " << maquina_folgada << endl;
                debugPrintMatriz("Gerando a seguinte nova solução:",nova_solucao);
                cout << "Com um Makespan de :" << funcaoAvaliativa(nova_solucao) << endl;

            }
            qtd_ite_ref++;

            if(funcaoAvaliativa(nova_solucao) < funcaoAvaliativa(solucao)){
                if(debug){
                    cout << "Nova solução melhorada encontrada!" << endl;
                    cout << "Fim da etapa de refinamento" << endl;
                    printLinha();
                }
                return nova_solucao;
            } else {
                if(debug){
                    cout << "Não foi encontrada uma solução melhor, reinicia-se o processo montando um novo bloco" << endl;
                }
            }
        }

        if(debug){
            cout << "Todos os possiveis blocos foram testandos e não houve melhora." << endl;
        }

        return nova_solucao;
    }

    //Metodo Original com uma heuristica não aleatoria
    vector < vector <int> > gerarSolucao(bool printSolution){

        auto start_tempo_HC = chrono::high_resolution_clock::now();
        
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

            //Para escolha da próxima tarefa, antes de considerar a tarefa que vai resultar no menor 
            //tempo de processamento da máquina, procure por tarefas que todas as ferramentas 
            //necessárias já estejam no magazine. Se houver mais de uma, selecione a de maior tempo 
            //de processamento. Se não existirem, siga com o processo de escolha anterior.
            
            //Vector de pair de int aonde first representa a tarefa e second o seu tempo
            vector <pair <int,int>> tarefas_sem_troca;
            for (int i = 0; i < tempo_quantidade_tarefas.size(); i++){
                if(getQuantidadTrocasComAdd(solucao[menorMaquina],get<0>(tempo_quantidade_tarefas[i])) == 0){
                    //cout << "A tarefa " << get<0>(tempo_quantidade_tarefas[i]) << " não precisa de troca para a maquina " << menorMaquina << endl;
                    //debugPrintMatriz("A solução atual é:",solucao);
                    tarefas_sem_troca.push_back(pair <int,int> (get<0>(tempo_quantidade_tarefas[i]),get<1>(tempo_quantidade_tarefas[i])));
                }
            }
            
            //Se tiver a possibilidade de executar tarefas sem a necessidade de troca coloco a que tiver maior
            //tempo na menor maquina
            if (!tarefas_sem_troca.empty()){
                sort(tarefas_sem_troca.begin(), tarefas_sem_troca.end(), sortBySecond);
                //cout<< "Foi inserida a tarefa: " << tarefas_sem_troca[0].first << endl;
                
                //Inisiro a tarefa no vetor de solução na maquina desejada e removo a mesma do vetor de tarefas
                solucao[menorMaquina].push_back(tarefas_sem_troca[0].first);

                removeElementoVectorTuple(tempo_quantidade_tarefas,tarefas_sem_troca[0].first);

                tarefas_sem_troca.clear();
            } else {

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
        }

        val_sol_inicial = funcaoAvaliativa(solucao);
        qtd_trocas_ini = getQuantidadeTrocas(solucao);

        if(printSolution){
            debugPrintMatriz("A solução inical é:",solucao);
            cout << "Quantidade de trocas: " << getQuantidadeTrocas(solucao) << endl;
            cout << "Makespan: " << funcaoAvaliativa(solucao) << endl;
        }


        auto end_tempo_HC = chrono::high_resolution_clock::now();

        duration_HC = chrono::duration_cast<chrono::duration<double>>(end_tempo_HC - start_tempo_HC);

        bool melhora;
        qtd_ite_ref = 0;

        auto start_REF = chrono::high_resolution_clock::now();
        houve_melhora = false;
        do {
            double actual_makespan = funcaoAvaliativa(solucao);
            vector <vector <int> > new_solution = etapaDeRefinamento(solucao,printSolution);
            double new_makespan = funcaoAvaliativa(new_solution);
            if (new_makespan < actual_makespan){
                melhora = true;
                solucao = new_solution;
                houve_melhora = true;
            } else {
                melhora = false;
            }
            
        } while(melhora);

        auto end_REF = chrono::high_resolution_clock::now();

        duration_REF = chrono::duration_cast<chrono::duration<double>>(end_REF - start_REF);

        /*//Famosa Heuristica TDC;

        for(int i = 0; i < w; i++){
            solucao[randomInt(0,m)].push_back(i);
        }*/

        
        //Solução Mocada
        /*vector <vector <int>> solucao(m,vector <int>());

        solucao[0] = {0,1,3,4};
        solucao[1] = {7,2,6,5};*/

        val_sol_pos_ref = funcaoAvaliativa(solucao);
        qtd_trocas_pos_ref = getQuantidadeTrocas(solucao);

        if(printSolution){
            debugPrintMatriz("A solução refinada é:",solucao);
            cout << "Quantidade de trocas: " << getQuantidadeTrocas(solucao) << endl;
            cout << "Makespan: " << funcaoAvaliativa(solucao) << endl;
        }

        return solucao;
    }

    vector < vector <int>> gerarSolucaoAleatoria (bool printSolution){
        auto start_tempo_HC = chrono::high_resolution_clock::now();

        //Gerar vetor de tarefas e aleatoriza-lo
        vector <int> vec_tarefas;

        for (int i = 0; i < w ;i++){
            vec_tarefas.push_back(i);
        }

        shuffleVec(vec_tarefas);
        //Ordernar a tarefa das maquinas mais desocupadas (A tarefa vai pra maquina com menor tempo de excução)
        vector <vector <int>> solucao(m,vector <int>());

        for (int i = 0; i < m ; i++){
            if (!vec_tarefas.empty()){
                int temp = vec_tarefas[0];
                solucao[i].push_back(temp);
                removeDoVector(vec_tarefas,temp);
            } else {
                //Não tem mais tarefas dispniveis
                return solucao;
            }
        }

        do{
            int maquina_mais_ociosa = getMenorMaquina(solucao);
            solucao[maquina_mais_ociosa].push_back(vec_tarefas[0]);
            removeDoVector(vec_tarefas,vec_tarefas[0]);

        }while(!vec_tarefas.empty());

        val_sol_inicial = funcaoAvaliativa(solucao);
        qtd_trocas_ini = getQuantidadeTrocas(solucao);

        if(printSolution){
            debugPrintMatriz("A solução inical é:",solucao);
            cout << "Quantidade de trocas: " << getQuantidadeTrocas(solucao) << endl;
            cout << "Makespan: " << funcaoAvaliativa(solucao) << endl;
        }

        auto end_tempo_HC = chrono::high_resolution_clock::now();

        duration_HC = chrono::duration_cast<chrono::duration<double>>(end_tempo_HC - start_tempo_HC);

        //Inicio da função de refinamento
        bool melhora;
        houve_melhora = false;
        qtd_ite_ref = 0;

        auto start_REF = chrono::high_resolution_clock::now();

        do {
            double actual_makespan = funcaoAvaliativa(solucao);
            vector <vector <int> > new_solution = etapaDeRefinamento(solucao,printSolution);
            double new_makespan = funcaoAvaliativa(new_solution);
            if (new_makespan < actual_makespan){
                melhora = true;
                houve_melhora = true;
                solucao = new_solution;
            } else {
                melhora = false;
            }

        } while(melhora);

        auto end_REF = chrono::high_resolution_clock::now();

        duration_REF = chrono::duration_cast<chrono::duration<double>>(end_REF - start_REF);

        val_sol_pos_ref = funcaoAvaliativa(solucao);
        qtd_trocas_pos_ref = getQuantidadeTrocas(solucao);

        if(printSolution){
            debugPrintMatriz("A solução refinada é:",solucao);
            cout << "Quantidade de trocas: " << getQuantidadeTrocas(solucao) << endl;
            cout << "Makespan: " << funcaoAvaliativa(solucao) << endl;
        }

        return solucao;

    }

    double funcaoAvaliativa(vector <vector <int>> solucao){
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = ktns.doKTNS(solucao[i]);
            for (int j = 0; j < solucao[i].size() ; j++){
                tempoMaquinas[i] += tempo_tarefa[solucao[i][j]];
            }
            tempoMaquinas[i] += (trocaMaquinas[i] * p);
        }

        double makespan =  tempoMaquinas[0];
        
        for(int i = 1; i < m ; i++){
            if (makespan < tempoMaquinas[i]){
                makespan = tempoMaquinas[i];
            }    
        }

        //Para garantir que a solução é valida, deve-se verificar se todas as tarefas estão alocadas nas máquinas
        if (!isValida(solucao)){
            cout << "ERRO CRÍTICO, SOLUÇÃO NÃO CONTEM A MESMA QUANTIDADE DE TAREFAS";
            exit(EXIT_FAILURE);
        }

        return makespan;
    }

    //Retorna uma tuple onde:
    //get <0>: Maquina critica
    //get <1>: Maquinas menos critica
    //get <2>: Makespan
    tuple<int,int,double> funcaoAvaliativaDetalhada(vector <vector <int>> solucao){
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        //Pos 0: Maquina critica
        //Pos 1: Maquinas menos critica
        //Pos 2: Makespan
        tuple<int,int,double> retorno;

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = ktns.doKTNS(solucao[i]);
            for (int j = 0; j < solucao[i].size() ; j++){
                tempoMaquinas[i] += tempo_tarefa[solucao[i][j]];
            }
            tempoMaquinas[i] += (trocaMaquinas[i] * p);
        }

        double makespan =  tempoMaquinas[0];
        get<0>(retorno) = 0;
        get<1>(retorno) = 0;
        
        for(int i = 1; i < m ; i++){
            if (makespan < tempoMaquinas[i]){
                makespan = tempoMaquinas[i];
               get<0>(retorno) = i;
            }
            if(tempoMaquinas[i] < tempoMaquinas[get<1>(retorno)]){
                get<1>(retorno) = i;
            }
        }

        get<2>(retorno) = makespan;

        //Para garantir que a solução é valida, deve-se verificar se todas as tarefas estão alocadas nas máquinas
        if (!isValida(solucao)){
            cout << "ERRO CRÍTICO, SOLUÇÃO NÃO CONTEM A MESMA QUANTIDADE DE TAREFAS";
            exit(EXIT_FAILURE);
        }

        return retorno;
    }

    void printSolucaoDetalhada(vector <vector <int>> solucao){
        string retorno;
        vector <double> tempoMaquinas(m,0);
        vector <int> trocaMaquinas(m,0);

        debugPrintMatriz("A solução é:",solucao);

        for (int i = 0; i < solucao.size(); i++){
            trocaMaquinas[i] = ktns.doKTNS(solucao[i]);
            for (int j = 0; j < solucao[i].size() ; j++){
                tempoMaquinas[i] += tempo_tarefa[solucao[i][j]];
            }
            tempoMaquinas[i] += (trocaMaquinas[i] * p);
            cout << "Tempo da maquina " << i << ": " << tempoMaquinas[i] << endl;
            cout << "Trocas da maquina  "<< i << ": " << trocaMaquinas[i] << endl;
        }

        double makespan =  tempoMaquinas[0];
        
        for(int i = 1; i < m ; i++){
            if (makespan < tempoMaquinas[i]){
                makespan = tempoMaquinas[i];
            }    
        }

        //Para garantir que a solução é valida, deve-se verificar se todas as tarefas estão alocadas nas máquinas
        if (!isValida(solucao)){
            cout << "ERRO CRÍTICO, SOLUÇÃO NÃO CONTEM A MESMA QUANTIDADE DE TAREFAS";
            exit(EXIT_FAILURE);
        }

        cout<< "Tendo um makespan de: " << makespan << endl;
    }


    bool isValida(vector <vector <int>>& solucao){
        int cont = 0;
        for (int i = 0; i < solucao.size();i++){
            cont += solucao[i].size();
        }
        if (cont == w){
            return true;
        } else {
            return false;
        }
    }

    //Retorna a quantidade de trocas de uma solução
    double getQuantidadeTrocas(vector <vector <int>> solucao){
        int qtd_trocas = 0;
        for (int i = 0; i < solucao.size(); i++){
            qtd_trocas += ktns.doKTNS(solucao[i]);
        }

        return qtd_trocas;
    }

    //Retorna a quantidade de trocas de uma solução
    double getQuantidadeTrocasMaquina(vector <int>& solucao){
        int qtd_trocas = ktns.doKTNS(solucao);

        return qtd_trocas;
    }

    //Retorna uma string contendo a quantidade de trocas e o makespan da solução
    string detalheSolucao(vector <vector <int>> solucao){
        string temp;
        temp += "Quantidade de trocas de:" + to_string(getQuantidadeTrocas(solucao)) + "\n";
        temp += "Makespan: " + to_string(funcaoAvaliativa(solucao)) + "\n";

        return temp;
    }
};

#endif