#include <list.hpp>
#include <iostream>
#include <gtest/gtest.h>


using namespace std;

class ListTest: public ::testing::Test
{
public:
    ListTest()
    {
    }
    ~ListTest()
    {
    }
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(ListTest, test)
{
    coslib::List<int>::Node *node1 = new coslib::List<int>::Node(1);
    coslib::List<int>::Node *node2 = new coslib::List<int>::Node(2);
    coslib::List<int>::Node *node3 = new coslib::List<int>::Node(3);
    coslib::List<int> list;
    list.push_back(node1);
    list.push_back(node2);
    list.push_back(node3);
    EXPECT_EQ(list.getNode(0), node1);
    EXPECT_EQ(list.getNode(1), node2);
    EXPECT_EQ(list.getNode(2), node3);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.empty(), false);
    EXPECT_EQ(list.size(), 3);

    list.erase(node2);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.empty(), false);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.getNode(1), node3);

    list.erase(node1);
    EXPECT_EQ(list.front(), 3);
    list.erase(node3);
    EXPECT_EQ(list.empty(), true);
    EXPECT_EQ(list.size(), 0);

}

int main(int argc, char **argv)
{


    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}
