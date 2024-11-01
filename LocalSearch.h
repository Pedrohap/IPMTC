#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include <iostream>
#include <vector>
#include <utility>
#include "Utilities.h"
#include "Particle.h"

//Houve melhore na busca local do 2APT
extern bool melhora_twoapt;

//Houve melhore na busca local do 2Swap
extern bool melhora_twoswap;

//Media de melhora 2APT
extern double media_melhora_twoapt;

//Media de melhora 2Swap
extern double media_melhora_twoswap;

//Media de melhora Insertion
extern double media_melhora_insertion;

//Media de melhora Exchange
extern double media_melhora_exchange;

using namespace std;

//Fazer o 2 APT, que pega uma particula (vetor de float), seleciona um 
//intervalo de 2 pontos e inverte a ordem nesses 2 pontos

void twoOPT (Particle &particula){
    //Pegar o tamanho da particula
    int tamanho_particula = particula.position.size();
    
    //Cria um vetor com todos os intervalos
    vector <pair <int,int>> posicoes;
    for (int i = 0; i < tamanho_particula ; i++){
        for (int j = (i + 1) ; j < tamanho_particula; j++){
            posicoes.push_back(pair <int,int> (i,j));
        }
    }

    //Mistura esse vetor e seleciona 10% deles
    shuffleVec(posicoes);
    int porcentagem = posicoes.size() * 1;
    
    //Realiza as interações, se alguma houver melhora substituia a original e vida que segue
    Particle new_particula = particula;
    for (int i = 0; i < porcentagem; i++){
        inverterIntervalo(new_particula.position , posicoes[i].first, posicoes[i].second);
        inverterIntervalo(new_particula.velocity , posicoes[i].first, posicoes[i].second);
        new_particula.recalcularParticle();
        if (new_particula.fitness < particula.fitness){
            if (particula.best_fitness > new_particula.fitness){
                media_melhora_twoapt += (particula.best_fitness - new_particula.fitness);
                melhora_twoapt = true;
            }
            particula = new_particula;
        } else{
            new_particula = particula;
        }
    }
}

void twoSwap (Particle &particula){
    //Pegar o tamanho da particula
    int tamanho_particula = particula.position.size();
    
    //Cria um vetor com todos os pontos selecionaveis
    vector <pair <int,int>> posicoes;
    for (int i = 0; i < tamanho_particula ; i++){
        for (int j = (i + 1) ; j < tamanho_particula; j++){
            posicoes.push_back(pair <int,int> (i,j));
        }
    }

    //Mistura esse vetor e seleciona 10% deles
    shuffleVec(posicoes);
    int porcentagem = posicoes.size() * 1;
    
    //Realiza as interações, se alguma houver melhora substituia a original e vida que segue
    Particle new_particula = particula;
    for (int i = 0; i < porcentagem; i++){
        swap(new_particula.position[posicoes[i].first], new_particula.position[posicoes[i].second]);
        swap(new_particula.velocity[posicoes[i].first], new_particula.velocity[posicoes[i].second]);
        new_particula.recalcularParticle();
        if (new_particula.fitness < particula.fitness){
            if (particula.best_fitness > new_particula.fitness){
                media_melhora_twoswap += (particula.best_fitness - new_particula.fitness);
                melhora_twoswap = true;
            }
            particula = new_particula;
        } else{
            new_particula = particula;
        }
    }
}

void insertion(Particle particula){
    bool debug = false;
    vector < vector <int> > decodedSolution = decode(particula.best_position,debug);

    IPMTC ipmtc;

    tuple <int,int,double> solutionResults = ipmtc.funcaoAvaliativaDetalhada(decodedSolution);

    int maquina_critica;
    int maquina_folgada;
    double makespan;
    tie (maquina_critica,maquina_folgada,makespan) = solutionResults;

    //cout << maquina_critica<<" " << maquina_folgada <<" "<< makespan <<" " << ipmtc.getTempoUnico(decodedSolution[maquina_folgada]) <<" " << ipmtc.getTempoUnico(decodedSolution[maquina_critica]) << endl;
    //ipmtc.printSolucaoDetalhada(decodedSolution);
    
    //Pegar 1 tarefa aleatoria do maquina critica:

    vector <int> randomTarefas = decodedSolution[maquina_critica];
    shuffleVec(randomTarefas);

    bool insertionMelhorou = false;

    vector < vector <int> > newSolution = decodedSolution;
    for (int i = 0; i < randomTarefas.size(); i++){
        newSolution = decodedSolution;
        //NÃO TENTE ENVIAR UMA TAREFA QUE O TEMPO PROCESSAMENTO DELA CAUSARIA UM 
        //MAKESPAN MAIOR MESMO NO MELHOR CENARIO
        if( (tempo_tarefa[randomTarefas[i]] + ipmtc.getTempoUnico(decodedSolution[maquina_folgada])) < makespan){
            //Envia a tarefa para a maquina folgada tentando todas as posições
            //O QUE ESTIVER COM MENOS TROCA SERA A MELHOR POSIÇÃO

            //Quantidade de trocas na maquina folgada
            int qtd_trocas_new_folgada = INT16_MAX;
            int best_postion = 0;
            for (int j = 0 ; j < decodedSolution[maquina_folgada].size(); j++){
                vector <int> novaMaquinaFolgada = decodedSolution[maquina_folgada];
                novaMaquinaFolgada.insert(novaMaquinaFolgada.begin() + j, randomTarefas[i]);
                
                if(qtd_trocas_new_folgada > ipmtc.getQuantidadeTrocasMaquina(novaMaquinaFolgada)){
                    best_postion = j;
                    qtd_trocas_new_folgada = ipmtc.getQuantidadeTrocasMaquina(novaMaquinaFolgada);
                }
            }

            //Melhor posição encontrada
            //Realizar a troca e verificar se houve melhora
            removeDoVector(newSolution[maquina_critica],randomTarefas[i]);
            newSolution[maquina_folgada].insert( newSolution[maquina_folgada].begin() + best_postion, randomTarefas[i] );
            
            //Se houve melhora recomeça o loop com a nova solução
            if(ipmtc.funcaoAvaliativa(newSolution) < makespan){
                media_melhora_insertion += (particula.best_fitness - ipmtc.funcaoAvaliativa(newSolution));
                i = -1;
                decodedSolution = newSolution;
                insertionMelhorou = true;

                solutionResults = ipmtc.funcaoAvaliativaDetalhada(decodedSolution);
                tie (maquina_critica,maquina_folgada,makespan) = solutionResults;
                randomTarefas = decodedSolution[maquina_critica];
                shuffleVec(randomTarefas);                
            }
        }
    }

    //Se rodamos todas e melhoramos alteramos a particula que recebemos e encerramos a busca local
    if(insertionMelhorou){
        vector <double> new_particula_vec = (recode(decodedSolution,debug));
        Particle new_particula = Particle(new_particula_vec);
        particula = new_particula;
    }
}

void exchange(Particle particula){
    bool debug = false;
    vector < vector <int> > decodedSolution = decode(particula.best_position,debug);

    IPMTC ipmtc;

    tuple <int,int,double> solutionResults = ipmtc.funcaoAvaliativaDetalhada(decodedSolution);

    int maquina_critica;
    int maquina_folgada;
    double makespan;
    tie (maquina_critica,maquina_folgada,makespan) = solutionResults;

    //Pegar 1 tarefa aleatoria do maquina critica:

    vector <int> randomTarefas = decodedSolution[maquina_critica];
    shuffleVec(randomTarefas);

    bool exchangeMelhorou = false;

    vector < vector <int> > newSolution = decodedSolution;

    //Testar todas as tarefas da maquina critica na troca
    for (int i = 0; i < randomTarefas.size(); i++){
        //pegar essa tarefa da maquina critica e trocar com todas da maquina
        for (int j = 0; j < decodedSolution[maquina_folgada].size(); j++){
            newSolution = decodedSolution;

            if(debug){
                cout << "SOLUÇÃO ANTIGA:" << endl;
                ipmtc.printSolucaoDetalhada(decodedSolution);
                cout << "Saiu da critica a " << randomTarefas[i] << " e saiu da folgada a " << newSolution[maquina_folgada][j] << endl;
            }
            //Realiza a troca
            int pos_tarefa_maquina_critica = getPosElementoVector(decodedSolution[maquina_critica],randomTarefas[i]);
            newSolution[maquina_critica][pos_tarefa_maquina_critica] = newSolution[maquina_folgada][j];
            newSolution[maquina_folgada][j] = randomTarefas[i];

            if(debug){
                cout << "GERANDO A NOVA:" << endl;
                ipmtc.printSolucaoDetalhada(newSolution);
            }

            //Se houve melhora recomeça o loop com a nova solução
            if(ipmtc.funcaoAvaliativa(newSolution) < makespan){
                media_melhora_exchange += (particula.best_fitness - ipmtc.funcaoAvaliativa(newSolution));
                i = -1;

                if(debug){
                    cout << "SOLUÇÃO ANTIGA:" << endl;
                    ipmtc.printSolucaoDetalhada(decodedSolution);
                    
                    cout << "SOLUÇÃO NOVA:" << endl;
                    ipmtc.printSolucaoDetalhada(newSolution);
                }

                decodedSolution = newSolution;
                exchangeMelhorou = true;
                solutionResults = ipmtc.funcaoAvaliativaDetalhada(decodedSolution);
                tie (maquina_critica,maquina_folgada,makespan) = solutionResults;
                randomTarefas = decodedSolution[maquina_critica];
                shuffleVec(randomTarefas);
                break;

             }

        }
    }

    //Se rodamos todas e melhoramos alteramos a particula que recebemos e encerramos a busca local
    if(exchangeMelhorou){
        vector <double> new_particula_vec = (recode(decodedSolution,debug));
        Particle new_particula = Particle(new_particula_vec);
        particula = new_particula;
    }
}

void twoOPTMachine (Particle &particula, bool debugs = true){
    bool debug = false;
    vector < vector <int> > decodedSolution = decode(particula.best_position,debug);

    IPMTC ipmtc;

    tuple <int,int,double> solutionResults = ipmtc.funcaoAvaliativaDetalhada(decodedSolution);

    int maquina_critica;
    int maquina_folgada;
    double makespan;
    tie (maquina_critica,maquina_folgada,makespan) = solutionResults;

    //Pegar o tamanho da particula
    int tamanho_particula = decodedSolution[maquina_critica].size();
    
    //Cria um vetor com todos os intervalos
    vector <pair <int,int>> posicoes;
    for (int i = 0; i < tamanho_particula ; i++){
        for (int j = (i + 1) ; j < tamanho_particula; j++){
            posicoes.push_back(pair <int,int> (i,j));
        }
    }

    //Mistura esse vetor e seleciona 100% deles
    shuffleVec(posicoes);
    int porcentagem = posicoes.size() * 1;
    
    //Realiza as interações, se alguma houver melhora substituia a original e vida que segue
    vector <int> new_makespan_machine = decodedSolution[maquina_critica];
    for (int i = 0; i < porcentagem; i++){
        if (debugs){
            debugPrintVector("Vetor antes de realizar a troca de intevalo:", new_makespan_machine);
            cout << "O intervalo e: " << posicoes[i].first << "," << posicoes[i].second << endl;
        }
        inverterIntervalo(new_makespan_machine , posicoes[i].first, posicoes[i].second);
        debugPrintVector("Vetor apos realizar a troca de intevalo:", new_makespan_machine);

        int temp_fitness = ipmtc.getTempoUnico(new_makespan_machine);
        if (temp_fitness < particula.fitness){
            if (particula.best_fitness > temp_fitness){
                media_melhora_twoapt += (particula.best_fitness - temp_fitness);
                melhora_twoapt = true;
            }
            decodedSolution[maquina_critica] = new_makespan_machine;
            vector <double> new_position = recode(decodedSolution, debug);
            Particle new_particula = Particle(new_position);
            particula = new_particula;
            debugPrintVector("Particula dentro da LS:",particula.position);
        } else {
            new_makespan_machine = decodedSolution[maquina_critica];
        }
    }
}

void twoSwapMachine (Particle &particula){
    bool debug = false;
    vector < vector <int> > decodedSolution = decode(particula.best_position,debug);

    IPMTC ipmtc;

    tuple <int,int,double> solutionResults = ipmtc.funcaoAvaliativaDetalhada(decodedSolution);

    int maquina_critica;
    int maquina_folgada;
    double makespan;
    tie (maquina_critica,maquina_folgada,makespan) = solutionResults;

    //Pegar o tamanho da particula
    int tamanho_particula = decodedSolution[maquina_critica].size();
    
    //Cria um vetor com todos os pontos selecionaveis
    vector <pair <int,int>> posicoes;
    for (int i = 0; i < tamanho_particula ; i++){
        for (int j = (i + 1) ; j < tamanho_particula; j++){
            posicoes.push_back(pair <int,int> (i,j));
        }
    }

    //Mistura esse vetor e seleciona 100% deles
    shuffleVec(posicoes);
    int porcentagem = posicoes.size() * 1;
    
    //Realiza as interações, se alguma houver melhora substituia a original e vida que segue
    vector <int> new_makespan_machine = decodedSolution[maquina_critica];
    for (int i = 0; i < porcentagem; i++){
        swap(new_makespan_machine[posicoes[i].first], new_makespan_machine[posicoes[i].second]);
        int temp_fitness = ipmtc.getTempoUnico(new_makespan_machine);
        if (temp_fitness < particula.fitness){
            if (particula.best_fitness > temp_fitness){
                media_melhora_twoswap += (particula.best_fitness - temp_fitness);
                melhora_twoswap = true;
            }
            decodedSolution[maquina_critica] = new_makespan_machine;
            vector <double> new_position = recode(decodedSolution, debug);
            Particle new_particula = Particle(new_position);
            particula = new_particula;
        } else{
            new_makespan_machine = decodedSolution[maquina_critica];
        }
    }
}
#endif