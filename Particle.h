#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <omp.h>

#include "Utilities.h"
#include "Decoder.h"

extern const int DECODER_VER;

//const double c1 = 2.0;
//const double c2 = 2.0;

//Inercia ou o "w estranho"
//const double omega = 1.0;

//iRace Mode
extern double c1;
extern double c2;
extern double omega;

extern int m;
extern int w;
using namespace std;

class Particle{
public:
    double initial_fitness;
    vector<double> initial_position;
    vector<double> position;
    vector<double> velocity;
    vector<double> best_position;
    double fitness;
    double best_fitness;
    double xmin;
    double xmax;
    int cont_stagnado;
    
    //Construtor de uma Particula aleatoria
    Particle(int tamanho_particula) {
        xmin = 0.0;

        if (DECODER_VER == 1){
            xmax = m - 0.000001;
        } else if (DECODER_VER == 2){
            xmax = 1 - 0.000001;
        }

        cont_stagnado = 0;
        position.resize(tamanho_particula);
        velocity.resize(tamanho_particula);

        IPMTC ipmtc;
        if (DECODER_VER == 1){
            position = recode(ipmtc.gerarSolucaoHeuristicaBasica(false));
            fitness = evaluate(position,false);
        } else if (DECODER_VER == 2){
            position = recode2(ipmtc.gerarSolucaoHeuristicaBasica(false));
            
            //Teste de recoder
            /*vector <vector <int>> new_solutations = ipmtc.gerarSolucaoHeuristicaBasica(false);
            position = recode2(new_solutations);
            vector <vector <int>> new_solutations_decoded = decode2(position);
            if(new_solutations_decoded != new_solutations){
                cout << "RECODER FALHOU KRAI FUDEU!" << endl;
                exit(EXIT_FAILURE);
            }*/

            fitness = evaluate2(position,false);
        }

        velocity = position;

        /*for (int i = 0; i < tamanho_particula; i++) {
            position[i] = randomDouble(xmin,xmax);
            velocity[i] = randomDouble(xmin,xmax);
        }*/
       
        initial_position = position;
        initial_fitness = fitness;
        best_position = position;
        best_fitness = fitness;
    }

    //Contrutor recebendo uma posição/particula como referencia
    Particle(vector<double>& particula) {
        position.resize(particula.size());
        velocity.resize(particula.size());
        cont_stagnado = 0;

        for (int i = 0; i < particula.size(); i++) {
            position[i] = particula[i];
            velocity[i] = randomDouble(xmin,xmax);
        }
        initial_position = position;
        if (DECODER_VER == 1){
            fitness = evaluate(position,false);
        } else if (DECODER_VER == 2){
            fitness = evaluate2(position,false);
        }
        initial_fitness = fitness;
        best_position = position;
        best_fitness = fitness;
    }

    //Função para caso ocorra alguma alteração externa na particle, sendo necessario recalibrar os valore
    void recalcularParticle(){
        if (DECODER_VER == 1){
            fitness = evaluate(position,false);
        } else if (DECODER_VER == 2){
            fitness = evaluate2(position,false);
        }

        if(best_fitness > fitness){
            best_fitness = fitness;
            best_position = position;
        }
    }

    void atualizarVelocidade(vector<double>& global_best_position) {
        //#pragma omp parallel for
        for (int i = 0; i < global_best_position.size(); i++) {
            double r1 = randomDouble(0,1);
            double r2 = randomDouble(0,1);
            velocity[i] = (omega * velocity[i]) + (c1 * r1) * (best_position[i] - position[i]) + (c2 * r2) * (global_best_position[i] - position[i]);
            if (false){
                cout << "Velocidade [" << i << "] = (" << omega << " * " << velocity[i] << ") + (" << c1 << " * " << r1 << ") * (" << best_position[i] << " - " << position[i] << ") + (" << c2 << " * " << r2 << ") * (" << global_best_position[i] << " - " << position[i] << ") \n";
            }
        }

        
    }

    //Retorna true, se houve melhora local e retorna false caso não
    bool atualizarPosicao() {
        //#pragma omp parallel for
        for (int i = 0; i < position.size(); i++) {
            position[i] += velocity[i];
            // Normaliza usando fmod
            position[i] = fmod(position[i] - xmin, xmax - xmin) + xmin;
        
            // Se position[i] for negativa após o fmod, ajusta para o intervalo
            if (position[i] < xmin) {
                position[i] += (xmax - xmin);
            }

            if (position[i] > xmax || position[i] < xmin){
                cout << "ERRO FATAL O LOOP DE ARREDONDAMENTO FALHOU" << endl;
                cout << position[i] << " <||> xmax:" << xmax << " || xmin:" << xmin << " Velocity: " << velocity[i] << endl;
                exit(EXIT_FAILURE);
            }
        }

        if (DECODER_VER == 1){
            fitness = evaluate(position,false);
        } else if (DECODER_VER == 2){
            fitness = evaluate2(position,false);
        }
        if (fitness < best_fitness) {
            best_position = position;
            best_fitness = fitness;

            return true;
        }
        cont_stagnado++;
        if (cont_stagnado >= 50){
            //cout << "Particula pertubada" << endl;
            //Aplica Pertubação nessa particula
            pertubaParicula();
            recalcularParticle();
        }
        return false;
    }

    //Verifica se esta particula esta utilizando todas as maquinas disponiveis
    bool isInAllMachines() {
        if (DECODER_VER == 1){
            vector <int> used_machines(m,0);
            for(int i = 0; i < position.size(); i++){
                used_machines[getFirstDigit(position[i])]++;
            }
            for (int i = 0; i < m ;i++){
                if(used_machines[i] == 0){
                    return false;
                }
            }
            return true;
        } else if (DECODER_VER == 2){
            vector <vector<int>> temp_solution = decode2(position);
            for (int i = 0; i < temp_solution.size(); i++){
                if(temp_solution[i].size() == 0){
                    return false;
                }
            }
            return true;
        }
        cout << "NÃO DEVERIA TER CHEGADO AQUI, FALHA NO ISINALLMACHINES" << endl;
        exit(EXIT_FAILURE);
        return false;
    }

    string toStringCsv(vector<double> particle_position){
        string saida = "";
        for (int i = 0 ; i < particle_position.size(); i++){
            saida += to_string(particle_position[i])  + "|";
        }
        saida += "\n";

        return saida;
    }

    vector < vector <int> > getSolution(vector<double> particle_position){
        bool debugs = false;

        if (DECODER_VER == 1){
            return decode(particle_position, debugs);
        } else if (DECODER_VER == 2){
            return decode2(particle_position, debugs);
        }

        cout << "NÃO DEVERIA TER CHEGADO AQUI, FALHA NO getSolution" << endl;
        exit(EXIT_FAILURE);
        return decode(particle_position, debugs);
    }

    void pertubaParicula(){
        int percent_size = ceil(position.size() * 0.5);
        for (int i = 0; i < percent_size; i++){
            int random_pos = randomInt(0,position.size());
            position[random_pos] = randomDouble(xmin,xmax);
        }
        cont_stagnado = 0;
    }
};

#endif