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
        
        enum positionType {
            NONE, OUTBOARD, INBOARD, BOTH//same, different
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
            
            Node *&selfFromParent() {
                if (isRoot())return parent->parent;
                else if (isLeft())return parent->left;
                else return parent->right;
            }
            
            Key getKey() const {
                return value->first;
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
                if (parent == nullptr)return true;//empty tree
                return parent->parent == this && color == RED;
            }
            
            bool isRoot() const {
                return parent->parent == this && color == BLACK;
            }
            
            int childNumber() const {
                int cnt = 0;
                if (left != nullptr)cnt++;
                if (right != nullptr)cnt++;
                return cnt;
            }
            
            int redChildNumber() const {
                int cnt = 0;
                if (left != nullptr && left->color == RED)cnt++;
                if (right != nullptr && right->color == RED)cnt++;
                return cnt;
            }
            
            positionType redChildPosition() {
                switch (redChildNumber()) {
                    case 0:
                        return NONE;
                    case 1:
                        if (isLeft()) {
                            if (left != nullptr && left->color == RED)return OUTBOARD;
                            else return INBOARD;
                        }
                        else {
                            if (right != nullptr && right->color == RED)return OUTBOARD;
                            else return INBOARD;
                        }
                    default:
                        return BOTH;
                }
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
            
            void swapColor(Node *other) {
                colorType t = other == nullptr ? BLACK : other->color;
                if (other != nullptr)other->color = color;
                color = t;
            }
            
            void swapPos(Node *other) {
                if (parent == other) {
                    swapColor(other);
                    Node *l = left, *r = right, *sib = getSibling();
                    other->selfFromParent() = this, parent = other->parent, other->parent = this;
                    if (l != nullptr)l->parent = other;
                    if (r != nullptr)r->parent = other;
                    if (sib != nullptr)sib->parent = this;
                    other->left = l, other->right = r;
                    if (isLeft())right = sib, left = other;
                    else left = sib, right = other;
                }
                else if (other->parent == this) {
                    other->swapPos(this);
                }
                else {
                    swapColor(other);
                    selfFromParent() = other;
                    other->selfFromParent() = this;
                    if (left != nullptr)left->parent = other;
                    if (right != nullptr)right->parent = other;
                    Node *l = left, *r = right, *p = parent;
                    left = other->left, right = other->right, parent = other->parent;
                    if (other->left != nullptr)other->left->parent = this;
                    if (other->right != nullptr)other->right->parent = this;
                    other->left = l, other->right = r, other->parent = p;
                }
            }
            
            Node *&getSibling() const {
                if (isLeft())return parent->right;
                else return parent->left;
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
//                    parent->color = BLACK;
//                    parent->right->color = RED;
                    parent->swapColor(parent->right);
                }
                else if (isRight() && parent->isRight()) {
                    parent->parent->rotateLeft();
//                    parent->color = BLACK;
//                    parent->left->color = RED;
                    parent->swapColor(parent->left);
                }
                else if (isLeft() && parent->isRight()) {
                    parent->rotateRight();
                    parent->rotateLeft();
//                    color = BLACK;
//                    left->color = RED;
                    swapColor(left);
                }
                else {
                    //isRight() && parent->isLeft()
                    parent->rotateLeft();
                    parent->rotateRight();
//                    color = BLACK;
//                    right->color = RED;
                    swapColor(right);
                }
            }

#ifdef debug
            
            void print() const {
                cout << (isRed() ? "R " : "B ") << getKey() << "  ";
            }

#endif
        };
        
        Node *header = nullptr; //header's parent is root, left is leftmost, right is rightmost
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
                if (equal(o, temp->getKey()))return temp;
                temp = less(o, temp->getKey()) ? temp->left : temp->right;
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
                now = less(value.first, now->getKey()) ? now->left : now->right;
            }
            return pa;
        }
        
        iterator RedBlackTreeInsert(Node *insertPos, const value_type &value) {
            nodeNumber++;
            if (insertPos == header) { //empty tree
                header->parent = new Node(value, BLACK);
                header->parent->parent = header;
                header->left = header->right = header->parent;
                return iterator(header->parent);
            }
            Node *insertNode = new Node(value, RED);
            if (less(value.first, insertPos->getKey())) {
                if (insertPos == header->left)header->left = insertNode;
                insertPos->left = insertNode;
            }
            else {
                if (insertPos == header->right)header->right = insertNode;
                insertPos->right = insertNode;
            }
            insertNode->parent = insertPos;
            if (insertNode->parent->color == RED)insertNode->rebalanceRedParentAndBlackUncle();
            return iterator(insertNode);
        }
        
        void rebalanceBlackSiblingWithNoRedChild(Node *now) {
            if (now->isRoot())return;
            if (now->parent->isRed()) {
                now->parent->swapColor(now->getSibling());
                return;
            }
            else if (now->getSibling() != nullptr && now->getSibling()->isRed()) {
                now->parent->swapColor(now->getSibling());
                if (now->isLeft())now->parent->rotateLeft();
                else now->parent->rotateRight();
                now->parent->swapColor(now->getSibling());
                return;
            }
            else {
                if (now->getSibling() != nullptr)now->getSibling()->color = RED;
                rebalanceBlackSiblingWithNoRedChild(now->parent);
            }
        }
        
        void RedBlackTreeErase(Node *now) {
            if (now->childNumber() == 2)now->swapPos(now->findSuccessor());
            int cn = now->childNumber();
            if (cn == 1) {
                if (now->left != nullptr) {
                    now->left->parent = now->parent;
                    now->left->color = BLACK;
                    now->selfFromParent() = now->left;
                }
                else {
                    now->right->parent = now->parent;
                    now->right->color = BLACK;
                    now->selfFromParent() = now->right;
                }
                delete now;
                return;
            }
            if (now->color == RED) {
                now->selfFromParent() = nullptr;
                delete now;
                return;
            }
            if (now->getSibling() != nullptr && now->getSibling()->isRed()) {
                now->parent->swapColor(now->getSibling());
                if (now->isLeft())now->parent->rotateLeft();
                else now->parent->rotateRight();
            }
            positionType pt;
            if (now->getSibling() == nullptr || (pt = now->getSibling()->redChildPosition()) == NONE) {
                rebalanceBlackSiblingWithNoRedChild(now);
            }
            else if (pt == OUTBOARD) {
                now->getSibling()->color = now->parent->color, now->parent->color = BLACK;
                if (now->isLeft())now->parent->rotateLeft();
                else now->parent->rotateRight();
            }
            else { //INBOARD & BOTH
                if (now->isLeft())now->getSibling()->rotateRight(), now->parent->rotateLeft();
                else now->getSibling()->rotateLeft(), now->parent->rotateRight();
                now->parent->parent->color = now->parent->color, now->parent->color = BLACK;
            }
            now->selfFromParent() = nullptr;
            delete now;
        }
    
    public:
        class iterator {
            friend class map;
        
        private:
            Node *node = nullptr;
            
            Key getKey() const {
                return node->getKey();
            }
            
            bool isInvalid() const {
                return node == nullptr;
            }
        
        public:
            iterator() = default;
            
            iterator(const iterator &o) : node(o.node) {}
            
            explicit iterator(Node *o) : node(o) {}
            
            iterator operator++(int) {
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                iterator temp(*this);
                node = node->findSuccessor();
                return temp;
            }
            
            iterator &operator++() {
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                node = node->findSuccessor();
                return *this;
            }
            
            iterator operator--(int) {
                iterator temp(*this);
                node = node->findPrecursor();//begin()'s precursor is header
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                return temp;
            }
            
            iterator &operator--() {
                node = node->findPrecursor();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
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
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                const_iterator temp(*this);
                node = node->findSuccessor();
                return temp;
            }
            
            const_iterator &operator++() {
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                node = node->findSuccessor();
                return *this;
            }
            
            const_iterator operator--(int) {
                const_iterator temp(*this);
                node = node->findPrecursor();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
                return temp;
            }
            
            const_iterator &operator--() {
                node = node->findPrecursor();
                if (node->isHeader()) {
                    node = nullptr;
                    throw invalid_iterator();
                }
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
            header->parent = nullptr;
            header->left = header;
            header->right = header;
        }
        
        map(const map &o) : nodeNumber(o.nodeNumber) {
            header = new Node;
            header->parent = recursionConstruct(header, o.header->parent);
            header->left = findNode(o.header->left->getKey());
            header->right = findNode(o.header->right->getKey());
        }
        
        map &operator=(const map &o) {
            if (this == &o)return *this;
            clear();
            nodeNumber = o.nodeNumber;
            header->parent = recursionConstruct(header, o.header->parent);
            header->left = findNode(o.header->left->getKey());
            header->right = findNode(o.header->right->getKey());
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
        
        T &operator[](const Key &key) {
            Node *pos = findNode(key);
            if (pos != nullptr)return pos->value->second;
            else {
                value_type value(key, T());
                iterator temp = RedBlackTreeInsert(findInsertPos(value), value);
                return temp.node->value->second;
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
        void erase(iterator pos) {
            if (pos == end() || pos.isInvalid() || find(pos.node->getKey()) != pos)throw runtime_error();
            nodeNumber--;
            if (nodeNumber == 0) {
                delete header->parent;
                header->parent = nullptr;
                header->left = header;
                header->right = header;
                return;
            }
            if (pos.node == header->left)header->left = header->left->findSuccessor();
            if (pos.node == header->right)header->right = header->right->findPrecursor();
            RedBlackTreeErase(pos.node);
        }
        
        size_t count(const Key &key) const {
            if (findNode(key) == nullptr)return 0;
            else return 1;
        }
        
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
            int sz = 2, la = layer();
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
                int t = (cnt++) + 2;
                if (t == (t & (-t)))cout << endl;
                if (t > sz)break;
                q.pop();
            }
            cout << endl;
        }

#endif
    };
    
}

#endif
