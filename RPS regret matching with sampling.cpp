#include<bits/stdc++.h>

using namespace std;

const int ROCK = 0, PAPER = 1, SCISSORS = 2, NUM_ACTIONS = 3;

vector<double> myStrategy(NUM_ACTIONS);
vector<double> myRegretSum(NUM_ACTIONS);
vector<double> myStrategySum(NUM_ACTIONS);

vector<double> oppStrategy(NUM_ACTIONS);
vector<double> oppRegretSum(NUM_ACTIONS);
vector<double> oppStrategySum(NUM_ACTIONS);

// payoff matrix
double utilMatrix[NUM_ACTIONS][NUM_ACTIONS] = {{0, -1, 1},
                                               {1, 0, -1},
                                               {-1, 1, 0}};

// get random number between 0 - 1
double getRand(){
    return (double) rand()/RAND_MAX;
}

double getUtil(int actionA, int actionB){
    return utilMatrix[actionA][actionB];
}

// get current mixed strategy through regret matching
vector<double> getStrategy(vector<double> regretSum){
    double normalizingSum = 0;
    vector<double> strategy(NUM_ACTIONS);

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
vector<double> getAverageStrategy(vector<double> strategySum){
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

// train cfr
void train(int iteration){

    double myActionUtility[NUM_ACTIONS];
    double oppActionUtility[NUM_ACTIONS];

    for (int i = 0; i < iteration; i++){

        // get regret matched strategy
        myStrategy = getStrategy(myRegretSum);
        oppStrategy = getStrategy(oppRegretSum);

        // add current strategy to strategySum
        for (int a = 0; a < NUM_ACTIONS; a++){
            myStrategySum[a] += myStrategy[a];
            oppStrategySum[a] += oppStrategy[a];
        }

        // get action from strategy
        int myAction = getAction(myStrategy);
        int oppAction = getAction(oppStrategy);

        // compute action utilities
        for (int a = 0; a < NUM_ACTIONS; a++){
            myActionUtility[a] = getUtil(a, oppAction);
            oppActionUtility[a] = getUtil(a, myAction);
        }

        // accumulate action regrets
        for (int a = 0; a < NUM_ACTIONS; a++){
            myRegretSum[a] += myActionUtility[a] - myActionUtility[myAction];
            oppRegretSum[a] += oppActionUtility[a] - oppActionUtility[oppAction];
        }
    }
}


int main()
{
    srand((unsigned)time(NULL));

    train(10000);

    cout<<"-- My Strategy --"<<endl;
    vector<double> myResult = getAverageStrategy(myStrategySum);
    for (int i = 0; i < NUM_ACTIONS; i++){
        cout<< myResult[i] << endl;
    }

    cout<<"-- Opp Strategy --"<<endl;
    vector<double> oppResult = getAverageStrategy(oppStrategySum);
    for (int i = 0; i < NUM_ACTIONS; i++){
        cout<< oppResult[i] << endl;
    }

}
