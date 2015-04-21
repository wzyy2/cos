#include <rbtree.hpp>
#include <iostream>
#include <gtest/gtest.h>


using namespace std;

class RBTreeTest: public ::testing::Test
{
public:
    RBTreeTest()
    {
    }
    ~RBTreeTest()
    {
    }
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(RBTreeTest, test)
{
    coslib::RBTree<int>::Node *node1 = new coslib::RBTree<int>::Node(1, 1);
    coslib::RBTree<int>::Node *node11 = new coslib::RBTree<int>::Node(1, 11);
    coslib::RBTree<int>::Node *node2 = new coslib::RBTree<int>::Node(2, 2);
    coslib::RBTree<int>::Node *node3 = new coslib::RBTree<int>::Node(3, 3);

    coslib::RBTree<int>::Node *node0 = new coslib::RBTree<int>::Node(0, 0);

    coslib::RBTree<int> tree;

//    tree.insert(node1);

//    EXPECT_EQ(tree.lookup(1), 1);
//    EXPECT_EQ(tree.max(), 1);
//    EXPECT_EQ(tree.min(), 1);

//    tree.remove(node1);
//    EXPECT_EQ(tree.max(), NULL);

    tree.insert(node1);
    tree.insert(node11);
    tree.insert(node2);
    tree.insert(node3);
    //EXPECT_EQ(node1->whichSide(node11), coslib::RBTree<int>::Node::RIGHT);
    EXPECT_EQ(tree.max(), 3);
    EXPECT_EQ(tree.min(), 1);
    EXPECT_EQ(tree.size(), 4);

    tree.remove(node2);
    EXPECT_EQ(tree.max(), 3);
    EXPECT_EQ(tree.size(), 3);
    tree.remove(node1);
    EXPECT_EQ(tree.max(), 3);
    EXPECT_EQ(tree.min(), 11);
    EXPECT_EQ(tree.size(), 2);

    tree.insert(node1);
    tree.remove(node3);
    EXPECT_TRUE(tree.max() == 11 || tree.max() == 1);
    EXPECT_TRUE(tree.min() == 11 || tree.min() == 1);
    EXPECT_EQ(tree.size(), 2);

    tree.insert(node0);
    tree.remove(node11);
    EXPECT_EQ(tree.max(), 1);
    EXPECT_EQ(tree.min(), 0);
    EXPECT_EQ(tree.size(), 2);
}

int main(int argc, char **argv)
{


    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}
