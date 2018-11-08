#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

struct Transaction {
    string firstState;
    string nameTransaction;
    string secondState;
};

/**
 * Parsing string prefix={values}
 * @param input is input string
 * @param prefix is prefix of string
 * @param output is output stream for error
 * @return vector of values
 */
vector<string> getValues(const string &input, const string &prefix, ofstream & output) {
    string temp = input.substr(0, input.size());
    vector<string> result;
    size_t startPosition = temp.find(prefix, 0);
    if (startPosition != string::npos && startPosition == 0) {
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
    } else {
        output << "Error:\nE5: Input file is malformed";
        output.close();
        exit(0);
    }
    return result;
}

/**
 * Parsing string of transactions
 * @param transString is input string
 * @param output is output stream for error
 * @return vector of transaction
 */
vector<Transaction> getTransactions(const string &transString, ofstream & output) {
    vector<string> trans = getValues(transString, "trans={", output);
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

/**
 * Checking the state of existence
 * @param stateString is name of the state
 * @param states is a set of states
 * @param output is output stream for error
 * @return exist or not
 */
bool checkState(const string &stateString, vector<string> &states, ostream &output) {

    bool e1 = true;
    for (auto &state : states) {
        if (!(state.compare(stateString))) {
            e1 = false;
        }
    }

    if (e1) {
        output << "Error:\nE1: A state \'" + stateString + "\' is not in set of states";
        return false;
    }
    return true;
}

/**
 * Checking initial state
 * @param initState is vector of initial state
 * @param states is vector of states
 * @param output is output stream for error
 * @return correct or not
 */
bool checkInitialState(vector<string> &initState, vector<string> &states, ostream &output) {
    if (initState.size() != 1) {
        output << "Error:\nE4: Initial state is not defined";
        return false;
    }

    return checkState(initState.back(), states, output);
}

/**
 * DFS
 * @param startIndex is start point
 * @param visitedStates is vector of usable points
 * @param graph is adjacency matrix
 * @param size is size of matrix
 */
void dfs(int startIndex, bool *visitedStates, int **graph, unsigned long size) {
    visitedStates[startIndex] = true;
    for (int i = 0; i < size; i++)
        if (graph[startIndex][i] == 1) {
            if (!visitedStates[i])
                dfs(i, visitedStates, graph, size);
        }
}

/**
 * Checking Joint of FSA
 * @param states is vector of states
 * @param transactions is vector of transactions
 * @param output is output stream for error
 * @return joint or disjoint
 */
bool checkJoint(vector<string> &states, vector<Transaction> &transactions, ofstream & output) {
    int **graph = new int *[states.size()];
    for (int i = 0; i < states.size(); ++i)
        graph[i] = new int[states.size()];

    bool *visitedStates = new bool[states.size()];

    map<string, int> stateId;

    for (int i = 0; i < states.size(); i++) {
        visitedStates[i] = false;
        stateId.insert(pair<string, int>(states[i], i));
        for (int j = 0; j < states.size(); j++) {
            graph[i][j] = 0;
        }
    }

    for (Transaction &trans: transactions) {
        graph[stateId[trans.firstState]][stateId[trans.secondState]] = 1;
        graph[stateId[trans.secondState]][stateId[trans.firstState]] = 1;
    }

    dfs(0, visitedStates, graph, states.size());

    for (int i = 0; i < states.size(); i++) {
        if (!visitedStates[i]) {
            output << "Error:\nE2: Some states are disjoint";
            return false;
        }
    }

    return true;
}

/**
 * Checking reachable from initial state
 * @param initialState is name of initial state
 * @param states is vector of states
 * @param transactions is vector of transactions
 * @return All are reachable or not
 */
bool checkReachable(string & initialState, vector<string> &states, vector<Transaction> &transactions) {
    int **graph = new int *[states.size()];
    for (int i = 0; i < states.size(); ++i)
        graph[i] = new int[states.size()];

    bool *visitedStates = new bool[states.size()];

    map<string, int> stateId;

    for (int i = 0; i < states.size(); i++) {
        visitedStates[i] = false;
        stateId.insert(pair<string, int>(states[i], i));
        for (int j = 0; j < states.size(); j++) {
            graph[i][j] = 0;
        }
    }

    for (Transaction &trans: transactions) {
        graph[stateId[trans.firstState]][stateId[trans.secondState]] = 1;
    }

    dfs(stateId[initialState], visitedStates, graph, states.size());

    for (int i = 0; i < states.size(); i++) {
        if (!visitedStates[i]) {
            return false;
        }
    }
    return true;
}

/**
 * Checking deterministic of FSA
 * @param states is vector of states
 * @param alpha is vector of transactions' names
 * @param transactions is vector of transactions
 * @return deterministic or not
 */
bool checkDeterministic(vector<string> &states, vector<string> &alpha, vector<Transaction> &transactions) {
    int **graph = new int *[states.size()];
    for (int i = 0; i < states.size(); ++i)
        graph[i] = new int[alpha.size()];

    map<string, int> stateId;
    map<string, int> alphaId;

    for (int i = 0; i < alpha.size(); i++) {
        alphaId.insert(pair<string, int>(alpha[i], i));
    }

    for (int i = 0; i < states.size(); i++) {
        stateId.insert(pair<string, int>(states[i], i));
        for (int j = 0; j < alpha.size(); j++) {
            graph[i][j] = 0;
        }
    }

    for (Transaction &transaction: transactions) {
        if (graph[stateId[transaction.firstState]][alphaId[transaction.nameTransaction]])
            return false;
        graph[stateId[transaction.firstState]][alphaId[transaction.nameTransaction]] = 1;
    }

    return true;
}

/**
 * Checking completeness of graph
 * @param states is vector of states
 * @param alpha is vector of vectors' names
 * @param transactions is vector of names
 * @return Complete or not
 */
bool checkComplete(vector<string> &states, vector<string> &alpha, vector<Transaction> &transactions)
{
    int **g = new int *[states.size()];
    for (int i = 0; i < states.size(); ++i)
        g[i] = new int[alpha.size()];

    map<string, int> stateId;
    map<string, int> alphaId;

    for (int i = 0; i < alpha.size(); i++) {
        alphaId.insert(pair<string, int>(alpha[i], i));
    }

    for (int i = 0; i < states.size(); i++) {
        stateId.insert(pair<string, int>(states[i], i));
        for (int j = 0; j < alpha.size(); j++) {
            g[i][j] = 0;
        }
    }

    for (Transaction &transaction: transactions) {
        g[stateId[transaction.firstState]][alphaId[transaction.nameTransaction]] = 1;
    }

    for (int i = 0; i < states.size(); i++) {
        for (int j = 0; j < alpha.size(); j++) {
            if (g[i][j] != 1) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Checking transaction
 * @param transaction is name of transaction
 * @param states is vector of states
 * @param alpha is vector of transactions' names
 * @param output is output stream for error
 * @return Correct transaction or not
 */
bool checkTransaction(Transaction & transaction, vector<string> & states, vector<string> & alpha, ofstream & output)
{
    if (checkState(transaction.firstState, states, output) && checkState(transaction.secondState, states, output)) {
        bool exist = false;
        for (const string &alph: alpha) {
            if (transaction.nameTransaction.compare(alph) == 0) {
                exist = true;
            }
        }

        if (!exist) {
            output << "Error:\nE3: A transition \'" + transaction.nameTransaction +
                      "\' is not represented in the alphabet";
            return false;
        }
    } else {
        return false;
    }
    return true;
}

/**
 * Printing results of checking FSA
 * @param states is vector of states
 * @param alpha is vector of transactions' names
 * @param initState is initial state
 * @param finalStates is vector of final states
 * @param transactions is vector of transaction
 * @param output is output stream
 */
void printResult(vector<string> & states, vector<string> & alpha, vector<string> & initState, vector<string> & finalStates, vector<Transaction> & transactions, ofstream & output)
{
    bool w1 = finalStates.empty();
    bool w2 = !checkReachable(initState.back(), states, transactions);
    bool w3 = false


    if (checkComplete(states, alpha, transactions)) {
        output << "FSA is complete";
    } else {
        output << "FSA is incomplete";
    }
    if (w1 || w2 || w3) {
        output << "\nWarning:";
        if (w1) {
            output << "\nW1: Accepting state is not defined";
        }
        if (w2) {
            output << "\nW2: Some states are not reachable from initial state";
        }
        if (w3) {
            output << "\nW3: FSA is nondeterministic";
        }
    }
}

std::string KleeneAlgorithm()
{
    std::string reg = "";

    if(reg.empty())
    {
        reg = "{}";
    }

    return reg;
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
    vector<string> states = getValues(stateString, "states={", output);

    std::string alphaString;
    std::getline(input, alphaString);
    vector<string> alpha = getValues(alphaString, "alpha={", output);

    std::string initStateString;
    std::getline(input, initStateString);
    vector<string> initState = getValues(initStateString, "init.st={", output);

    if (!checkInitialState(initState, states, output)) {
        return 0;
    }

    std::string finalStateString;
    std::getline(input, finalStateString);
    vector<string> finalStates = getValues(finalStateString, "fin.st={", output);

    for (string &state: finalStates) {
        if (!checkState(state, states, output)) {
            return 0;
        }
    }

    std::string transString;
    std::getline(input, transString);
    vector<Transaction> transactions = getTransactions(transString, output);

    for (Transaction trans: transactions) {
        if(!checkTransaction(trans, states, alpha, output))
        {
            return 0;
        }
    }

    if (!checkJoint(states, transactions, output)) {
        return 0;
    }

    if(!checkDeterministic(states, alpha, transactions))
    {
        output << "Error:\nE6: FSA is nondeterministic";
        return 0;
    }

    //printResult(states, alpha, initState, finalStates, transactions, output);

    output << KleeneAlgorithm();

    output.close();
    return 0;
}