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
        public:
            colorType color = RED;
            value_type *value = nullptr;
            Node *parent = nullptr;
            Node *left = nullptr;
            Node *right = nullptr;
            
            Node() = default;
            
            explicit Node(const value_type &o, colorType c = RED) : value(new value_type(o)), color(c) {}
            
            explicit Node(value_type *o, colorType c = RED) : value(o), color(c) {}
            
            Node(const Node &o) : value(new value_type(*o.value)), color(o.color) {}
            
            Node &operator=(const Node &o) = delete;
            
            ~Node() {
                delete value;
            }
            
            bool isLeft() const {
                if (parent->left == this)return true;
                else return false;
            }
            
            bool isHeader() const {
                return parent->parent == this && color == RED;
            }
            
            void rotateLeft() {
                if (isLeft()) {
                    parent->left = right;
                    right = right->left;
                    right->parent = this;
                    parent->left->parent = parent;
                    parent->left->left = this;
                    parent = parent->left;
                }
                else {
                    parent->right = right;
                    right = right->left;
                    right->parent = this;
                    parent->right->parent = parent;
                    parent->right->left = this;
                    parent = parent->right;
                }
            }
            
            void rotateRight() {
                if (isLeft()) {
                    parent->left = left;
                    left = left->right;
                    left->parent = this;
                    parent->left->parent = parent;
                    parent->left->right = this;
                    parent = parent->left;
                }
                else {
                    parent->right = left;
                    left = left->right;
                    left->parent = this;
                    parent->right->parent = parent;
                    parent->right->right = this;
                    parent = parent->right;
                }
            }
            
            Node *findMin() const {
                Node *node = this;
                while (node->left != nullptr)node = node->left;
                return node;
            }
            
            Node *findMax() const {
                Node *node = this;
                while (node->right != nullptr)node = node->right;
                return node;
            }
            
            Node *findPrecursor() const {
                Node *node = this;
                if (node->isHeader())node = node->right;
                else if (node->left != nullptr)node = node->left->findMax();
                else {
                    while (node->parent->left == node)node = node->parent;
                    node = node->parent;
                }
                return node;
            }
            
            Node *findSucceed() const {
                Node *node = this;
                if (node->right != nullptr)node = node->right->findMin();
                else {
                    while (node->parent->right == node)node = node->parent;
                    if (node->right != node->parent)node = node->parent;
                }
                return node;
            }
        };
        
        Node *header = nullptr;//header's parent is root, left is leftmost, right is rightmost
        int nodeNumber = 0;
        Compare compare;
        
        bool less(const Key &a, const Key &b) const {
            return compare(a, b);
        }
        
        bool equal(const Key &a, const Key &b) const {
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
        
        Node *insert(Node *o) {
        
        }
    
    public:
        class iterator {
        private:
            Node *node = nullptr;
        
        public:
            iterator() = default;
            
            iterator(const iterator &o) : node(o.node) {}
            
            explicit iterator(Node *o) : node(o) {}
            
            iterator operator++(int) {
                if (node->isHeader())throw invalid_iterator();
                iterator temp(*this);
                node = node->findSucceed();
                return temp;
            }
            
            iterator &operator++() {
                if (node->isHeader())throw invalid_iterator();
                node = node->findSucceed();
                return *this;
            }
            
            iterator operator--(int) {
                iterator temp(*this);
                node = node->findPrecursor();
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
                return !(node == o.node);
            }
            
            bool operator!=(const const_iterator &o) const {
                return !(node == o.node);
            }
            
            value_type *operator->() const noexcept {
                if(node->isHeader())throw invalid_iterator();
                return node->value;
            }
        };
        
        class const_iterator {
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
                node = node->findSucceed();
                return temp;
            }
            
            const_iterator &operator++() {
                if (node->isHeader())throw invalid_iterator();
                node = node->findSucceed();
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
                return !(node == o.node);
            }
            
            bool operator!=(const const_iterator &o) const {
                return !(node == o.node);
            }
            
            value_type *operator->() const noexcept {
                if(node->isHeader())throw invalid_iterator();
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
            Node *temp = findNode(key);
            if (temp != nullptr)return temp->value->second;
            else {
                //todo insert
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
        
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            if (findNode(value.first) != nullptr)return pair<iterator, bool>(iterator(nullptr), false);
            
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
    };
    
}

#endif
