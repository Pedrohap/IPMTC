#ifndef GPCA_H
#define GPCA_H

#include <iostream>
#include <vector>
#include <math.h>
#include <atomic>
#include <omp.h>
#include "Utilities.h"

using namespace std;

extern int c; //Capacidade do Mazine
extern int w; //Quantidade de tarefas
extern int t; //Quantidade de ferramentas
extern vector < vector <int> > matriz_ferramentas; //Matriz de tarefas ferramentas, onde a coluna representa a tarefa;

class GPCA {
public:
    int meio = 0 ; /// auxiliary of KTNS PAR
    vector<vector<int>> MD;/// auxiliary of GPCA PAR
    int sum = 0; /// auxiliary of GPCA
	int pipes_countD = 0;/// auxiliary of GPCA PAR
    

    unsigned int GPCAnoPar(vector<int> s){	
	    sum = 0;
	    int pipes_count = 0;
	    int last_full = 0;
	    vector<int> last_seen(t);
	    vector<vector<int>> M;

	    vector<vector<bool>> toolJob;
        vector<vector <int>> JobTools;

        toolJob.assign(t, vector<bool>(w,false));
        JobTools.resize(w);
        //Preenche a matriz de tooljob e Job Tools
        for(int i = 0 ; i < matriz_ferramentas.size(); i++){
            for (int j = 0; j < matriz_ferramentas[i].size(); j++){
                if(matriz_ferramentas[i][j] == 1){
                    toolJob[i][j] = true;
                    JobTools[j].push_back(i);
                    ++sum;
                }
            }
        }

	    //Completa o last_seen
	    for(unsigned int i = 0; i < t; ++i){
	    	if(toolJob[i][s[0]]) last_seen[i] = 0;
	    	else last_seen[i] = -1;
	    }
	    M.push_back(JobTools[s[0]]);
    
    
	    for(unsigned int e = 1; e < w; ++e){
	    	M.push_back(JobTools[s[e]]);
	    	for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
	    		if(last_full <= last_seen[*t]){
	    			++pipes_count;
	    			for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
	    				M[i].push_back(*t);					
	    				if(M[i].size() == c) last_full = i;
	    			}
	    		}
	    		last_seen[*t] = e; 	
	    	}
	    	if(M[e].size() == c) last_full = e;

	    }
        return (sum - c - pipes_count);
    }

    unsigned int GPCAPar(vector<int> s) {	
        sum = 0;
        int pipes_count = 0;
        int last_full = 0;
        vector<int> last_seen(t);
        vector<vector<int>> M;

        vector<vector<bool>> toolJob(t, vector<bool>(w, false));
        vector<vector<int>> JobTools(w);

        // Preenche a matriz de tooljob e Job Tools
        #pragma omp parallel for reduction(+:sum)
        for (int i = 0; i < matriz_ferramentas.size(); i++) {
            for (int j = 0; j < matriz_ferramentas[i].size(); j++) {
                if (matriz_ferramentas[i][j] == 1) {
                    #pragma omp critical
                    {
                        toolJob[i][j] = true;
                        JobTools[j].push_back(i);
                        ++sum;
                    }
                }
            }
        }

        // Completa o last_seen
        #pragma omp parallel for
        for (unsigned int i = 0; i < t; ++i) {
            if (toolJob[i][s[0]]) last_seen[i] = 0;
            else last_seen[i] = -1;
        }

        M.push_back(JobTools[s[0]]);

        for (unsigned int e = 1; e < w; ++e) {
            M.push_back(JobTools[s[e]]);
            #pragma omp parallel for
            for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t) {
                int toolIndex = *t;
                if (last_full <= last_seen[toolIndex]) {
                    #pragma omp atomic
                    ++pipes_count;

                    for (unsigned int i = (last_seen[toolIndex] + 1); i < e; ++i) {
                        M[i].push_back(toolIndex);
                        if (M[i].size() == c) {
                            #pragma omp critical
                            {
                                last_full = i;
                            }
                        }
                    }
                }
                last_seen[toolIndex] = e; 	
            }
            if (M[e].size() == c) {
                #pragma omp critical
                last_full = e;
            }
        }

        return (sum - c - pipes_count);
    }

    /*unsigned int GPCAPar(vector<int> s){
        meio = ceil((float)w/2.0);
	    vector <int> neigh = s;
	    atomic_bool fase = false;

        sum = 0;
        int pipes_count = 0;
	    int last_full = 0;
        
	    vector<int> last_seen(t);
	    vector<vector<int>> M;
	    vector<vector<bool>> toolJob;
        vector<vector <int>> JobTools;

        toolJob.assign(t, vector<bool>(w,false));
        JobTools.resize(w);
        //Preenche a matriz de tooljob e Job Tools
        for(int i = 0 ; i < matriz_ferramentas.size(); i++){
            for (int j = 0; j < matriz_ferramentas[i].size(); j++){
                if(matriz_ferramentas[i][j] == 1){
                    toolJob[i][j] = true;
                    JobTools[j].push_back(i);
                    ++sum;
                }
            }
        }

	    //Completa o last_seen
	    for(unsigned int i = 0; i < t; ++i){
	    	if(toolJob[i][s[0]]) last_seen[i] = 0;
	    	else last_seen[i] = -1;
	    }
	    M.push_back(JobTools[s[0]]);
    
    
	    for(unsigned int e = 1; e <= meio; ++e){
            M.push_back(JobTools[s[e]]);		
                for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
                    if(last_full <= last_seen[*t]){
                        ++pipes_count;
                        
                        for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
                            M[i].push_back(*t);					
                            if(M[i].size() == c) last_full = i;
                        }
                    }
                    last_seen[*t] = e; 	
                }
            if(M[e].size() == c) last_full = e;
	    }

        int last_fullD = last_full;
        pipes_countD = pipes_count;
        MD = M;
	    //while(!endD){}
	    //endD = false;


	    for(unsigned int e = (meio+1); e <= last_fullD; ++e){
	    	M.push_back(MD[((w-1) - e)]);		
	    	for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
	    		if((last_full <= last_seen[*t]) && (last_seen[*t] != meio)){
	    			++pipes_count;			
	    			for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
	    				M[i].push_back(*t);					
	    				if(M[i].size() == c) last_full = i;
	    			}
	    		last_seen[*t] = -1; 	
	    		}
	    	}
	    	if(M[e].size() == c) last_full = e;
	    }


        return (sum - c - (pipes_count+pipes_countD));
    }
*/
};



#endif