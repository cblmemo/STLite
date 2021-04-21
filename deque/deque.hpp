#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

//this deque have passed point 1 & 2 & 3 & 4 & 5

#include <cstddef>

#define debug

#ifdef debug

#include <iostream>

using std::cout;
using std::endl;

#endif

namespace sjtu {
    template<class T>
    class deque {
    private:
        int BLOCK_SIZE = 160;
        int MERGE_THRESHOLD = 144;
        
        class Element {
        public:
            T *value;
            Element *preElement = nullptr;
            Element *nextElement = nullptr;
            
            Element() = delete;
            
            explicit Element(const T &_value, Element *_pre = nullptr, Element *_next = nullptr) : preElement(_pre), nextElement(_next) {
                value = new T(_value);
            }
            
            Element(const Element &o) {
                value = new T(*(o.value));
            }
            
            Element &operator=(const Element &o) = delete;
            
            ~Element() {
                if (value != nullptr)delete value;
            }
        };
        
        class Block {
        public:
            int elementNum = 0;
            Block *preBlock = nullptr;
            Block *nextBlock = nullptr;
            Element *elementHead = nullptr;
            Element *elementTail = nullptr;
            
            Block() = delete;
            
            Block(int _elementNum, Block *_pre, Block *_next, Element *_head, Element *_tail) : elementNum(_elementNum), preBlock(_pre), nextBlock(_next), elementHead(_head), elementTail(_tail) {}
            
            void pushBackElement(Element *o) {
                if (elementTail != nullptr) {
                    elementTail->nextElement = new Element(*o);
                    elementTail->nextElement->preElement = elementTail;
                    elementTail = elementTail->nextElement;
                }
                else {
                    elementHead = new Element(*o);
                    elementTail = elementHead;
                }
                elementNum++;
            }
            
            Block(const Block &o) {
                Element *cur = o.elementHead;
                while (cur != nullptr) {
                    pushBackElement(cur);
                    cur = cur->nextElement;
                }
            }
            
            Block &operator=(const Block &o) = delete;
            
            Block *splitBlock(int REMAIN) {
                //add a new block after *this
                Block *newBlock = new Block(elementNum - REMAIN, this, nextBlock, nullptr, nullptr);
                elementNum = REMAIN;
                if (nextBlock != nullptr)nextBlock->preBlock = newBlock;
                nextBlock = newBlock;
                Element *temp = findElement(REMAIN);
                newBlock->elementHead = temp;
                newBlock->elementTail = elementTail;
                elementTail = temp->preElement;
                newBlock->elementHead->preElement = nullptr;
                elementTail->nextElement = nullptr;
                return newBlock;
            }
            
            void mergeBlock() {
                //merge this and next
                if (elementNum == 0) {
                    elementHead = nextBlock->elementHead;
                    elementTail = nextBlock->elementTail;
                }
                else if (nextBlock->elementNum == 0) {
                    //do nothing...
                }
                else {
                    elementTail->nextElement = nextBlock->elementHead;
                    nextBlock->elementHead->preElement = elementTail;
                    elementTail = nextBlock->elementTail;
                }
                elementNum += nextBlock->elementNum;
                nextBlock->elementHead = nullptr;
                nextBlock->elementTail = nullptr;
                if (nextBlock->nextBlock != nullptr)nextBlock->nextBlock->preBlock = this;
                Block *temp = nextBlock;
                nextBlock = nextBlock->nextBlock;
                delete temp;
            }
            
            Element *findElement(int index) {
                Element *temp = elementHead;
                //let temp point at pos index
                for (int i = 0; i < index; i++)temp = temp->nextElement;
                return temp;
            }
            
            bool insertElement(deque<T> *dq, int index, Element *o, int SIZE) {
                if (elementNum == 0) {
                    elementHead = o;
                    elementTail = o;
                }
                else {
                    if (index == elementNum) {
                        elementTail->nextElement = o;
                        o->preElement = elementTail;
                        o->nextElement = nullptr;
                        elementTail = o;
                    }
                    else if (index == 0) {
                        elementHead->preElement = o;
                        o->nextElement = elementHead;
                        o->preElement = nullptr;
                        elementHead = o;
                    }
                    else {
                        Element *temp = findElement(index);
                        o->preElement = temp->preElement;
                        o->nextElement = temp;
                        temp->preElement->nextElement = o;
                        temp->preElement = o;
                    }
                }
                elementNum++;
                dq->len++;
                if (elementNum == SIZE) {
                    Block *newBlock = splitBlock(SIZE >> 1);
                    if (this == dq->blockTail)dq->blockTail = newBlock;
                    dq->blockNum++;
                    return true;
                }
                else return false;
            }
            
            bool eraseElement(deque<T> *dq, int index, int THRESHOLD) {
                if (elementNum == 0)throw container_is_empty();
                if (elementNum == 1) {
                    delete elementHead;
                    elementHead = nullptr;
                    elementTail = nullptr;
                }
                else {
                    if (index > 0 && index < elementNum - 1) {
                        Element *temp = findElement(index);
                        if (temp->preElement != nullptr)temp->preElement->nextElement = temp->nextElement;
                        if (temp->nextElement != nullptr)temp->nextElement->preElement = temp->preElement;
                        delete temp;
                    }
                    else if (index == 0) {
                        Element *temp = elementHead;
                        elementHead = elementHead->nextElement;
                        elementHead->preElement = nullptr;
                        delete temp;
                    }
                    else {
                        //negative or index == elementNum - 1 represent delete tail
                        Element *temp = elementTail;
                        elementTail = elementTail->preElement;
                        elementTail->nextElement = nullptr;
                        delete temp;
                    }
                }
                elementNum--;
                dq->len--;
                if (elementNum == 0) {
                    if (nextBlock != nullptr) {
                        if (this == dq->blockTail->preBlock)dq->blockTail = this;
                        mergeBlock();
                        dq->blockNum--;
                    }
                    else if (preBlock != nullptr) {
                        if (this == dq->blockTail)dq->blockTail = this->preBlock;
                        preBlock->mergeBlock();
                        dq->blockNum--;
                    }
                    return true;
                }
                if (nextBlock != nullptr && elementNum + nextBlock->elementNum < THRESHOLD) {
                    if (this == dq->blockTail->preBlock)dq->blockTail = this;
                    mergeBlock();
                    dq->blockNum--;
                    return true;
                }
                else return false;
            }
            
            ~Block() {
                Element *tempDelete;
                while (elementHead != nullptr) {
                    tempDelete = elementHead;
                    elementHead = elementHead->nextElement;
                    delete tempDelete;
                }
            }

#ifdef debug
            
            void print() const {
                cout << "[print block]" << endl;
                cout << "elementNum: " << elementNum << endl;
                Element *temp = elementHead;
                int cnt = 0;
                while (temp != nullptr) {
                    cout << "No. " << ++cnt << " Element: " << *(temp->value) << endl;
                    temp = temp->nextElement;
                }
            }

#endif
        };
        
        Block *blockHead = nullptr;//point to the first block
        Block *blockTail = nullptr;//point to the last block
        int len = 0;
        int blockNum = 0;
        
        Block *findBlock(const size_t &_pos, int &index) const {
            int pos = _pos;
            if (pos == 0) {
                index = 0;
                return blockHead;
            }
            else if (pos == len - 1) {
                index = blockTail->elementNum - 1;
                return blockTail;
            }
            Block *cur = blockHead;
            int cnt = pos;
            while (cur->elementNum <= cnt) {
                cnt -= cur->elementNum;
                if (cur->nextBlock == nullptr)break;
                cur = cur->nextBlock;
            }
            index = cnt;
            return cur;
        }
        
        void initialize() {
            blockHead = new Block(0, nullptr, nullptr, nullptr, nullptr);
            blockTail = blockHead;
            blockNum = 1, len = 0;
            BLOCK_SIZE = 160, MERGE_THRESHOLD = 144;
        }
        
        void pushBackBlock(Block *o) {
            if (blockTail != nullptr) {
                blockTail->nextBlock = new Block(*o);
                blockTail->nextBlock->preBlock = blockTail;
                blockTail = blockTail->nextBlock;
            }
            else {
                blockHead = new Block(*o);
                blockTail = blockHead;
            }
            blockNum++;
            len += o->elementNum;
        }
        
        inline void adjustSize() {
            if (blockNum == BLOCK_SIZE * 4) {
                BLOCK_SIZE <<= 1, MERGE_THRESHOLD = BLOCK_SIZE * 0.9;
                blockTail = blockTail->preBlock;
                Block *now = blockHead;
                while (now != nullptr) {
                    now->mergeBlock();
                    now = now->nextBlock;
                }
            }
            else if (BLOCK_SIZE > 160 && blockNum * 4 == BLOCK_SIZE) {
                BLOCK_SIZE >>= 1, MERGE_THRESHOLD = BLOCK_SIZE * 0.9;
                Block *now = blockHead;
                while (now != nullptr) {
                    now->splitBlock(now->elementNum >> 1);
                    now = now->nextBlock, now = now->nextBlock;
                }
                blockTail = blockTail->nextBlock;
            }
        }
    
    public:
        class const_iterator;
        
        class iterator {
            friend class deque;
        
        private:
            bool invalid = false;
            deque<T> *ptr = nullptr;
            Block *nowBlock = nullptr;
            Element *nowElement = nullptr;
            int nowIndex = -1;//index in the whole deque
        
        public:
            iterator() = default;
            
            iterator(deque<T> *_ptr, Block *_nowBlock, Element *_nowElement, int _nowIndex) : ptr(_ptr), nowBlock(_nowBlock), nowElement(_nowElement), nowIndex(_nowIndex) {}
            
            iterator(const iterator &o) : invalid(o.invalid), ptr(o.ptr), nowBlock(o.nowBlock), nowElement(o.nowElement), nowIndex(o.nowIndex) {}
            
            iterator(deque<T> *_ptr, int _nowIndex) : ptr(_ptr), nowIndex(_nowIndex) {
                if (nowIndex > ptr->len)throw invalid_iterator();
                else if (nowIndex == ptr->len) {
                    //end()
                }
                else {
                    if (ptr->blockNum == 0)ptr->initialize();
                    int index;
                    nowBlock = ptr->findBlock(nowIndex, index);
                    nowElement = nowBlock->findElement(index);
                }
            }
            
            explicit iterator(bool _invalid) : invalid(_invalid) {}
            
            iterator &operator=(const iterator &o) {
                if (this == &o)return *this;
                invalid = o.invalid;
                ptr = o.ptr;
                nowBlock = o.nowBlock;
                nowElement = o.nowElement;
                nowIndex = o.nowIndex;
                return *this;
            }
            
            iterator operator+(const int &n) const {
                if (nowIndex + n > ptr->len || nowIndex + n < 0)throw invalid_iterator();
                return iterator(ptr, nowIndex + n);
            }
            
            iterator operator-(const int &n) const {
                if (nowIndex - n > ptr->len || nowIndex - n < 0)throw invalid_iterator();
                return iterator(ptr, nowIndex - n);
            }
            
            int operator-(const iterator &rhs) const {
                if (ptr != rhs.ptr)throw invalid_iterator();
                return nowIndex - rhs.nowIndex;
            }
            
            iterator &operator+=(const int &n) {
                if (nowIndex + n > ptr->len || nowIndex + n < 0)throw invalid_iterator();
                iterator temp = (*this) + n;
                (*this) = temp;
                return *this;
            }
            
            iterator &operator-=(const int &n) {
                if (nowIndex - n > ptr->len || nowIndex - n < 0)throw invalid_iterator();
                iterator temp = (*this) - n;
                (*this) = temp;
                return *this;
            }
            
            iterator operator++(int) {
                if (nowIndex == ptr->len) {
                    invalid = true;
                    return *this;
                }
                iterator temp(*this);
                if (nowElement->nextElement == nullptr) {
                    nowBlock = nowBlock->nextBlock;
                    if (nowBlock != nullptr)nowElement = nowBlock->elementHead;
                    else nowElement = nullptr;
                }
                else nowElement = nowElement->nextElement;
                nowIndex++;
                return temp;
            }
            
            iterator &operator++() {
                if (nowIndex == ptr->len)throw invalid_iterator();
                if (nowElement->nextElement == nullptr) {
                    nowBlock = nowBlock->nextBlock;
                    if (nowBlock != nullptr)nowElement = nowBlock->elementHead;
                    else nowElement = nullptr;
                }
                else nowElement = nowElement->nextElement;
                nowIndex++;
                return *this;
            }
            
            iterator operator--(int) {
                if (nowIndex == 0)throw invalid_iterator();
                iterator temp(*this);
                if (nowBlock != nullptr) {
                    if (nowElement->preElement == nullptr) {
                        nowBlock = nowBlock->preBlock;
                        nowElement = nowBlock->elementTail;
                    }
                    else nowElement = nowElement->preElement;
                }
                else {
                    nowBlock = ptr->blockTail;
                    nowElement = nowBlock->elementTail;
                }
                nowIndex--;
                return temp;
            }
            
            iterator &operator--() {
                if (nowIndex == 0)throw invalid_iterator();
                if (nowBlock != nullptr) {
                    if (nowElement->preElement == nullptr) {
                        nowBlock = nowBlock->preBlock;
                        nowElement = nowBlock->elementTail;
                    }
                    else nowElement = nowElement->preElement;
                }
                else {
                    nowBlock = ptr->blockTail;
                    nowElement = nowBlock->elementTail;
                }
                nowIndex--;
                return *this;
            }
            
            T &operator*() const {
                if (invalid || nowIndex >= ptr->len || nowIndex < 0)throw invalid_iterator();
                return *(nowElement->value);
            }
            
            T *operator->() const {
                if (invalid || nowIndex >= ptr->len || nowIndex < 0)throw invalid_iterator();
                return nowElement->value;
            }
            
            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr && nowBlock == rhs.nowBlock && nowElement == rhs.nowElement && nowIndex == rhs.nowIndex;
            }
            
            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr && nowBlock == rhs.nowBlock && nowElement == rhs.nowElement && nowIndex == rhs.nowIndex;
            }
            
            bool operator!=(const iterator &rhs) const {
                return !((*this) == rhs);
            }
            
            bool operator!=(const const_iterator &rhs) const {
                return !((*this) == rhs);
            }
        };
        
        class const_iterator {
            friend class deque;
        
        private:
            bool invalid = false;
            const deque<T> *ptr = nullptr;
            Block *nowBlock = nullptr;
            Element *nowElement = nullptr;
            int nowIndex = -1;//index in the whole deque
        
        public:
            const_iterator() = default;
            
            const_iterator(const deque<T> *_ptr, Block *_nowBlock, Element *_nowElement, int _nowIndex) : ptr(_ptr), nowBlock(_nowBlock), nowElement(_nowElement), nowIndex(_nowIndex) {}
            
            const_iterator(const const_iterator &o) : invalid(o.invalid), ptr(o.ptr), nowBlock(o.nowBlock), nowElement(o.nowElement), nowIndex(o.nowIndex) {}
            
            const_iterator(const iterator &o) : invalid(o.invalid), ptr(o.ptr), nowBlock(o.nowBlock), nowElement(o.nowElement), nowIndex(o.nowIndex) {}
            
            const_iterator(const deque<T> *_ptr, int _nowIndex) : ptr(_ptr), nowIndex(_nowIndex) {
                if (nowIndex > ptr->len)throw invalid_iterator();
                else if (nowIndex == ptr->len) {
                    //end()
                }
                else {
                    if (ptr->blockNum == 0)throw invalid_iterator();
                    int index;
                    nowBlock = ptr->findBlock(nowIndex, index);
                    nowElement = nowBlock->findElement(index);
                }
            }
            
            explicit const_iterator(bool _invalid) : invalid(_invalid) {}
            
            const_iterator &operator=(const const_iterator &o) {
                if (this == &o)return *this;
                ptr = o.ptr;
                invalid = o.invalid;
                nowBlock = o.nowBlock;
                nowElement = o.nowElement;
                nowIndex = o.nowIndex;
                return *this;
            }
            
            const_iterator operator+(const int &n) const {
                if (nowIndex + n > ptr->len || nowIndex + n < 0)throw invalid_iterator();
                return const_iterator(ptr, nowIndex + n);
            }
            
            const_iterator operator-(const int &n) const {
                if (nowIndex - n > ptr->len || nowIndex - n < 0)throw invalid_iterator();
                return const_iterator(ptr, nowIndex - n);
            }
            
            int operator-(const const_iterator &rhs) const {
                if (ptr != rhs.ptr)throw invalid_iterator();
                return nowIndex - rhs.nowIndex;
            }
            
            const_iterator &operator+=(const int &n) {
                if (nowIndex + n > ptr->len || nowIndex + n < 0)throw invalid_iterator();
                const_iterator temp = (*this) + n;
                (*this) = temp;
                return *this;
            }
            
            const_iterator &operator-=(const int &n) {
                if (nowIndex - n > ptr->len || nowIndex - n < 0)throw invalid_iterator();
                const_iterator temp = (*this) - n;
                (*this) = temp;
                return *this;
            }
            
            const_iterator operator++(int) {
                if (nowIndex == ptr->len)throw invalid_iterator();
                const_iterator temp(*this);
                if (nowElement->nextElement == nullptr) {
                    nowBlock = nowBlock->nextBlock;
                    if (nowBlock != nullptr)nowElement = nowBlock->elementHead;
                    else nowElement = nullptr;
                }
                else nowElement = nowElement->nextElement;
                nowIndex++;
                return temp;
            }
            
            const_iterator &operator++() {
                if (nowIndex == ptr->len)throw invalid_iterator();
                if (nowElement->nextElement == nullptr) {
                    nowBlock = nowBlock->nextBlock;
                    if (nowBlock != nullptr)nowElement = nowBlock->elementHead;
                    else nowElement = nullptr;
                }
                else nowElement = nowElement->nextElement;
                nowIndex++;
                return *this;
            }
            
            const_iterator operator--(int) {
                if (nowIndex == 0)throw invalid_iterator();
                const_iterator temp(*this);
                if (nowBlock != nullptr) {
                    if (nowElement->preElement == nullptr) {
                        nowBlock = nowBlock->preBlock;
                        nowElement = nowBlock->elementTail;
                    }
                    else nowElement = nowElement->preElement;
                }
                else {
                    nowBlock = ptr->blockTail;
                    nowElement = nowBlock->elementTail;
                }
                nowIndex--;
                return temp;
            }
            
            const_iterator &operator--() {
                if (nowIndex == 0)throw invalid_iterator();
                if (nowBlock != nullptr) {
                    if (nowElement->preElement == nullptr) {
                        nowBlock = nowBlock->preBlock;
                        nowElement = nowBlock->elementTail;
                    }
                    else nowElement = nowElement->preElement;
                }
                else {
                    nowBlock = ptr->blockTail;
                    nowElement = nowBlock->elementTail;
                }
                nowIndex--;
                return *this;
            }
            
            const T &operator*() const {
                if (invalid || nowIndex >= ptr->len || nowIndex < 0)throw invalid_iterator();
                return *(nowElement->value);
            }
            
            T *operator->() const {
                if (invalid || nowIndex >= ptr->len || nowIndex < 0)throw invalid_iterator();
                return nowElement->value;
            }
            
            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr && nowBlock == rhs.nowBlock && nowElement == rhs.nowElement && nowIndex == rhs.nowIndex;
            }
            
            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr && nowBlock == rhs.nowBlock && nowElement == rhs.nowElement && nowIndex == rhs.nowIndex;
            }
            
            bool operator!=(const iterator &rhs) const {
                return !((*this) == rhs);
            }
            
            bool operator!=(const const_iterator &rhs) const {
                return !((*this) == rhs);
            }
        };
        
        deque() {
            initialize();
        }
        
        deque(const deque &o) {
            Block *cur = o.blockHead;
            while (cur != nullptr) {
                pushBackBlock(cur);
                cur = cur->nextBlock;
            }
        }
        
        ~deque() {
            Block *tempDelete;
            while (blockHead != nullptr) {
                tempDelete = blockHead;
                blockHead = blockHead->nextBlock;
                delete tempDelete;
            }
        }
        
        deque &operator=(const deque &o) {
            if (this == &o)return *this;
            Block *tempDelete;
            while (blockHead != nullptr) {
                tempDelete = blockHead;
                blockHead = blockHead->nextBlock;
                delete tempDelete;
            }
            blockTail = nullptr;
            len = 0;
            blockNum = 0;
            Block *cur = o.blockHead;
            while (cur != nullptr) {
                pushBackBlock(cur);
                cur = cur->nextBlock;
            }
            return *this;
        }
        
        T &at(const size_t &pos) {
            if (pos < 0 || pos >= len)throw index_out_of_bound();
            int index;
            Block *now = findBlock(pos, index);
            Element *target = now->findElement(index);
            return *(target->value);
        }
        
        const T &at(const size_t &pos) const {
            if (pos < 0 || pos >= len)throw index_out_of_bound();
            int index;
            Block *now = findBlock(pos, index);
            Element *target = now->findElement(index);
            return *(target->value);
        }
        
        T &operator[](const size_t &pos) {
            if (pos < 0 || pos >= len)throw index_out_of_bound();
            int index;
            Block *now = findBlock(pos, index);
            Element *target = now->findElement(index);
            return *(target->value);
        }
        
        const T &operator[](const size_t &pos) const {
            if (pos < 0 || pos >= len)throw index_out_of_bound();
            int index;
            Block *now = findBlock(pos, index);
            Element *target = now->findElement(index);
            return *(target->value);
        }
        
        const T &front() const {
            if (len == 0)throw container_is_empty();
            return *(blockHead->elementHead->value);
        }
        
        const T &back() const {
            if (len == 0)throw container_is_empty();
            return *(blockTail->elementTail->value);
        }
        
        iterator begin() {
            if (blockNum == 0)initialize();
            if (len == 0)return end();
            return iterator(this, blockHead, blockHead->elementHead, 0);
        }
        
        const_iterator cbegin() const {
            if (len == 0)return cend();
            return const_iterator(this, blockHead, blockHead->elementHead, 0);
        }
        
        iterator end() {
            if (blockNum == 0)initialize();
            return iterator(this, nullptr, nullptr, len);
        }
        
        const_iterator cend() const {
            return const_iterator(this, nullptr, nullptr, len);
        }
        
        bool empty() const {
            return len == 0;
        }
        
        size_t size() const {
            return len;
        }
        
        void clear() {
            Block *tempDelete;
            while (blockHead != nullptr) {
                tempDelete = blockHead;
                blockHead = blockHead->nextBlock;
                delete tempDelete;
            }
            blockTail = nullptr;
            initialize();
        }
        
        iterator insert(iterator pos, const T &value) {
            adjustSize();
            if (blockNum == 0)initialize();
            if (pos.invalid || pos.nowIndex < 0 || pos.nowIndex > len || pos.ptr != this)throw invalid_iterator();
            if (pos.nowIndex == len) {
                push_back(value);
                return iterator(this, blockTail, blockTail->elementTail, len - 1);
            }
            int index;
            Block *now = findBlock(pos.nowIndex, index);
            Element *temp = new Element(value);
            now->insertElement(this, index, temp, BLOCK_SIZE);
            return iterator(this, pos.nowIndex);
        }
        
        iterator erase(iterator pos) {
            adjustSize();
            if (blockNum == 0)initialize();
            if (len == 0)throw container_is_empty();
            if (pos.invalid || pos.nowIndex < 0 || pos.nowIndex >= len || pos.ptr != this)throw invalid_iterator();
            bool flag = pos.nowIndex == len - 1;
            int index;
            Block *now = findBlock(pos.nowIndex, index);
            now->eraseElement(this, index, MERGE_THRESHOLD);
            if (flag)return iterator(this, nullptr, nullptr, len);
            else return iterator(this, pos.nowIndex);
        }
        
        void push_back(const T &value) {
            adjustSize();
            if (blockNum == 0)initialize();
            blockTail->insertElement(this, blockTail->elementNum, new Element(value), BLOCK_SIZE);
        }
        
        void pop_back() {
            adjustSize();
            if (blockNum == 0)initialize();
            if (len == 0)throw container_is_empty();
            blockTail->eraseElement(this, -1, MERGE_THRESHOLD);
        }
        
        void push_front(const T &value) {
            adjustSize();
            if (blockNum == 0)initialize();
            blockHead->insertElement(this, 0, new Element(value), BLOCK_SIZE);
        }
        
        void pop_front() {
            adjustSize();
            if (blockNum == 0)initialize();
            if (len == 0)throw container_is_empty();
            blockHead->eraseElement(this, 0, MERGE_THRESHOLD);
        }

#ifdef debug
        
        void printStructure() const {
            cout << "[print deque]" << endl;
            cout << "len: " << len << endl;
            cout << "blockNum: " << blockNum << endl;
            Block *temp = blockHead;
            int cnt = 0;
            while (temp != nullptr) {
                cout << "No. " << ++cnt << " Block:" << endl;
                cout << "--------------------" << endl;
                temp->print();
                cout << "--------------------" << endl;
                temp = temp->nextBlock;
            }
        }
        
        void print() {
            for (auto i:(*this)) {
                cout << i << " ";
            }
            cout << endl;
        }
        
        int blockNumber() const {
            return blockNum;
        }

#endif
    };
    
}

#endif