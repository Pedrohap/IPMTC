#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <omp.h>

#include "Utilities.h"
#include "Decoder.h"



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
        xmax = m - 0.000001;
        cont_stagnado = 0;
        position.resize(tamanho_particula);
        velocity.resize(tamanho_particula);

        for (int i = 0; i < tamanho_particula; i++) {
            position[i] = randomDouble(xmin,xmax);
            velocity[i] = randomDouble(xmin,xmax);
        }
        initial_position = position;
        fitness = evaluate(position,false);
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
        fitness = evaluate(position,false);
        initial_fitness = fitness;
        best_position = position;
        best_fitness = fitness;
    }

    //Função para caso ocorra alguma alteração externa na particle, sendo necessario recalibrar os valore
    void recalcularParticle(){
        fitness = evaluate(position,false);

        if(best_fitness > fitness){
            best_fitness = fitness;
            best_position = position;
        }
    }

    void atualizarVelocidade(vector<double>& global_best_position) {
        #pragma omp parallel for
        for (int i = 0; i < w; i++) {
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
        for (int i = 0; i < w; i++) {
            position[i] += velocity[i];
            while (position[i] < xmin){
                position[i] += xmax;
            }
            while (position[i] > xmax){
                position[i] -= xmax;
            }

            if (position[i] > xmax || position[i] < xmin){
                cout << "ERRO FATAL O LOOP DE ARREDONDAMENTO FALHOU" << endl;
                exit(EXIT_FAILURE);
            }
        }
        fitness = evaluate(position,false);
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
        }
        return false;
    }

    //Verifica se esta particula esta utilizando todas as maquinas disponiveis
    bool isInAllMachines() {
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
        return decode(particle_position, debugs);
    }

    void pertubaParicula(){
        int percent_size = ceil(w * 0.1);
        for (int i = 0; i < percent_size; i++){
            int random_pos = randomInt(0,w);
            position[random_pos] = randomDouble(xmin,xmax);
        }
        cont_stagnado = 0;
    }
};

#endif