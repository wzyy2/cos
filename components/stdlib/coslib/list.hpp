/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef LIST_HPP
#define LIST_HPP

#ifndef NULL
#define NULL 0
#endif

namespace coslib{

    template<class T>
    /* definition of the linked list class */
    class List
    {
    public:
        class Node
        {
            friend class List<T>;
        private:
            T _value; /* data, can be any data type, but use integer for easiness */
            Node *_pNext; /* pointer to the next Node */

        public:
            /* Constructors with No Arguments */
            Node(void)
                : _pNext(NULL)
            { }

            /* Constructors with a given value */
            Node(T val)
                : _value(val), _pNext(NULL)
            { }

            /* Constructors with a given value and a link of the next Node */
            Node(T val, Node* next)
                : _value(val), _pNext(next)
            {}

            /* Getters */
            T getValue(void)
            { return _value; }

            Node* getNext(void)
            { return _pNext; }


        };
    private:
        /* pointer of head Node */
        Node *_pHead;
        /* pointer of tail Node */
        Node *_pTail;

    public:
        /* Constructors with No Arguments */
        List(void);
        /* Destructor */
        ~List(void);

        /* Function to get the Node in the specific position */
        Node *getNode(int pos);

        int size(){
            int i = 0;
            Node *p = _pHead;
            while(p != NULL){
                p = p->_pNext;
                i++;
            }
            return i;
        }

        bool empty(){
            if(_pHead == NULL)
                return true;
            else
                return false;
        }

        T front(){
            if(_pHead != NULL)
                return _pHead->_value;
            else
                return NULL;
        }

        Node *begin(){
            return _pHead;
        }

        void push_back(Node *p);
        void erase(Node *p);
    };

    template<class T> List<T>::List()
    {
        /* Initialize the head and tail Node */
        _pHead = _pTail = NULL;
    }

    template<class T> List<T>::~List()
    {
        /*
         * Leave it empty temporarily.
         * It will be described in detail in the example "How to delete a linkedlist".
         */
    }

    template<typename T>
    typename List<T>::Node *List<T>::getNode(int pos)
    {
        typename List<T>::Node  *p = _pHead;
        /* Counter */
        while (pos--) {
            if (p != NULL)
                p = p->_pNext;
            else
                return NULL;
        }
        return p;
    }

    template<class T> void List<T>::push_back(List<T>::Node *p)
    {
        /* The list is empty? */
        if (_pHead == NULL) {
            /* the same to create a new list with a given value */
            _pTail = _pHead = p;
        }
        else
        {
            /* Append the new Node to the tail */
            _pTail->_pNext = p;
            /* Update the tail pointer */
            _pTail = _pTail->_pNext;

            p->_pNext = NULL;
        }
    }

    template<class T> void List<T>::erase(List<T>::Node *p)
    {
        List<T>::Node *tmp = _pHead;
        if(_pHead == p){
            _pHead = p->_pNext;
            return;
        }

        while(tmp != NULL){
            if(tmp->_pNext == p){
                tmp->_pNext = p->_pNext;
                if(_pTail == p){
                    _pTail = tmp;
                }
            }
            tmp = tmp->_pNext;
        }

        p->_pNext = NULL;

    }
}
#endif // LIST_HPP

