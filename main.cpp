#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "priority_queue//priority_queue.hpp"
#include "deque/deque.hpp"

using namespace std;

void commandLinePriorityQueue() {
    sjtu::priority_queue<int> pq;
    string cmd;
    while (getline(cin, cmd)) {
        stringstream ss(cmd);
        string ty;
        ss >> ty;
        if (ty == "fill") {
            int va;
            ss >> va;
            for (int i = 0; i < va; i++) {
                pq.push(i);
            }
            cout << "Fill successful." << endl;
        }
        else if (ty == "top") {
            cout << pq.top() << endl;
        }
        else if (ty == "pop") {
            pq.pop();
        }
        else if (ty == "popall") {
            int cnt = 0;
            while (!pq.empty()) {
                cout << "Erase No." << ++cnt << " Element " << pq.top();
                cout.flush();
                pq.pop();
                cout << " successful." << endl;
            }
        }
        else if (ty == "push") {
            int va;
            ss >> va;
            pq.push(va);
        }
        else if (ty == "sz") {
            cout << pq.size() << endl;
        }
        else cout << "Invalid" << endl;
    }
}

void commandLineDeque() {
    srand(1);
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
        else if (ty == "randomfill") {
            int va;
            ss >> va;
            for (int i = 0; i < va; i++) {
                int pos = (i == 0) ? 0 : (rand() % dq.size());
                sjtu::deque<int>::iterator it(&dq, pos);
                dq.insert(it, i);
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
        else if (ty == "randompop") {
            int cnt = 0;
            while (!dq.empty()) {
                int judge = rand() % 2;
                if (judge)dq.pop_front();
                else dq.pop_back();
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
            auto tp = dq.insert(dq.begin() + pos, va);
            cout << *tp << endl;
        }
        else if (ty == "insb") {
            int pos, va;
            ss >> pos >> va;
            auto tp = dq.insert(dq.end() - pos, va);
            cout << *tp << endl;
        }
        else if (ty == "er") {
            int pos;
            ss >> pos;
            auto tp = dq.erase(dq.begin() + pos);
            if (tp != dq.end())cout << *tp << endl;
            else cout << "it::end()" << endl;
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
//    commandLinePriorityQueue();
    commandLineDeque();
    return 0;
}
