#ifndef RBTREE_HPP
#define RBTREE_HPP

extern "C" void printk(const char *fmt, ...);

namespace coslib{
    template<typename T> class RBTree {
    public:
        class RBTreeNode {
        public:
            enum Color {
                RED = 0,
                BLACK = 1
            };
            enum Side {
                LEFT = 0,
                RIGHT = 1,
            };

            RBTreeNode (unsigned long k, T * o, RBTree<T> * t) :
                obj (o), key (k), color (RED), parent (NULL), tree (t) {
                this->link[LEFT]  = NULL;
                this->link[RIGHT] = NULL;
            }
            ~RBTreeNode () {
                if (this->link[LEFT] != NULL)
                    delete this->link[LEFT];
                if (this->link[RIGHT] != NULL)
                    delete this->link[RIGHT];
            }

            inline void swapColor (RBTreeNode &node) {
                Color c = this->color;
                this->color = node.color;
                node.color = c;
            }
            inline T * getObj () { return this->obj; }
            inline void setBlack () { this->color = BLACK; }
            inline void setRed () { this->color = RED; }
            inline Color getColor () { return this->color; }
            inline bool isBlack () { return (this->color == BLACK); }
            inline bool isRed () { return (this->color == RED); }

            inline Side whichSide (RBTreeNode &node) {
                if (this->link[LEFT] == &node)
                    return LEFT;
                else if (this->link[RIGHT] == &node)
                    return RIGHT;
                return LEFT;
            }

            inline Side otherSide (Side s) {
                return (s == LEFT ? RIGHT : LEFT);
            }

            inline RBTreeNode * getBrother () {
                if (this->parent == NULL)
                    return NULL;

                return (this->parent->link[LEFT] == this ?
                            this->parent->link[RIGHT] : this->parent->link[LEFT]);
            }

            inline void attach (RBTreeNode &node) {
                Side s = (node.key < this->key ? LEFT : RIGHT);
                this->attach (s, node);
            }

            inline void attach (Side s, RBTreeNode &node) {
                this->link[s] = &node;
                if (&node != NULL)
                    node.parent = this;
            }

            inline RBTreeNode * detach (Side s) {

                if (this == NULL || this->link[s] == NULL)
                    return NULL;

                RBTreeNode * node = this->link[s];
                this->link[s]->parent = NULL;
                this->link[s] = NULL;
                return node;
            }
            inline RBTreeNode * detach (RBTreeNode &node) {
                if (this->link[RIGHT] == &node)
                    return this->detach (RIGHT);
                else if (this->link[LEFT] == &node)
                    return this->detach (LEFT);

                return NULL;
            }
            inline RBTreeNode * searchMax () {
                if (this->link[RIGHT] != NULL)
                    return this->link[RIGHT]->searchMax ();
                else
                    return this;
            }
            inline RBTreeNode * searchMin () {
                if (this->link[LEFT] != NULL)
                    return this->link[LEFT]->searchMin ();
                else
                    return this;
            }
            void rotate (Side s) {
                RBTreeNode * nLeaf;   // New leaf
                RBTreeNode * nParent; // New parent
                RBTreeNode * nGrand;  // New grand father
                Side r = otherSide (s);

                nGrand = this->parent;
                nParent = this->detach (r);

                nLeaf = nParent->detach (s);

                if (nGrand) {
                    Side ps = nGrand->whichSide (*this);
                    nGrand->detach (ps);
                    nGrand->attach (ps, *nParent);
                }
                else {
                    this->tree->root = nParent;
                }

                nParent->attach (s, *this);

                if (nLeaf != NULL)
                    this->attach (r, *nLeaf);
            }

            void adjustInsert ()
            {
                if (this->parent == NULL) {
                    // this node is root
                    this->setBlack ();
                    return ;
                }
                else {
                    if (this->parent->isRed ()) {
                        RBTreeNode * cParent = this->parent;
                        RBTreeNode * grand = this->parent->parent;
                        RBTreeNode * uncle = this->parent->getBrother ();
                        Side s;

                        if (uncle->isRed ()) {
                            uncle->setBlack ();
                            this->parent->setBlack ();
                            grand->setRed ();
                            grand->adjustInsert ();
                        }
                        else {
                            if (this->parent->whichSide (*this) !=
                                    grand->whichSide (*this->parent)) {
                                s = otherSide (cParent->whichSide (*this));
                                cParent->rotate (s);
                                cParent = this;
                            }

                            s = otherSide (grand->whichSide (*cParent));
                            grand->rotate (s);

                            grand->swapColor (*cParent);
                        }
                    }
                }
            }

            bool insert (RBTreeNode &node)
            {
                {
                    Side s = (node.key < this->key ? LEFT : RIGHT);
                    if (this->link[s] != NULL)
                        return this->link[s]->insert (node);
                    else
                        this->attach (s, node);
                }

                node.adjustInsert ();
                return true;
            }

            RBTreeNode * lookup (unsigned long k)
            {
                if (this->key == k) {
                    return this;
                }
                else {
                    Side s = (k < this->key ? LEFT : RIGHT);
                    return (this->link[s] == NULL ? NULL : this->link[s]->lookup (k));
                }
            }

            void leave () {
                // only detach from tree, balancing color & tree in adjustLeave ()
                RBTreeNode * cParent = this->parent;

                if (this->link[LEFT] == NULL && this->link[RIGHT]  == NULL) {
                    if (cParent) {
                        Side s = cParent->whichSide (*this);
                        cParent->detach (*this);

                        if (this->isBlack ()) {
                            cParent->link[s]->adjustLeave (cParent);
                        }
                    }
                    else  {
                        this->tree->root = NULL;
                    }
                }
                else if ((this->link[LEFT] == NULL) ^
                         (this->link[RIGHT] == NULL)) {
                    Side s = (this->link[LEFT] == NULL ? RIGHT : LEFT);
                    RBTreeNode * cTarget = this->detach (s);

                    if (cParent) {
                        cParent->detach (*this);
                        cParent->attach (*cTarget);
                    }
                    else
                        this->tree->root = cTarget;

                    if (this->isBlack ())
                        cTarget->adjustLeave (cParent);
                }
                else {
                    // swap target node & maximum node in left subtree

                    RBTreeNode * cMax = this->link[LEFT]->searchMax ();
                    RBTreeNode * mParent = cMax->parent;
                    RBTreeNode * cLeft  = this->detach (LEFT);
                    RBTreeNode * cRight = this->detach (RIGHT);
                    RBTreeNode * mLeft  = cMax->detach (LEFT);

                    this->attach (*mLeft);
                    if (cParent) {
                        cParent->detach (*this);
                    }
                    else {
                        this->tree->root = NULL;
                    }

                    if (cMax != cLeft) {
                        // cMax have more 1 hop from THIS
                        mParent->detach (*cMax);
                        mParent->attach (*this);
                        cMax->attach (LEFT,  *cLeft);
                        cMax->attach (RIGHT, *cRight);
                    }
                    else {
                        // cMax == cLeft (cMax is left node of THIS)
                        cMax->attach (RIGHT, *cRight);
                        cMax->attach (LEFT,  *this);
                    }

                    if (cParent) {
                        cParent->attach (*cMax);
                    }
                    else {
                        this->tree->root = cMax;
                    }

                    this->swapColor (*cMax);
                    this->leave ();
                }
            }

            void adjustLeave (RBTreeNode * cParent)
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

                RBTreeNode * cNeighbor =
                        cParent->link[otherSide (cParent->whichSide (*this))];

                // cParent->tree->dumpTree ("Adjusting by Leave");

                if (cNeighbor->isRed ()) {
                    Side s = cParent->whichSide (*this);
                    cParent->swapColor (*cNeighbor);
                    cParent->rotate (s);
                    cNeighbor = cParent->link[otherSide(s)];
                }
                else if (cParent->isBlack () &&
                         cNeighbor->link[LEFT]->isBlack () &&
                         cNeighbor->link[RIGHT]->isBlack ()) {
                    cNeighbor->setRed ();
                    return cParent->adjustLeave (cParent->parent);
                }

                if (cParent->isRed () &&
                        cNeighbor->link[LEFT]->isBlack () &&
                        cNeighbor->link[RIGHT]->isBlack ()) {
                    cParent->swapColor (*cNeighbor);
                }
                else {
                    Side ns = cParent->whichSide(*cNeighbor); // Neighbor side
                    Side os = otherSide (ns); // Other side

                    if (cNeighbor->link[os]->isRed () &&
                            cNeighbor->link[ns]->isBlack ()) {
                        cNeighbor->swapColor (*cNeighbor->link[os]);
                        cNeighbor->rotate (ns);
                        cNeighbor = cParent->link[ns];
                    }

                    if (cNeighbor->link[ns]->isRed ()) {
                        cNeighbor->link[ns]->setBlack ();
                        cParent->swapColor (*cNeighbor);
                        cParent->rotate (os);
                    }
                }
            }
            int count_size(){
                if(this == NULL)
                    return 0;

                int size = 1;
                if (this->link[RIGHT] != NULL){
                    size += this->link[RIGHT]->count_size();
                }
                if (this->link[LEFT] != NULL) {
                    size += this->link[LEFT]->count_size() ;
                }
                return size;
            }

            void attach_tree(RBTree<T> * t){
                tree = t;
            }
            void set_key(unsigned long k){
                key = k;
            }
        private:
            friend class RBTree<T>;

            T * obj;
            unsigned long key;
            Color color;
            RBTreeNode * parent, * link[2];
            RBTree<T> * tree;
        };

        RBTreeNode *root;

        RBTreeNode *max_key_node = NULL;

    public:
        RBTree () : root (NULL) {}
        ~RBTree () {
            delete this->root;
        }

        bool insert (unsigned long key, T *p) {
            RBTreeNode * node = new RBTreeNode (key, p, this);

            if (this->root) {
                if (! this->root->insert (*node)) {
                    delete node;
                    return false;
                }
            }
            else {
                this->root = node;
                node->setBlack ();
            }

            return true;
        }

        T * lookup (unsigned long key)
        {
            if (NULL == this->root)
                return NULL;

            RBTreeNode * node = this->root->lookup (key);
            return (node ? node->getObj () : NULL);
        }

        bool remove (unsigned long key)
        {
            if (NULL == this->root)
                return false;

            RBTreeNode * node = this->root->lookup (key);
            if (NULL == node)
                return false;

            node->leave ();
            delete node;

            return true;
        }

        bool insert (RBTreeNode * node) {
            node->attach_tree(this);

            if (this->root) {
                if (! this->root->insert (*node)) {
                    return false;
                }
            }
            else {
                this->root = node;
                node->setBlack ();
            }
            return true;
        }

        bool remove (RBTreeNode * node)
        {
            if (NULL == this->root || NULL == node || NULL == node->tree)
                return false;

            node->leave();

            node->attach_tree(NULL);

            return true;
        }

        T *max()
        {
            return root->searchMax()->getObj();
        }

        T *min()
        {

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

}
#endif // RBTREE_HPP

