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
using namespace std;

template <typename T>
class queue{

    private:

    mutex d_mutex;
    condition_variable d_condition;
    deque<T> d_queue;

    public:

    void push(T const& value) {
        {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        d_queue.push_front(value);
        this->d_condition.notify_one();
        }
    }

    T pop() {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        this->d_condition.wait(lock, [=]{ return !this->d_queue.empty(); });
        T rc(std::move(this->d_queue.back()));
        this->d_queue.pop_back();
        return rc;
        }
};

class pipeline4 {

    private:

    queue<vector<double>*> queue_gs;
    queue<vector<double>*> queue_ss;
    queue<vector<double>*> queue_se;

    public:

    void generator(int);
    void stage1();
    void stage2();
    void end();
    double waste(long, double);
};

void pipeline4::generator(int number_of_vectors){

    //crea i thread ed assegna ad ognuno il giusto compito
    thread stage1 = thread(&pipeline4::stage1, this); 
    thread stage2 = thread(&pipeline4::stage2, this);
    thread print = thread(&pipeline4::end, this);

    //crea vettore per inserire i puntatori ai vettori che verranno generati
    vector<vector<double>*> vecs(number_of_vectors);

    //ad ogni iterazione crea un nuovo vettore, inserisci il suo puntatore nella coda12
    for (int i = 0; i < number_of_vectors; i++) {
        vecs[i] = new vector<double>();
        vector<double> v = * vecs[i];
        int lung = rand() % 20 + 1;
        for(int i = 0; i < lung; i ++)
            v.push_back(rand()); 
        *vecs[i] = v;    
        queue_gs.push(vecs[i]);
        
    }

    //per notificare la fine dello stream inserisco un puntatore a null nella coda12
    vector<double>* last = NULL;
    queue_gs.push(last);
    
    //join dei thread che hanno concluso
    stage1.join();
    stage2.join();
    print.join();
        
}

double pipeline4::waste(long iter, double x) {
    for(long i=0; i<iter; i++)
        x = sin(x);
    return(x);
}    

void pipeline4::stage1(){
    //prendi vettore
    //fai la somma
    //metti il puntatore a coda 23

    while(true){
        vector<double>* vec = queue_gs.pop();

        // check per gestire eos
        if(vec == NULL){
            queue_ss.push(vec);
            break;
        }

        vector<double> v = *vec;
        for (auto i = v.begin(); i != v.end(); ++i){
            *i = *i + 20 + waste(8000, 3.4);
        }    
        *vec = v;    
        queue_ss.push(vec);
    }
}

void pipeline4::stage2(){

    //prendi vettore
    //fai la moltiplicazione
    //metti il puntatore nella coda 34
    while(true){
        vector<double>* vec = queue_ss.pop();

        //check per gestire eos    
        if(vec == NULL){
            queue_se.push(vec);
            break;
        }

        vector<double> v = *vec;
        for (auto i = v.begin(); i != v.end(); ++i){
            *i = (*i)*2*waste(8000, 6.7);
        }            
        *vec = v;
        queue_se.push(vec);
    }
    
}

void pipeline4::end(){
    //prendi il vettore
    //stampa
    while(true){
        vector<double>* vec = queue_se.pop();

        //check per gestire eos
        if(vec == NULL){
            cout<<"End of the stream \n";
            break;
        }

        vector<double> v = *vec;
        cout<<"Result vector: \n";
        for (auto i = v.begin(); i != v.end(); ++i){
            cout << *i << ' ';
        }
        cout<<"\n";
    }
}

int main(){
    pipeline4 pipetry;
    pipetry.generator(100);
    return 0;
}
