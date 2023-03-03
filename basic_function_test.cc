#include "gtest/gtest.h"
#include "skiplist.hpp"
#include <string>

TEST(TestNode, NodeOperation) {
    Node<int, std::string> newNode(1, "abc", 2);
    EXPECT_EQ(newNode.GetKey(), 1);
    EXPECT_EQ(newNode.GetValue(), "abc");
    newNode.SetValue("aaa");
    EXPECT_EQ(newNode.GetValue(), "aaa");
}

SkipList<int, std::string> skiplist(6);

TEST(TestSkipList, SkipListInsert) {
    skiplist.InsertElement(1, "abc");
    EXPECT_EQ(skiplist.GetElementCount(), 1);
}

TEST(TestSkipList, SkipListSearch) {
    std::string value;
    EXPECT_FALSE(skiplist.SearchElement(2, value));
    EXPECT_TRUE(skiplist.SearchElement(1, value));
}
