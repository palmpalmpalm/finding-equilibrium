#include<bits/stdc++.h>

using namespace std;

const int ROCK = 0, PAPER = 1, SCISSORS = 2, NUM_ACTIONS = 3;

vector<double> strategy(NUM_ACTIONS);
vector<double> regretSum(NUM_ACTIONS);
vector<double> strategySum(NUM_ACTIONS);
vector<double> oppStrategy{0.4, 0.3, 0.3};

// get random number between 0 - 1
double getRand(){
    return (double) rand()/RAND_MAX;
}

// get current mixed strategy through regret matching
vector<double> getStrategy(){
    double normalizingSum = 0;
    for (int a = 0; a < NUM_ACTIONS; a++){
        if (regretSum[a] > 0){
            strategy[a] = regretSum[a];
        }else{
            strategy[a] = 0;
        }
        normalizingSum += strategy[a];
    }
    for (int a = 0; a < NUM_ACTIONS; a++){
        if (normalizingSum > 0){
            strategy[a] /= normalizingSum;
        }else{
            strategy[a] = 1.0 / NUM_ACTIONS;
        }
        strategySum[a] += strategy[a];
    }
    return strategy;
}

// get random action according to mixed-strategy distribution
int getAction(vector<double> strategy){
    double r = getRand();
    int a = 0;
    double cumuProb = 0;
    while (a < NUM_ACTIONS - 1){
        cumuProb += strategy[a];
        if (r < cumuProb) {
            break;
        }
        a++;
    }
    return a;
}

// get average strategy across all training iteration
vector<double> getAverageStrategy(){
    vector<double> avgStrategy(NUM_ACTIONS);
    double normalizingSum = 0;
    for (int a = 0; a < NUM_ACTIONS; a++){
        normalizingSum += strategySum[a];
    }
    for (int a = 0; a < NUM_ACTIONS; a++){
        if (normalizingSum > 0){
            avgStrategy[a] = strategySum[a] / normalizingSum;
        }else{
            avgStrategy[a] = 1.0 / NUM_ACTIONS;
        }
    }
    return avgStrategy;
}


void train(int iteration){
    double actionUtility[NUM_ACTIONS];
    for (int i = 0; i < iteration; i++){
        // get regret matched strategy
        strategy = getStrategy();
        int myAction = getAction(strategy);
        int oppAction = getAction(oppStrategy);

        // compute action utilities
        actionUtility[oppAction] = 0;
        actionUtility[oppAction == NUM_ACTIONS - 1 ? 0 : oppAction + 1] = 1;
        actionUtility[oppAction == 0 ? NUM_ACTIONS - 1 : oppAction - 1] = -1;

        // accumulate action regrets
        for (int a = 0; a < NUM_ACTIONS; a++){
            regretSum[a] += actionUtility[a] - actionUtility[myAction];
        }
    }
}


int main()
{
    srand((unsigned)time(NULL));

    train(10000);

    vector<double> result = getAverageStrategy();
    for (int i = 0; i < NUM_ACTIONS; i++){
        cout<< result[i] << endl;
    }

}
