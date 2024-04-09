#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <random>


using namespace std;

int randomInt(int min, int max){
    max--;
    random_device rd;
    mt19937 generator(rd());

    uniform_int_distribution<int> distribution(min, max);

    int random_number = distribution(generator);

    return random_number;
}

void debugPrintMatriz(string menssagem, vector <vector <int>> matriz){
    cout << menssagem << endl;
    for(int i = 0; i < matriz.size(); i++){
        for(int j = 0; j < matriz[i].size(); j++){
            cout << matriz[i][j] << " ";
        }
        cout << endl;
    }
}

void debugPrintVector(string menssagem, vector <int> vetor){
    cout << menssagem << endl;
    for(int i = 0; i < vetor.size(); i++){
        cout << vetor[i] << " ";
    }
    cout << endl;
}

void removeDoVector (vector <int>& vetor, int element){
    auto iterator = find(vetor.begin(), vetor.end(), element);
    if (iterator != vetor.end()){
        vetor.erase(iterator);
    } else {
        cout << "ELEMENTO NÃO ENCONTRADO" << endl;
    }
}

int getPosElementoVector(vector <int>& vetor,int elemento){

    auto iterator = find(vetor.begin(), vetor.end(), elemento);

    if (iterator != vetor.end()) {
        int posicao = distance(vetor.begin(),iterator);
        
        return posicao;
    } else {
        return -1;
    }
}

//Função EXCLUSIVA para ordernar um pair baseado em seu segundo valor
bool sortBySecond(const pair<int, int> &a, const pair<int, int> &b) {
    return a.second < b.second;
}

//Função EXCLSUIVA de comparação para o sort dos tuple
bool comparacaoNoSegundoTuple(const tuple<int, int, int>& a, const tuple<int, int, int>& b) {
    // Comparando o segundo elemento
    if (get<2>(a) == get<2>(b)) {
        // Se os segundos elementos são iguais, use o primeiro elemento para desempatar
        return get<1>(a) < get<1>(b);
    }
    // Se os segundos elementos não são iguais, ordene normalmente por eles
    return get<2>(a) < get<2>(b);
}


#endif