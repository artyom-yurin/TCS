#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <map>

using namespace std;

struct Transaction {
    string firstState;
    string nameTransaction;
    string secondState;
};

vector<string> getValues(const string &states, const string &prefix) {
    string temp = states.substr(0, states.size());
    vector<string> result;
    size_t startPosition = temp.find(prefix, 0);
    if (startPosition != string::npos) {
        temp = temp.replace(startPosition, prefix.size(), "");
        startPosition = 0;
        while (startPosition != string::npos) {
            size_t finishPosition = temp.find(',', startPosition);
            if (finishPosition == string::npos) {
                finishPosition = temp.find('}', startPosition);
            }
            if (finishPosition != string::npos) {
                string value = temp.substr(startPosition, finishPosition - startPosition);
                if (!value.empty()) {
                    result.push_back(value);
                }
                startPosition = finishPosition + 1;
            } else {
                startPosition = string::npos;
            }
        }
    }
    return result;
}

vector<Transaction> getTransactions(const string &transString) {
    vector<string> trans = getValues(transString, "trans={");
    vector<Transaction> result;
    while (!trans.empty()) {
        string transactionString = trans.back();
        trans.pop_back();
        Transaction newTrans;
        size_t startPosition = 0;
        size_t finishPosition = transactionString.find('>', startPosition);
        newTrans.firstState = transactionString.substr(startPosition, finishPosition);
        startPosition = finishPosition + 1;
        finishPosition = transactionString.find('>', startPosition);
        newTrans.nameTransaction = transactionString.substr(startPosition, finishPosition - startPosition);
        startPosition = finishPosition + 1;
        newTrans.secondState = transactionString.substr(startPosition);
        result.push_back(newTrans);
    }
    return result;
}

int checkState(const string & stateString, vector<string> &states, ostream &output) {

    int index = -1;
    bool e1 = true;
    for (int i = 0; i < states.size(); i++) {
        if (!(states[i].compare(stateString))) {
            e1 = false;
            index = i;
        }
    }

    if (e1) {
        output << "Error:\nE1: A state " + stateString +" is not in set of states\n";
    }
    return index;
}

int checkInitialState(vector<string> &initState, vector<string> &states, ostream &output) {
    if (initState.empty()) {
        output << "Error:\nE4: Initial state is not defined";
        return -1;
    }

    return checkState(initState.back(), states, output);
}

void dfs (int v, bool * used, int** g, unsigned long size) {
    used[v] = true;
    for (int i = 0; i < size; i++)
        if(g[v][i] == 1)
        {
            if (!used[i])
                dfs (i, used, g, size);
        }
}

bool checkJoint(int initStateId, vector<string> states, vector<Transaction> transactions)
{
    int** g = new int*[states.size()];
    for(int i = 0; i < states.size(); ++i)
        g[i] = new int[states.size()];

    bool* used = new bool [states.size()];

    map<string, int> stateId;

    for(int i = 0; i < states.size();i++)
    {
        used[i] = false;
        stateId.insert(pair<string, int>(states[i], i));
        for(int j = 0; j < states.size(); j++)
        {
            g[i][j] = 0;
        }
    }

    for(Transaction & trans: transactions)
    {
        g[stateId[trans.firstState]][stateId[trans.secondState]] = 1;
        g[stateId[trans.secondState]][stateId[trans.firstState]] = 1;
    }

    dfs(initStateId, used, g, states.size());

    for(int i =0; i < states.size(); i++)
    {
        if(!used[i])
        {
            return false;
        }
    }
    return true;
}

int main() {
    std::ifstream input("fsa.txt");
    std::ofstream output("result.txt");

    if (!input.is_open()) {
        std::cerr << "File doesn't exist" << std::endl;
        return 1;
    }

    std::string stateString;
    std::getline(input, stateString);
    vector<string> states = getValues(stateString, "states={");

    std::string alphaString;
    std::getline(input, alphaString);
    vector<string> alpha = getValues(alphaString, "alpha={");

    std::string initStateString;
    std::getline(input, initStateString);
    vector<string> initState = getValues(initStateString, "init.st={");

    int initStateId = checkInitialState(initState, states, output);

    if (initStateId == -1) {
        return 0;
    }
    std::string finalStateString;
    std::getline(input, finalStateString);
    vector<string> finalStates = getValues(finalStateString, "fin.st={");

    std::string transString;
    std::getline(input, transString);
    vector<Transaction> transactions = getTransactions(transString);

    for (Transaction trans: transactions) {
        if(checkState(trans.firstState, states, output) != -1 && checkState(trans.secondState, states, output) != -1)
        {
            bool exist = false;
            for (const string &alph: alpha) {
                if (trans.nameTransaction.compare(alph) == 0) {
                    exist = true;
                }
            }

            if(!exist)
            {
                output << "Error:\nE3: A transition a is not represented in the alphabet\n";
                return 0;
            }
        } else {
            return 0;
        }
    }

    if(!checkJoint(initStateId, states, transactions))
    {
        output << "Error:\nE2: Some states are disjoint\n";
        return 0;
    }

    output.close();
    return 0;
}