# Proposta de Heurística e Metaheurística para o IPMTC

Este projeto consiste na implementação de uma proposta de heurística e metaheurística para resolver o problema do IPMTC (Identical Parallel Machines with Tools Constraint). O IPMTC é um problema de otimização combinatória com aplicações em sistemas de manufatura.

## Sobre o Problema

O IPMTC envolve a alocação de tarefas em máquinas paralelas idênticas, levando em consideração restrições de ferramentas. As instâncias utilizadas para benchmark foram propostas por Andreza Cristina Beezao Moreira.

## Implementação

O projeto é desenvolvido em C++ e visa oferecer uma solução eficiente para o IPMTC. Inclui a implementação de uma heurística e uma metaheurística para encontrar soluções ótimas ou próximas do ótimo.

## Como Compilar

Para compilar no Windows com MinGW use: `g++ -O3 -march=native -o loader loader.cpp -static-libgcc -static-libstdc++`

Para compilar no Linux use: `g++ -O3 -march=native -o loader loader.cpp -static-libgcc -static-libstdc++ -std=c++17`

Para compilar o target-runner no Windows com MinGW use: `g++ -o target-runner target-runner.cpp -static-libgcc -static-libstdc++`

Para compilar o target-runner no Linux use: `g++ -o target-runner target-runner.cpp -static-libgcc -static-libstdc++ -std=c++17`

Execute o arquivo `loader` e o programma será executado.