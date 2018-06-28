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
        void initTest(vector<T> input, int maxNumberWorkers, function<T(T, T)> f);
    private:
        void master(int numberWorkers, function<T(T, T)> f);
        void compute(int start, int end, int level, int iterFactor, function<T(T, T)> f);
        vector<vector<T>> interValue;
        int elChuck;
        int level;
};

template <class T>
void ParallelPrefix<T>::init(vector<T> input, int numberWorkers, function<T(T, T)> f){
    //init di interValue: intervalue[0] = input
    interValue.resize(2);
    interValue[0] = input;
    interValue[1].resize(input.size());
    // split del lavoro tra i workers possibili
    elChuck  = ceil(interValue[0].size()/numberWorkers);
    auto start   = std::chrono::high_resolution_clock::now();
    master(numberWorkers, f);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto msec    = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    string title = to_string(numberWorkers)+"_"+to_string(input.size())+"_"+"C.txt";
    ofstream output;
    output.open(title);
    if(level == 1) level = 0;
    else level = 1;
    for(int i= 0; i < input.size(); i++){
        output<<interValue[level][i]<<" ";
    }
    output << "\n Elapsed time is " << msec << " msecs ";    
    output.close();
}    


template <class T>
void ParallelPrefix<T>::initTest(vector<T> input, int maxNumberWorkers, function<T(T, T)> f){
    //init di interValue: intervalue[0] = input
    interValue.resize(2);
    interValue[0] = input;
    interValue[1].resize(input.size());
    string title = to_string(input.size())+"_"+"Test.txt";
    ofstream output;
    output.open(title);
    for(int k = 1; k < maxNumberWorkers; k++){
        auto start   = std::chrono::high_resolution_clock::now();
        elChuck  = ceil(interValue[0].size()/k);
        master(k, f);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto msec    = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        output << k<<", "<<msec;  
        }
    output.close();
}    


template <class T>
void ParallelPrefix<T>::master(int numberWorkers, function<T(T, T)> f){
    //sistemo chucnks index
    int start = 0, end = elChuck;
    // mando in esecuzione, aspetto il ritorno dei threads 
    vector<thread> workers;
    workers.resize(numberWorkers);
    level = 1;
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
        if(level == 1) level = 0;
        else level = 1;    
    }
}    

template <class T>
void ParallelPrefix<T>::compute(int start, int end, int level, int iterFactor, function<T(T,T)> f){
    int oldLevel;
    if(level == 0) oldLevel = 1;
    else oldLevel = 0;
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