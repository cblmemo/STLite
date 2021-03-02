/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

#define debug

#ifdef debug

#include <iostream>
#include <queue>

using std::queue;
using std::cout;
using std::endl;

#endif

namespace sjtu {
    
    template<class Key, class T, class Compare = std::less<Key>>
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
        
        class iterator;
        
        class const_iterator;
    
    private:
        enum colorType {
            RED, BLACK
        };
        
        class Node {
            friend class map;
        
        public:
            colorType color = RED;
            value_type *value = nullptr;
            Node *parent = nullptr;
            Node *left = nullptr;
            Node *right = nullptr;
            
            Node() = default;
            
            explicit Node(const value_type &o, colorType c) : value(new value_type(o)), color(c) {}
            
            explicit Node(value_type *o, colorType c) : value(o), color(c) {}
            
            Node(const Node &o) : value(new value_type(*o.value)), color(o.color) {}
            
            Node &operator=(const Node &o) = delete;
            
            ~Node() {
                delete value;
            }
            
            bool isRed() const {
                return color == RED;
            }
            
            bool isBlack() const {
                return color == BLACK;
            }
            
            bool isLeft() const {
                if (parent->left == this)return true;
                else return false;
            }
            
            bool isRight() const {
                if (parent->right == this)return true;
                else return false;
            }
            
            bool isHeader() const {
                return parent->parent == this && color == RED;
            }
            
            bool isRoot() const {
                return parent->parent == this && color == BLACK;
            }
            
            void rotateLeft() {
                if (isRoot()) {
                    parent->parent = right;
                    right = right->left;
                    if (right != nullptr)right->parent = this;
                    parent->parent->parent = parent;
                    parent->parent->left = this;
                    parent = parent->parent;
                }
                else if (isLeft()) {
                    parent->left = right;
                    right = right->left;
                    if (right != nullptr)right->parent = this;
                    parent->left->parent = parent;
                    parent->left->left = this;
                    parent = parent->left;
                }
                else {
                    parent->right = right;
                    right = right->left;
                    if (right != nullptr)right->parent = this;
                    parent->right->parent = parent;
                    parent->right->left = this;
                    parent = parent->right;
                }
            }
            
            void rotateRight() {
                if (isRoot()) {
                    parent->parent = left;
                    left = left->right;
                    if (left != nullptr)left->parent = this;
                    parent->parent->parent = parent;
                    parent->parent->right = this;
                    parent = parent->parent;
                }
                else if (isLeft()) {
                    parent->left = left;
                    left = left->right;
                    if (left != nullptr)left->parent = this;
                    parent->left->parent = parent;
                    parent->left->right = this;
                    parent = parent->left;
                }
                else {
                    parent->right = left;
                    left = left->right;
                    if (left != nullptr)left->parent = this;
                    parent->right->parent = parent;
                    parent->right->right = this;
                    parent = parent->right;
                }
            }
            
            Node *findMin() const {
                Node *node = const_cast<Node *>(this);
                while (node->left != nullptr)node = node->left;
                return node;
            }
            
            Node *findMax() const {
                Node *node = const_cast<Node *>(this);
                while (node->right != nullptr)node = node->right;
                return node;
            }
            
            Node *findPrecursor() const {
                Node *node = const_cast<Node *>(this);
                if (node->isHeader())node = node->right;
                else if (node->left != nullptr)node = node->left->findMax();
                else {
                    while (node->parent->left == node)node = node->parent;
                    node = node->parent;
                }
                return node;
            }
            
            Node *findSuccessor() const {
                Node *node = const_cast<Node *>(this);
                if (node->right != nullptr)node = node->right->findMin();
                else {
                    while (node->parent->right == node)node = node->parent;
                    if (node->right != node->parent)node = node->parent;
                }
                return node;
            }
            
            void rebalanceRedParentAndBlackUncle() {
                if (isLeft() && parent->isLeft()) {
                    parent->parent->rotateRight();
                    parent->color = BLACK;
                    parent->right->color = RED;
                }
                else if (isRight() && parent->isRight()) {
                    parent->parent->rotateLeft();
                    parent->color = BLACK;
                    parent->left->color = RED;
                }
                else if (isLeft() && parent->isRight()) {
                    parent->rotateRight();
                    color = BLACK;
                    parent->color = RED;
                    parent->rotateLeft();
                }
                else {
                    //isRight() && parent->isLeft()
                    parent->rotateLeft();
                    color = BLACK;
                    parent->color = RED;
                    parent->rotateRight();
                }
            }

#ifdef debug
            
            void print() const {
                cout << (isRed() ? "R " : "B ") << value->first << "  ";
            }

#endif
        };
        
        Node *header = nullptr;//header's parent is root, left is leftmost, right is rightmost
        int nodeNumber = 0;
        Compare compare;
        
        inline bool less(const Key &a, const Key &b) const {
            return compare(a, b);
        }
        
        inline bool equal(const Key &a, const Key &b) const {
            return !(less(a, b) || less(b, a));
        }
        
        void recursionClear(Node *now) {
            if (now == nullptr)return;
            recursionClear(now->left);
            recursionClear(now->right);
            delete now;
        }
        
        Node *recursionConstruct(Node *pa, Node *now) {
            if (now == nullptr)return nullptr;
            Node *temp = new Node(*now);
            temp->parent = pa;
            temp->left = recursionConstruct(temp, now->left);
            temp->right = recursionConstruct(temp, now->right);
            return temp;
        }
        
        Node *findNode(const Key &o) const {
            Node *temp = header->parent;
            while (temp != nullptr) {
                if (equal(o, temp->value->first))return temp;
                temp = less(o, temp->value->first) ? temp->left : temp->right;
            }
            return nullptr;
        }
        
        Node *findInsertPos(const value_type &value) {
            Node *pa = header;
            Node *now = header->parent;
            while (now != nullptr) {
                //adjust two red child case
                if (now->left != nullptr && now->right != nullptr && now->left->isRed() && now->right->isRed()) {
                    now->left->color = now->right->color = BLACK;
                    if (!now->isRoot()) {
                        now->color = RED;
                        if (now->parent->isRed())now->rebalanceRedParentAndBlackUncle();
                    }
                }
                pa = now;
                now = less(value.first, now->value->first) ? now->left : now->right;
            }
            return pa;
        }
        
        iterator RedBlackTreeInsert(Node *insertPos, const value_type &value) {
            nodeNumber++;
            if (insertPos == header) {//empty tree
                header->parent = new Node(value, BLACK);
                header->parent->parent = header;
                header->left = header->right = header->parent;
                return iterator(header->parent);
            }
            Node *insertNode = new Node(value, RED);
            if (less(value.first, insertPos->value->first)) {
                if (insertPos == header->left)header->left = insertNode;
                insertPos->left = insertNode;
            }
            else {
                if (insertPos == header->right)header->right = insertNode;
                insertPos->right = insertNode;
            }
            insertNode->parent = insertPos;
            insertRebalance(insertNode);
            return iterator(insertNode);
        }
        
        void insertRebalance(Node *now) {
            if (now->parent->color == BLACK)return;
            now->rebalanceRedParentAndBlackUncle();
        }
    
    public:
        class iterator {
            friend class map;
        
        private:
            Node *node = nullptr;
        
        public:
            iterator() = default;
            
            iterator(const iterator &o) : node(o.node) {}
            
            explicit iterator(Node *o) : node(o) {}
            
            iterator operator++(int) {
                if (node->isHeader())throw invalid_iterator();
                iterator temp(*this);
                node = node->findSuccessor();
                return temp;
            }
            
            iterator &operator++() {
                if (node->isHeader())throw invalid_iterator();
                node = node->findSuccessor();
                return *this;
            }
            
            iterator operator--(int) {
                iterator temp(*this);
                node = node->findPrecursor();//begin()'s precursor is header
                if (node->isHeader())throw invalid_iterator();
                return temp;
            }
            
            iterator &operator--() {
                node = node->findPrecursor();
                if (node->isHeader())throw invalid_iterator();
                return *this;
            }
            
            value_type &operator*() const {
                return *node->value;
            }
            
            bool operator==(const iterator &o) const {
                return node == o.node;
            }
            
            bool operator==(const const_iterator &o) const {
                return node == o.node;
            }
            
            bool operator!=(const iterator &o) const {
                return node != o.node;
            }
            
            bool operator!=(const const_iterator &o) const {
                return node != o.node;
            }
            
            value_type *operator->() const noexcept {
                if (node->isHeader())throw invalid_iterator();
                return node->value;
            }
        };
        
        class const_iterator {
            friend class map;
        
        private:
            const Node *node = nullptr;
        
        public:
            const_iterator() = default;
            
            const_iterator(const const_iterator &o) : node(o.node) {}
            
            const_iterator(const iterator &o) : node(o.node) {}
            
            explicit const_iterator(Node *o) : node(o) {}
            
            const_iterator operator++(int) {
                if (node->isHeader())throw invalid_iterator();
                const_iterator temp(*this);
                node = node->findSuccessor();
                return temp;
            }
            
            const_iterator &operator++() {
                if (node->isHeader())throw invalid_iterator();
                node = node->findSuccessor();
                return *this;
            }
            
            const_iterator operator--(int) {
                const_iterator temp(*this);
                node = node->findPrecursor();
                if (node->isHeader())throw invalid_iterator();
                return temp;
            }
            
            const_iterator &operator--() {
                node = node->findPrecursor();
                if (node->isHeader())throw invalid_iterator();
                return *this;
            }
            
            value_type &operator*() const {
                return *node->value;
            }
            
            bool operator==(const iterator &o) const {
                return node == o.node;
            }
            
            bool operator==(const const_iterator &o) const {
                return node == o.node;
            }
            
            bool operator!=(const iterator &o) const {
                return node != o.node;
            }
            
            bool operator!=(const const_iterator &o) const {
                return node != o.node;
            }
            
            value_type *operator->() const noexcept {
                if (node->isHeader())throw invalid_iterator();
                return node->value;
            }
        };
        
        map() {
            header = new Node;
            header->right = header;
            header->left = header;
            header->parent = nullptr;
        }
        
        map(const map &o) : nodeNumber(o.nodeNumber) {
            header = new Node;
            header->parent = recursionConstruct(header, o.header->parent);
            header->left = findNode(o.header->left->value->first);
            header->right = findNode(o.header->right->value->first);
        }
        
        map &operator=(const map &o) {
            if (this == &o)return *this;
            clear();
            nodeNumber = o.nodeNumber;
            header->parent = recursionConstruct(header, o.header->parent);
            header->left = findNode(o.header->left->value->first);
            header->right = findNode(o.header->right->value->first);
            return *this;
        }
        
        ~map() {
            clear();
            delete header;
        }
        
        T &at(const Key &key) {
            Node *temp = findNode(key);
            if (temp == nullptr)throw index_out_of_bound();
            return temp->value->second;
        }
        
        const T &at(const Key &key) const {
            Node *temp = findNode(key);
            if (temp == nullptr)throw index_out_of_bound();
            return temp->value->second;
        }
        
        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            Node *pos = findNode(key);
            if (pos != nullptr)return pos->value->second;
            else {
                pair<iterator, bool> temp = insert(value_type(key, T()));
                return temp.first.node->value->second;
            }
        }
        
        const T &operator[](const Key &key) const {
            return at(key);
        }
        
        iterator begin() {
            return iterator(header->left);
        }
        
        const_iterator cbegin() const {
            return const_iterator(header->left);
        }
        
        iterator end() {
            return iterator(header);
        }
        
        const_iterator cend() const {
            return const_iterator(header);
        }
        
        bool empty() const {
            return nodeNumber == 0;
        }
        
        size_t size() const {
            return nodeNumber;
        }
        
        void clear() {
            recursionClear(header->parent);
            header->left = header;
            header->right = header;
            header->parent = nullptr;
            nodeNumber = 0;
        }
        
        pair<iterator, bool> insert(const value_type &value) {
            if (findNode(value.first) != nullptr)return pair<iterator, bool>(iterator(nullptr), false);
            return pair<iterator, bool>(RedBlackTreeInsert(findInsertPos(value), value), true);
        }
        
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {}
        
        size_t count(const Key &key) const {
            if (findNode(key) == nullptr)return 0;
            else return 1;
        }
        
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            Node *temp = findNode(key);
            if (temp == nullptr)return end();
            else return iterator(temp);
        }
        
        const_iterator find(const Key &key) const {
            Node *temp = findNode(key);
            if (temp == nullptr)return cend();
            else return const_iterator(temp);
        }

#ifdef debug
        
        int blackLayer() const {
            Node *now = const_cast<Node *>(header->parent);
            int cnt = 0;
            while (now != nullptr) {
                if (now->color == BLACK)cnt++;
                now = now->left;
            }
            return cnt;
        }
        
        bool checkColoringProperties() const {
            return recursionCheck(header->parent, 0, blackLayer());
        }
        
        bool recursionCheck(Node *now, int nowBlackLayer, int totalBlackLayer) const {
            if (now == nullptr) {
                if (nowBlackLayer == totalBlackLayer)return true;
                else return false;
            }
            if (now->isRed()) {
                if (now->parent->isRed())return false;
            }
            else nowBlackLayer++;
            return recursionCheck(now->left, nowBlackLayer, totalBlackLayer) && recursionCheck(now->right, nowBlackLayer, totalBlackLayer);
        }
        
        int layer() const {
            return recursionLayer(1, const_cast<Node *>(header->parent));
        }
        
        int recursionLayer(int nowLayer, Node *nowNode) const {
            if (nowNode == nullptr)return nowLayer;
            int a = recursionLayer(nowLayer + 1, nowNode->left);
            int b = recursionLayer(nowLayer + 1, nowNode->right);
            return a > b ? a : b;
        }
        
        void print() const {
            int sz = 1, la = layer();
            for (int i = 1; i < la; i++)sz *= 2;
            sz -= 3;
            queue<Node *> q;
            q.push(header->parent);
            int cnt = 0;
            while (!q.empty()) {
                if (q.front() != nullptr) {
                    q.push(q.front()->left);
                    q.push(q.front()->right);
                    q.front()->print();
                }
                else {
                    q.push(nullptr);
                    q.push(nullptr);
                    cout << "\\0   ";
                }
                int t = cnt++;
                if (t == 0 || t == 2 || t == 6 || t == 14 || t == 30 || t == 62 || t == 126 || t == 254 || t == 510 || t == 1022 || t == 2046 || t == 4094 || t == 8190)cout << endl;
                if (t > sz)break;
                q.pop();
            }
            cout << endl;
        }

#endif
    };
    
}

#endif
