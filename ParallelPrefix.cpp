#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <mutex>
#include <thread>
#include <unistd.h>
#include <deque>
#include <condition_variable>
#include <sstream> 
#include <math.h> 
#include<functional>
#include <fstream>
using namespace std;

template <class T>
class ParallelPrefix{
    public:
        void init(vector<T> input, int numberWorkers, function<T(T, T)> f);
    private:
        void master(vector<T> input, int numberWorkers, function<T(T, T)> f);
        void compute(int start, int end, int level, int iterFactor, function<T(T, T)> f);
        vector<vector<T>> interValue;
        int elChuck;
        int leng;
};

template <class T>
void ParallelPrefix<T>::init(vector<T> input, int numberWorkers, function<T(T, T)> f){
    //init di interValue: intervalue[0] = input
    leng = ceil(log2(input.size()))+1;
    interValue.resize(leng);
    interValue[0] = input;
    for(int i = 0; i < interValue.size(); i++)
        interValue[i].resize(input.size());
    // split del lavoro tra i workers possibili
    elChuck  = ceil(interValue[0].size()/numberWorkers);
    auto start   = std::chrono::high_resolution_clock::now();
    master(input, numberWorkers, f);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto msec    = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    string title = to_string(numberWorkers)+"_"+to_string(input.size())+"_"+"C.txt";
    ofstream output;
    output.open(title);
    leng --;
    for(int i= 0; i < input.size(); i++){
        output<<interValue[leng][i]<<" ";
    }
    output << "\n Elapsed time is " << msec << " msecs ";    
    output.close();
}    

template <class T>
void ParallelPrefix<T>::master(vector<T> input, int numberWorkers, function<T(T, T)> f){
    //sistemo chucnks index
    int start = 0, end = elChuck;
    // mando in esecuzione, aspetto il ritorno dei threads 
    vector<thread> workers;
    workers.resize(numberWorkers);
    int level = 1;
    for(int iterFactor = 1; iterFactor < interValue[0].size(); iterFactor= iterFactor*2){
        for(int i = 0; i < numberWorkers; i++){
            if(i == numberWorkers-1){
                workers[i] = thread(&ParallelPrefix<T>::compute, this, start, interValue[0].size(), level, iterFactor, ref(f));
            }
            else {
                workers[i] = thread(&ParallelPrefix<T>::compute, this, start, end, level, iterFactor, ref(f));
                start += elChuck;
                end += elChuck; 
            }    
        }
        for(int i = 0; i < numberWorkers; i++){
            workers[i].join();
        }
        start = 0;
        end = elChuck;  
        level++;     
    }
}    

template <class T>
void ParallelPrefix<T>::compute(int start, int end, int level, int iterFactor, function<T(T,T)> f){
    int oldLevel = level-1;
    while(start < end){
        if(start < iterFactor){
            interValue[level][start] = interValue[oldLevel][start];
        }
        else{
            interValue[level][start] = f(interValue[oldLevel][start], interValue[oldLevel][start-iterFactor]);
        }
        start++;
    }
}

int easySum(int x, int y) {return x+y;}


int main(int argc, char* argv[]){
    int numberElements = atoi(argv[1]);
    int numberWorkers = atoi(argv[2]);
    for(int k = 0; k < 1; k++){
        ParallelPrefix<int> prefix;
        vector<int> inputs;
        for(int i =1; i < numberElements; i ++)
            inputs.push_back(i);
        function<int(int, int)> f = easySum; 
        prefix.init(inputs, numberWorkers, f);
    }
    
    return 0;
}

