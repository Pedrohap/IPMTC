#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <random>
#include <string>


using namespace std;

int randomInt(int min, int max){
    max--;
    random_device rd;
    mt19937 generator(rd());

    uniform_int_distribution<int> distribution(min, max);

    int random_number = distribution(generator);

    return random_number;
}

float randomFloat(float min, float max) {
    random_device rd;
    mt19937 generator(rd());
    uniform_real_distribution<float> distribution(min, max);
    float random_number = distribution(generator);
    return random_number;
}

//Retorna o inteiro do ultimo numero de um float
int getLastDigit(float num) {
    string numStr = to_string(num);
    char lastChar = numStr[numStr.size() - 1];
    int lastDigit = lastChar - '0'; // Convertendo caractere para inteiro
    return lastDigit;
}

//Retorna o inteiro do primeiro numero de um float
int getFirstDigit(float num) {
    string numStr = to_string(num);
    char firstChar = numStr[0];
    int firstDigit = firstChar - '0'; // Convertendo caractere para inteiro
    return firstDigit;
}


void debugPrintMatriz(string menssagem, vector <vector <int>>& matriz){
    cout << menssagem << endl;
    for(int i = 0; i < matriz.size(); i++){
        for(int j = 0; j < matriz[i].size(); j++){
            cout << matriz[i][j] << " ";
        }
        cout << endl;
    }
}

string debugStringMatriz(string menssagem, vector <vector <int>> matriz){
    string temp;
    temp += menssagem + "\n";
    for(int i = 0; i < matriz.size(); i++){
        for(int j = 0; j < matriz[i].size(); j++){
            temp += to_string(matriz[i][j]) + " ";
        }
        temp += "\n";
    }

    return temp;
}

void debugPrintVectorPair(string menssagem, vector <pair <int,int>>& vetor){
    cout << menssagem << endl;
    for(int i = 0; i < vetor.size(); i++){
        cout << vetor[i].first << "," << vetor[i].second << " ";
    }
    cout << endl;
}

//Simplesmente imprime uma linha separadora
void printLinha(){
    cout << "------------------------------------------------" << endl;
}

void debugPrintVector(string menssagem, vector <int> vetor){
    cout << menssagem << endl;
    for(int i = 0; i < vetor.size(); i++){
        cout << vetor[i] << " ";
    }
    cout << endl;
}

//Remove o elemento informado do vetor
void removeDoVector (vector <int>& vetor, int element){
    auto iterator = find(vetor.begin(), vetor.end(), element);
    if (iterator != vetor.end()){
        vetor.erase(iterator);
    } else {
        cout << "ELEMENTO NÃO ENCONTRADO" << endl;
        exit(EXIT_FAILURE);
    }
}

template<typename T>
void shuffleVec (vector<T>& vetor){
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    
    shuffle (vetor.begin(), vetor.end(), default_random_engine(seed));
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

//Função EXCLUSIVA para ordernar um pair de Int, Int baseado em seu segundo valor
bool sortBySecond(const pair<int, int> &a, const pair<int, int> &b) {
    return a.second < b.second;
}

//Função EXCLUSIVA para ordernar em forma crecente um pair de Int, Float baseado em seu segundo valor
bool sortBySecondFloat(const pair<int, float> &a, const pair<int, float> &b) {
    return a.second < b.second;
}

//Função EXCLUSIVA para ordernar em forma decrecente um pair de Int, Float baseado em seu segundo valor
bool sortBySecondFloatDecrecente(const pair<int, float> &a, const pair<int, float> &b) {
    return a.second > b.second;
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

string boolToString(bool value){
    if(value){
        return "sim";
    } else {
        return "não";
    }
}

vector<string> separarString(const string entrada) {
    vector<string> partes;
    string entrada_copy = entrada; // Faz uma cópia da entrada para poder modificá-la
    string parte;
    size_t pos = 0;
    while ((pos = entrada_copy.find('/')) != string::npos) {
        parte = entrada_copy.substr(0, pos);
        partes.push_back(parte);
        entrada_copy.erase(0, pos + 1);
    }
    pos = entrada_copy.find('.');
    partes.push_back(entrada_copy.substr(0, pos)); // Adiciona a parte antes do ponto
    partes.push_back(entrada_copy.substr(pos));    // Adiciona a extensão do arquivo
    return partes;
}

//Recebe um vetor e inverte o intervalo nele contido;
template<typename T>
void inverterIntervalo(vector<T>& vetor, int inicio, int fim) {
    if (inicio < 0 || fim >= vetor.size() || inicio >= fim) {
        cout << "Intervalo inválido." << endl;
        return;
    }

    while (inicio < fim) {
        swap(vetor[inicio], vetor[fim]);
        inicio++;
        fim--;
    }
}

#endif