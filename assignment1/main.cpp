#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

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
    }

    output.close();
    return 0;
}