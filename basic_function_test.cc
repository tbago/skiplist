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
