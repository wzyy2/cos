///*
// * Copyright (C) 2015 - 2021  by  Jacob Chen
// *
// * This program is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License version 2 as
// * published by the Free Software Foundation.
// *
// */

//#ifndef LIST_HPP
//#define LIST_HPP
//namespace coslib{

//    /* definition of the list node class */
//    template<class T>
//    class Node
//    {
//        friend class List;
//    private:
//        int _value; /* data, can be any data type, but use integer for easiness */
//        Node *_pNext; /* pointer to the next node */

//    public:
//        /* Constructors with No Arguments */
//        Node(void)
//            : _pNext(NULL)
//        { }

//        /* Constructors with a given value */
//        Node(T val)
//            : _value(val), _pNext(NULL)
//        { }

//        /* Constructors with a given value and a link of the next node */
//        Node(T val, Node* next)
//            : _value(val), _pNext(next)
//        {}

//        /* Getters */
//        T getValue(void)
//        { return _value; }

//        Node* getNext(void)
//        { return _pNext; }
//    };

//    template<class T>
//    /* definition of the linked list class */
//    class List
//    {
//    private:
//        /* pointer of head node */
//        Node<T> *_pHead;
//        /* pointer of tail node */
//        Node<T> *_pTail;

//    public:
//        /* Constructors with No Arguments */
//        List(void);
//        /* Destructor */
//        ~List(void);

//        /* Function to add a node at the head of a linked list */
//        void push_front(T);
//        /* Function to append a node to the end of a linked list */
//        void push_back(T);

//        /* Function to get the node in the specific position */
//        Node<T> *getNode(int pos);

//    };

//    template<class T> List<T>::List()
//    {
//        /* Initialize the head and tail node */
//        _pHead = _pTail = NULL;
//    }

//    template<class T> List<T>::~List()
//    {
//        /*
//         * Leave it empty temporarily.
//         * It will be described in detail in the example "How to delete a linkedlist".
//         */
//    }

//    template<class T> void List<T>::push_front(T val)
//    {
//        if(afterMe != NULL)
//        {
//            /* The image of inserting a node like the figures below
//             before inserting:
//             +---------+    --->---------+    --->+---------+
//             | afterMe |    |   | Node(a) |    |   |  others |
//             +---------+    |   +---------+    |   +---------+
//             |  next   |-----   |  next   |-----   |  NULL   |
//             +---------+        +---------+        +---------+
//             after inserting:
//             (1)
//             +---------+    --->+---------+    --->+---------+    --->+---------+
//             | afterMe | (3)|   | New Node| (2)|   | Node(a) |    |   |  others |
//             +---------+    |   +---------+    |   +---------+    |   +---------+
//             |  next   |-----   |  next   |-----   |  next   |-----   |  NULL   |
//             +---------+        +---------+        +---------+        +---------+

//             * There are three steps to insert a node into a list
//             * Step(1): shown as (1) in the figure above
//             Create a new node to be inserted
//             * Step(2):
//             Make the "next" pointer of the New Node point to the Node(a)
//             * Step(3):
//             Make the "next" pointer of the afterMe node point to the New Node
//             */
//            afterMe->_pNext = new Node<T>(val, afterMe->_pNext);
//        }
//    }

//    template<class T> void List<T>::push_back(T val)
//    {
//        /* The list is empty? */
//        if (_pHead == NULL) {
//            /* the same to create a new list with a given value */
//            _pTail = _pHead = new Node<T>(val);
//        }
//        else
//        {
//            /* Append the new node to the tail */
//            _pTail->_pNext = new Node<T>(val);
//            /* Update the tail pointer */
//            _pTail = _pTail->_pNext;
//        }
//    }

//    template<class T> Node* List<T>::getNode(T pos)
//    {
//        Node<T>* p = _pHead;
//        /* Counter */
//        while (pos--) {
//            if (p != NULL)
//                p = p->_pNext;
//            else
//                return NULL;
//        }
//        return p;
//    }



//}
//#endif // LIST_HPP

