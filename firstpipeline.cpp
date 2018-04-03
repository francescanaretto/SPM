#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <mutex>
#include <thread>
#include <unistd.h>

using namespace std;
vector<mutex *> mlist(2);
thread stage2_thread;
thread stage3_thread;
thread stage4_thread;

class pipfirst {

    public:
    void stage1 (int);
    void stage2(vector<double>&, int);
    void stage3(vector<double>&, int);
    void stage4(vector<double>&, int);
};

void pipfirst::stage4(vector<double>& v, int i){
    cout<<"ultimo";
    mlist[i]->lock();
    for(int i = 0; i < 10; i ++){
        cout<<v[i];
    }
    mlist[i]->unlock();
}

void pipfirst::stage3(vector<double>& v, int i){
    mlist[i]->lock();
    for(int i = 0; i < 10; i ++){
        v[i] = v[i]*v[i];
    }
    cout<<"terzo thread";
    stage3_thread = thread(&pipfirst::stage4, this, ref(v), i);
    mlist[i]->unlock();
    stage3_thread.detach();
}

void pipfirst::stage2(vector<double>& v, int i){
    mlist[i]->lock();
    cout<<"ANCORA";

    for(int i = 0; i < 10; i ++){
        v[i]= v[i]++;
    }

    cout<<"ANCORA2";
    //m2.lock();
    cout<<"secondo thread";
    stage3_thread = thread(&pipfirst::stage3, this, ref(v), i);
    mlist[i]->unlock();
    stage3_thread.detach();
}
void pipfirst::stage1(int number_of_vectors){
    vector<vector<double>> myvecs(number_of_vectors);
    cout<<"prova";
    for(int i= 0; i < number_of_vectors; i++) {
        vector<double> xi(10);
                cout<<"uno";
        myvecs.push_back(xi);
        mutex *m= new mutex();
        mlist.push_back(m);
    }
    for(int i = 0; i < myvecs.size(); i ++){
        mlist[i]->lock();
        //vector v = myvecs[i];
        for(int j = 0; j < 10; j++){
            myvecs[i][j] = push_back(1);
        }
        usleep(1099);
        cout<<"thread 1 : \n";
        cout<<i;
        stage2_thread = thread(&pipfirst::stage2, this, ref(myvecs[i]), i);
        cout<<"QUIIIII";
        mlist[i]->unlock();
        stage2_thread.detach();
    }

}

int main(){
    pipfirst prova;
    prova.stage1(2);
    return 0;
}
