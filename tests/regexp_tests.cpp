#include <libformal/regexp_algorithms.hpp>
#include <gtest/gtest.h>

TEST(GeneralTest, RegExpTest1) {
    // Public test 1
    EXPECT_EQ(formal::GetPrefixedMin("ab+c.aba.*.bac.+.+*", 'c', 4), formal::INT_NONE);
}

TEST(GeneralTest, RegExpTest2) {
    // Public test 2
    EXPECT_EQ(formal::GetPrefixedMin("acb..bab.c.*.ab.ba.+.+*a.", 'b', 2), 4);
}

TEST(GeneralTest, RegExpTest3) {
    std::string test = "aa.b.*cc..";
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 0), 2);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 1), 5);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 2), 5);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 3), formal::INT_NONE);
}

TEST(GeneralTest, RegExpTest4) {
    std::string test = "aaba...aba..+1+";
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 0), 0);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 1), 3);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 2), 4);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 3), formal::INT_NONE);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 100), formal::INT_NONE);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'z', 0), 0);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'z', 1), formal::INT_NONE);
}

TEST(GeneralTest, RegExpTest5) {
    std::string test = "aa.1+";
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 0), 0);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 1), 2);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 2), 2);
    EXPECT_EQ(formal::GetPrefixedMin(test, 'a', 3), formal::INT_NONE);
}

TEST(GeneralTest, RegExpTest6) {
    EXPECT_ANY_THROW(formal::GetPrefixedMin("a.", 'a', 1));
    EXPECT_ANY_THROW(formal::GetPrefixedMin(".", 'a', 1));
    EXPECT_ANY_THROW(formal::GetPrefixedMin("aa.*+", 'a', 1));
    EXPECT_ANY_THROW(formal::GetPrefixedMin("*", 'a', 1));
}