#include <iostream>
#include <string>
#include <sstream>
#include "deque/deque.hpp"

using namespace std;

void commandLineDeque() {
    sjtu::deque<int> dq;
    string cmd;
    while (getline(cin, cmd)) {
        stringstream ss(cmd);
        string ty;
        ss >> ty;
        if (ty == "fill") {
            int va;
            ss >> va;
            for (int i = 0; i < va; i++) {
                dq.push_back(i);
            }
            cout << "Fill successful." << endl;
        }
        else if (ty == "popb") {
            int cnt = 0;
            while (!dq.empty()) {
                dq.pop_back();
                cout << "Erase No." << ++cnt << " Element successful." << endl;
            }
        }
        else if (ty == "popf") {
            int cnt = 0;
            while (!dq.empty()) {
                dq.pop_front();
                cout << "Erase No." << ++cnt << " Element successful." << endl;
            }
        }
        else if (ty == "pr") {
            dq.print();
        }
        else if (ty == "prst") {
            dq.printStructure();
        }
        else if (ty == "puf") {
            int va;
            ss >> va;
            dq.push_front(va);
        }
        else if (ty == "pub") {
            int va;
            ss >> va;
            dq.push_back(va);
        }
        else if (ty == "pof") {
            dq.pop_front();
        }
        else if (ty == "pob") {
            dq.pop_back();
        }
        else if (ty == "ins") {
            int pos, va;
            ss >> pos >> va;
            auto iter = dq.begin();
            iter += pos;
            auto tp = dq.insert(iter, va);
            cout << *tp << endl;
        }
        else if (ty == "er") {
            int pos;
            ss >> pos;
            auto iter = dq.begin();
            iter += pos;
            auto tp = dq.erase(iter);
            cout << *tp << endl;
        }
        else if (ty == "sz") {
            cout << dq.size() << endl;
        }
        else if (ty == "bn") {
            cout << dq.blockNumber() << endl;
        }
        else if (ty == "cl") {
            dq.clear();
            cout << "Clear successful." << endl;
        }
        else if (ty == "exit") {
            break;
        }
        else cout << "Invalid" << endl;
    }
}

int main() {
    cout << "[System]Programme initializing..." << endl;
    cout << "[Info]Welcome to RainyMemory's STLite project!" << endl;
    commandLineDeque();
    return 0;
}
