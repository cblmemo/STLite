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
        enum sizeInfo {
            BLOCK_SIZE = 320,
            SPLIT_REMAIN = 160,
            MERGE_THRESHOLD = 288
        };
//
//        enum sizeInfo {
//            BLOCK_SIZE = 4,
//            SPLIT_REMAIN = 2,
//            MERGE_THRESHOLD = 3
//        };
        
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
            
            void clearElement() {
                Element *temp = elementHead;
                while (elementHead != nullptr) {
                    elementHead = elementHead->nextElement;
                    delete temp;
                    temp = elementHead;
                }
            }
            
            Block *splitBlock() {
                //add a new block after *this
                Block *newBlock = new Block(elementNum - SPLIT_REMAIN, this, nextBlock, nullptr, nullptr);
                elementNum = SPLIT_REMAIN;
                if (nextBlock != nullptr)nextBlock->preBlock = newBlock;
                nextBlock = newBlock;
                Element *temp = findElement(SPLIT_REMAIN);
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
            
            bool insertElement(deque<T> *dq, int index, Element *o) {
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
                if (elementNum == BLOCK_SIZE) {
                    Block *newBlock = splitBlock();
                    if (this == dq->blockTail)dq->blockTail = newBlock;
                    dq->blockNum++;
                    return true;
                }
                else return false;
            }
            
            bool eraseElement(deque<T> *dq, int index) {
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
                if (nextBlock != nullptr && elementNum + nextBlock->elementNum < MERGE_THRESHOLD) {
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
            blockNum = 1;
            len = 0;
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
            
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                if (nowIndex + n > ptr->len || nowIndex + n < 0)throw invalid_iterator();
                return iterator(ptr, nowIndex + n);
            }
            
            iterator operator-(const int &n) const {
                if (nowIndex - n > ptr->len || nowIndex - n < 0)throw invalid_iterator();
                return iterator(ptr, nowIndex - n);
            }
            
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
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
            
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr && nowBlock == rhs.nowBlock && nowElement == rhs.nowElement && nowIndex == rhs.nowIndex;
            }
            
            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr && nowBlock == rhs.nowBlock && nowElement == rhs.nowElement && nowIndex == rhs.nowIndex;
            }
            
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !((*this) == rhs);
            }
            
            bool operator!=(const const_iterator &rhs) const {
                return !((*this) == rhs);
            }
        };
        
        class const_iterator {
            friend class deque;
            
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
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
        
        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
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
        
        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T &front() const {
            if (len == 0)throw container_is_empty();
            return *(blockHead->elementHead->value);
        }
        
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T &back() const {
            if (len == 0)throw container_is_empty();
            return *(blockTail->elementTail->value);
        }
        
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            if (blockNum == 0)initialize();
            if (len == 0)return end();
            return iterator(this, blockHead, blockHead->elementHead, 0);
        }
        
        const_iterator cbegin() const {
            if (len == 0)return cend();
            return const_iterator(this, blockHead, blockHead->elementHead, 0);
        }
        
        /**
         * returns an iterator to the end.
         */
        iterator end() {
            if (blockNum == 0)initialize();
            return iterator(this, nullptr, nullptr, len);
        }
        
        const_iterator cend() const {
            return const_iterator(this, nullptr, nullptr, len);
        }
        
        /**
         * checks whether the container is empty.
         */
        bool empty() const {
            return len == 0;
        }
        
        /**
         * returns the number of elements
         */
        size_t size() const {
            return len;
        }
        
        /**
         * clears the contents
         */
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
        
        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            if (pos.invalid)throw invalid_iterator();
            if (blockNum == 0)initialize();
            if (pos.nowIndex < 0 || pos.nowIndex > len)throw invalid_iterator();
            if (pos.ptr != this)throw invalid_iterator();
            if (pos.nowIndex == len) {
                push_back(value);
                return iterator(this, blockTail, blockTail->elementTail, len - 1);
            }
            else {
                int index;
                Block *now = findBlock(pos.nowIndex, index);
                Element *temp = new Element(value);
                if (now->insertElement(this, index, temp))now = findBlock(pos.nowIndex, index);
                return iterator(this, pos.nowIndex);
            }
        }
        
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            if (blockNum == 0)initialize();
            if (len == 0)throw container_is_empty();
            if (pos.invalid)throw invalid_iterator();
            if (pos.nowIndex < 0 || pos.nowIndex >= len)throw invalid_iterator();
            if (pos.ptr != this)throw invalid_iterator();
            bool flag = pos.nowIndex == len - 1;
            int index;
            Block *now = findBlock(pos.nowIndex, index);
            if (now->eraseElement(this, index))now = findBlock(pos.nowIndex, index);
            Element *temp = now->findElement(index);
//            if(temp== nullptr)throw invalid_iterator();
            if (flag)return iterator(this, nullptr, nullptr, len);
            else return iterator(this, pos.nowIndex);
        }
        
        /**
         * adds an element to the end
         */
        void push_back(const T &value) {
            if (blockNum == 0)initialize();
            blockTail->insertElement(this, blockTail->elementNum, new Element(value));
        }
        
        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
            if (blockNum == 0)initialize();
            if (len == 0)throw container_is_empty();
            blockTail->eraseElement(this, -1);
        }
        
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            if (blockNum == 0)initialize();
            blockHead->insertElement(this, 0, new Element(value));
        }
        
        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {
            if (blockNum == 0)initialize();
            if (len == 0)throw container_is_empty();
            blockHead->eraseElement(this, 0);
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
