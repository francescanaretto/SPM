#include "ParallelPrefix.h"

int easySum(int x, int y) {return x+y;}


int main(int argc, char* argv[]){
    int numberElements = atoi(argv[1]);
    //int numberWorkers = atoi(argv[2]);
    vector<int> inputs;
    for(int i =1; i < numberElements; i ++)
        inputs.push_back(i);
    ParallelPrefix<int> prefix;
    function<int(int, int)> f = easySum; 
    prefix.initTest(inputs, 10, f); 
    return 0;
}

