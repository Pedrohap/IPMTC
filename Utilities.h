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

#endif