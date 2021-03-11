#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <map>
#include "map/map.hpp"
#include "priority_queue/priority_queue.hpp"
#include "deque/deque.hpp"
//#include "map/stl_rbtree.h"

#define TEST_SIZE 10000

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::stringstream;
using std::to_string;

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

void commandLineMap() {
    srand(1);
    sjtu::map<int, int> mp;
    string cmd;
    while (getline(cin, cmd)) {
        stringstream ss(cmd);
        string ty;
        ss >> ty;
        if (ty == "fill") {
            int va;
            ss >> va;
            for (int i = 0; i < va; i++) {
                mp[i] = i;
            }
            cout << "Fill successful." << endl;
        }
        else if (ty == "randomfill") {
            int va;
            ss >> va;
            for (int i = 0; i < va; i++) {
                int pos = rand() % va;
                mp[pos] = pos;
            }
            cout << "Fill successful." << endl;
        }
        else if (ty == "ins") {
            int va1, va2;
            ss >> va1 >> va2;
            cout << (mp[va1] = va2) << endl;
        }
        else if (ty == "vis") {
            int key;
            ss >> key;
            cout << mp[key] << endl;
        }
        else if (ty == "er") {
            int key;
            ss >> key;
            auto it = mp.find(key);
            mp.erase(it);
        }
        else if (ty == "erck") {
            int key;
            ss >> key;
            auto it = mp.find(key);
            mp.erase(it);
            if (mp.checkColoringProperties())cout << "[Correct]Your RedBlackTree has correct coloring properties." << endl;
            else {
                cout << "[Invalid]Your RedBlackTree has wrong coloring properties." << endl;
                mp.print();
            }
        }
        else if (ty == "it") {
            auto it = mp.begin();
            int va;
            ss >> va;
            while (va--)it++;
            cout << "key: " << it->first << " value: " << it->second << endl;
        }
        else if (ty == "sz") {
            cout << mp.size() << endl;
        }
        else if (ty == "pr") {
            mp.print();
        }
        else if (ty == "cl") {
            mp.clear();
            cout << "Clear successful." << endl;
        }
        else if (ty == "check") {
            if (mp.checkColoringProperties())cout << "[Correct]Your RedBlackTree has correct coloring properties." << endl;
            else {
                cout << "[Invalid]Your RedBlackTree has wrong coloring properties." << endl;
                mp.print();
            }
        }
        else if (ty == "ly") {
            cout << mp.layer() << endl;
        }
        else if (ty == "blly") {
            cout << mp.blackLayer() << endl;
        }
        else if (ty == "exit") {
            break;
        }
        else cout << "Invalid" << endl;
    }
}

void testMapInsert() {
    srand(172817);
    std::map<int, string> answer;
    sjtu::map<int, string> result;
    int k, v;
    for (int i = 0; i < TEST_SIZE; i++) {
        k = rand();
        v = rand();
        string s = to_string(v);
        answer[k] = s;
        result[k] = s;
//        result.print();
        if (i % (TEST_SIZE / 100) == 0)cout << "[count down] Inserting...\t" << i * 100 / TEST_SIZE + 1 << "%" << endl;
    }
//    k = rand();
//    v = rand();
//    string s = to_string(v);
//    answer[k] = s;
//    result[k] = s;
//    result.print();
    int cnt = 0;
    int size = answer.size();
    for (const auto &i:answer) {
        if (result[i.first] != i.second) {
            cout << "[Error]Wrong Answer." << endl;
            if (result.checkColoringProperties())cout << "[Info]Correct coloring properties." << endl;
            result.print();
            return;
        }
        if (cnt % (size / 100) == 0)cout << "[count down] Checking...\t" << cnt * 100 / size + 1 << "%" << endl;
        cnt++;
    }
    if (result.checkColoringProperties())cout << "[Info]Accepted." << endl;
    else {
        cout << "[Error]Wrong coloring properties." << endl;
        result.print();
    }
}

void testMapAll() {
    srand(172817);
    std::map<int, string> answer;
    sjtu::map<int, string> result;
    int k, v;
    for (int i = 0; i < TEST_SIZE; i++) {
        k = rand();
        v = rand();
        string s = to_string(v);
        answer[k] = s;
        result[k] = s;
        if (i % (TEST_SIZE / 100) == 0)cout << "[count down] Inserting...\t" << i * 100 / TEST_SIZE + 1 << "%" << endl;
    }
    for (int i = 0; i < TEST_SIZE; i += 2) {
        answer.erase(answer.begin());
        result.erase(result.begin());
        if (i % (TEST_SIZE / 100) == 0)cout << "[count down] Erasing...\t" << i * 100 / TEST_SIZE + 1 << "%" << endl;
    }
    int cnt = 0;
    int size = answer.size();
    for (const auto &i:answer) {
        if (result[i.first] != i.second) {
            cout << "[Error]Wrong Answer." << endl;
            if (result.checkColoringProperties())cout << "[Info]Correct coloring properties." << endl;
            result.print();
            return;
        }
        if (cnt % (size / 100) == 0)cout << "[count down] Checking...\t" << cnt * 100 / size + 1 << "%" << endl;
        cnt++;
    }
    if (result.checkColoringProperties())cout << "[Info]Accepted." << endl;
    else {
        cout << "[Error]Wrong coloring properties." << endl;
        result.print();
    }
}

int main() {
    cout << "[System]Programme initializing..." << endl;
    cout << "[Info]Welcome to RainyMemory's STLite project!" << endl;
//    commandLinePriorityQueue();
//    commandLineDeque();
//    commandLineMap();
//    testMapInsert();
    testMapAll();
    return 0;
}
