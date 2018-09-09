#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Transaction{
    string firstState;
    string nameTransaction;
    string finalTransaction;
};

vector<Transaction> getTransactions(vector<string> trans)
{
    vector<Transaction> result;
    while(!trans.empty())
    {
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
        newTrans.finalTransaction = transactionString.substr(startPosition);
        result.push_back(newTrans);
    }
    return result;
}

vector<string> getValues(string states, const string &prefix) {
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
                if (!value.empty())
                {
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

    if(initState.empty())
    {
        output << "Error:\nE4: Initial state is not defined";
        return 0;
    }

    bool e1 = true;
    for(const string &state: states)
    {
        if(!(state.compare(initState.back())))
        {
            e1 = false;
        }
    }

    if(e1)
    {
        output << "Error:\nE1: A state s is not in set of states\n";
        return 0;
    }

    std::string finalStateString;
    std::getline(input, finalStateString);
    vector<string> finalStates = getValues(finalStateString, "fin.st={");

    std::string transString;
    std::getline(input, transString);
    vector<Transaction> transactions = getTransactions(getValues(transString, "trans={"));

    bool e3 = false;
    for (Transaction trans: transactions) {
        if(!e3)
        {
            bool exist = false;
            for(const string &alph: alpha)
            {
                if(trans.nameTransaction.compare(alph) == 0)
                {
                    exist = true;
                }
            }
            e3 = !exist;
        }
    }

    if(e3)
    {
        output << "Error:\nE3: A transition a is not represented in the alphabet\n";
        return 0;
    }

    output.close();
    return 0;
}