/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef RBTREE_HPP
#define RBTREE_HPP

extern "C" void printk(const char *fmt, ...);

#ifndef NULL
#define NULL 0
#endif
#include <iostream>

namespace coslib{
    template<class T> class RBTree {
    public:
        class Node {
        public:
            enum Color {
                RED = 0,
                BLACK = 1
            };
            enum Side {
                LEFT = 0,
                RIGHT = 1,
            };

            Node(long k, T o, RBTree<T> *t = NULL) :
                obj (o), key (k), color (RED), parent (NULL), tree (t) {
                this->link[LEFT]  = RBTree<T>::nil;
                this->link[RIGHT] = RBTree<T>::nil;
            }

            //nil
            Node() :
                key (-1), parent (NULL), tree (NULL) {
                //std::cout<<"sadsasdasdad\n";
                //printk("dasdasdad\n");
                this->color = BLACK;
                this->link[LEFT] = NULL;
                this->link[RIGHT] = NULL;
            }

            ~Node() {
                if (this->link[LEFT]->isNil() == false)
                    delete this->link[LEFT];
                if (this->link[RIGHT]->isNil() == false)
                    delete this->link[RIGHT];
            }

            inline void swapColor (Node *node) {
                Color c = this->color;
                this->color = node->color;
                node->color = c;
            }

            inline bool isNil() { return (this == RBTree<T>::nil); }
            inline T getObj() { return this->obj; }
            inline void setBlack() { this->color = BLACK; }
            inline void setRed() { this->color = RED; }
            inline Color getColor() { return this->color; }
            inline bool isBlack() { return (this->color == BLACK); }
            inline bool isRed() { return (this->color == RED); }

            inline Side whichSide(Node *node) {
                if (this->link[LEFT] == node)
                    return LEFT;
                else if (this->link[RIGHT] == node)
                    return RIGHT;

                return LEFT;
            }

            inline Side otherSide(Side s) {
                return (s == LEFT ? RIGHT : LEFT);
            }

            inline Node *getBrother () {
                if (this->parent == NULL)
                    return NULL;

                return (this->parent->link[LEFT] == this ?
                            this->parent->link[RIGHT] : this->parent->link[LEFT]);
            }

            inline void attach(Node *node) {
                Side s = (node->key < this->key ? LEFT : RIGHT);
                this->attach(s, node);
            }

            inline void attach(Side s, Node *node) {
                this->link[s] = node;

                //std::cout<< this->obj << " " << " " << node->obj << " " << s <<std::endl;

                if (!node->isNil())
                    node->parent = this;
            }

            inline Node *detach(Side s) {
                if (this->isNil() || this->link[s]->isNil())
                    return RBTree<T>::nil;

                Node * node = this->link[s];
                this->link[s]->parent = NULL;
                this->link[s] = RBTree<T>::nil;

                return node;
            }

            inline Node *detach(Node *node) {
                if (this->link[RIGHT] == node)
                    return this->detach(RIGHT);
                else if (this->link[LEFT] == node)
                    return this->detach(LEFT);

                return NULL;
            }

            inline Node *searchMax() {
                if (! this->link[RIGHT]->isNil())
                    return this->link[RIGHT]->searchMax ();
                else
                    return this;
            }
            inline Node *searchMin() {
                if (! this->link[LEFT]->isNil())
                    return this->link[LEFT]->searchMin ();
                else
                    return this;
            }
            void rotate(Side s) {
                Node * nLeaf;   // New leaf
                Node * nParent; // New parent
                Node * nGrand;  // New grand father
                Side r = otherSide(s);

                nGrand = this->parent;
                nParent = this->detach(r);

                nLeaf = nParent->detach(s);

                if (nGrand){
                    Side ps = nGrand->whichSide(this);
                    nGrand->detach(ps);
                    nGrand->attach(ps, nParent);
                }
                else {
                    this->tree->root = nParent;
                }

                nParent->attach(s, this);

                if (! nLeaf->isNil())
                    this->attach(r, nLeaf);
            }

            void adjustInsert()
            {
                if (this->parent == NULL) {
                    // this node is root
                    this->setBlack ();
                    return ;
                }
                else {
                    if (this->parent->isRed()) {
                        Node * cParent = this->parent;
                        Node * grand = this->parent->parent;
                        Node * uncle = this->parent->getBrother();
                        Side s;

                        if (uncle->isRed()) {
                            uncle->setBlack();
                            this->parent->setBlack();
                            grand->setRed();
                            grand->adjustInsert();
                        }
                        else {
                            if (this->parent->whichSide(this) !=
                                    grand->whichSide(this->parent)) {
                                s = otherSide (cParent->whichSide(this));
                                cParent->rotate(s);
                                cParent = this;
                            }

                            s = otherSide (grand->whichSide(cParent));
                            grand->rotate(s);

                            grand->swapColor(cParent);
                        }
                    }
                }
            }

            bool insert(Node *node)
            {
                {
                    Side s = (node->key < this->key ? LEFT : RIGHT);
                    if(! this->link[s]->isNil())
                        return this->link[s]->insert(node);
                    else
                        this->attach(s, node);
                }
                node->adjustInsert();
                return true;
            }

            Node *lookup(long k)
            {
                if (this->key == k) {
                    return this;
                }
                else {
                    Side s = (k < this->key ? LEFT : RIGHT);
                    return (this->link[s]->isNil() ? NULL : this->link[s]->lookup(k));
                }
            }

            void leave() {
                // only detach from tree, balancing color & tree in adjustLeave ()
                Node *cParent = this->parent;

                if (this->link[LEFT]->isNil() && this->link[RIGHT]->isNil()) {
                    if (cParent) {
                        Side s = cParent->whichSide(this);
                        cParent->detach(this);

                        if (this->isBlack ()) {
                            cParent->link[s]->adjustLeave(cParent);
                        }
                    }
                    else  {
                        this->tree->root = NULL;
                    }
                }
                else if ((this->link[LEFT]->isNil()) ^
                         (this->link[RIGHT]->isNil())) {
                    Side s = (this->link[LEFT]->isNil() ? RIGHT : LEFT);
                    Node *cTarget = this->detach(s);
                    if (cParent) {
                        cParent->detach(this);
                        cParent->attach(cTarget);
                    }
                    else
                        this->tree->root = cTarget;

                    if (this->isBlack ()){
                        cTarget->adjustLeave(cParent);
                    }
                }
                else {
                    // swap target node & maximum node in left subtree

                    Node *cMax = this->link[LEFT]->searchMax ();
                    Node *mParent = cMax->parent;
                    Node *cLeft  = this->detach (LEFT);
                    Node *cRight = this->detach (RIGHT);
                    Node *mLeft  = cMax->detach (LEFT);

                    this->attach (mLeft);
                    if (cParent) {
                        cParent->detach(this);
                    }
                    else {
                        this->tree->root = NULL;
                    }

                    if (cMax != cLeft) {
                        // cMax have more 1 hop from THIS
                        mParent->detach (cMax);
                        mParent->attach (this);
                        cMax->attach (LEFT,  cLeft);
                        cMax->attach (RIGHT, cRight);
                    }
                    else {
                        // cMax == cLeft (cMax is left node of THIS)
                        cMax->attach(RIGHT, cRight);
                        cMax->attach(LEFT,  this);
                    }

                    if (cParent) {
                        cParent->attach(cMax);
                    }
                    else {
                        this->tree->root = cMax;
                    }

                    this->swapColor(cMax);
                    this->leave();
                }
            }

            void adjustLeave(Node *cParent)
            {
                // nothing to do when node is root
                if (NULL == cParent) {
                    this->setBlack ();
                    return ;
                }
                if (this->isRed ()) {
                    this->setBlack ();
                    return ;
                }

                Node *cNeighbor =
                        cParent->link[otherSide(cParent->whichSide(this))];

                if (cNeighbor->isRed()) {
                    Side s = cParent->whichSide(this);
                    cParent->swapColor(cNeighbor);
                    cParent->rotate(s);
                    cNeighbor = cParent->link[otherSide(s)];
                }
                else if (cParent->isBlack () &&
                         cNeighbor->link[LEFT]->isBlack () &&
                         cNeighbor->link[RIGHT]->isBlack ()) {
                    cNeighbor->setRed ();
                    return cParent->adjustLeave(cParent->parent);
                }

                if (cParent->isRed () &&
                        cNeighbor->link[LEFT]->isBlack () &&
                        cNeighbor->link[RIGHT]->isBlack ()) {
                    cParent->swapColor(cNeighbor);
                }
                else {
                    Side ns = cParent->whichSide(cNeighbor); // Neighbor side
                    Side os = otherSide(ns); // Other side

                    if (cNeighbor->link[os]->isRed () &&
                            cNeighbor->link[ns]->isBlack ()) {
                        cNeighbor->swapColor(cNeighbor->link[os]);
                        cNeighbor->rotate(ns);
                        cNeighbor = cParent->link[ns];
                    }

                    if (cNeighbor->link[ns]->isRed ()) {
                        cNeighbor->link[ns]->setBlack ();
                        cParent->swapColor(cNeighbor);
                        cParent->rotate(os);
                    }
                }
            }

            int count_size(){
                if(this == NULL)
                    return 0;

                int size = 1;
                if (! this->link[RIGHT]->isNil()){
                    size += this->link[RIGHT]->count_size();
                }
                if (! this->link[LEFT]->isNil()) {
                    size += this->link[LEFT]->count_size() ;
                }
                return size;
            }

            void attach_tree(RBTree<T> * t){
                tree = t;
            }

            void set_key(long k){
                key = k;
            }
            long get_key(){
                return key;
            }
        private:
            friend class RBTree<T>;

            T obj;
            long key;
            Color color;
            Node *parent, *link[2];
            RBTree<T> * tree;
        };

        static Node *nil;
        Node *root;

    public:
        RBTree() : root (NULL) {

        }
        ~RBTree() {
            delete this->root;
        }

        T lookup(long key)
        {
            if (NULL == this->root)
                return NULL;

            Node *node = this->root->lookup (key);
            return (node ? node->getObj () : NULL);
        }

        bool insert (Node *node) {
            node->attach_tree(this);
            if (this->root) {
                if (!this->root->insert(node)) {
                    return false;
                }
            }
            else {
                this->root = node;
                node->setBlack();
            }
            return true;
        }

        bool remove (Node *node)
        {
            if (NULL == this->root || NULL == node || NULL == node->tree)
                return false;

            node->leave();

            node->link[Node::LEFT] = RBTree<T>::nil;
            node->link[Node::RIGHT] = RBTree<T>::nil;

            node->attach_tree(NULL);

            return true;
        }

        T  max()
        {
            if(root == NULL)
                return NULL;
            return root->searchMax()->getObj();
        }

        T  min()
        {
            if(root == NULL)
                return NULL;
            return root->searchMin()->getObj();
        }

        bool empty()
        {
            if(root != NULL)
                return false;
            return true;
        }

        int size()
        {
            if(root == NULL)
                return 0;
            return root->count_size();
        }

    };

    template<typename T>  typename RBTree<T>::Node *RBTree<T>::nil = new RBTree<T>::Node();
}
#endif // RBTREE_HPP

