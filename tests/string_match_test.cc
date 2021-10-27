/*
 * @Author: your name
 * @Date: 2021-10-20 14:52:29
 * @LastEditTime: 2021-10-27 14:32:35
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\tests\temp_engine_test.cc
 */

#include "ahocorasick.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Create AHC instance
StringMatching::ACAutomaton *ahc = new StringMatching::ACAutomaton();
// Test keywords as entity
std::vector<std::string> keywords = {
    "王者荣耀", "王者",     "天空之城",     "稻花香",      "哈利波特",
    "悟",       "我想要",   "帮我",         "你好",        "您好",
    "ShangHai", "London",   "London Tower", "13578921465", "万夫2",
    "iphone",   "iphone13", "iphone13pro"};
// Test ids as labels of keywords
std::vector<int> labels = {1, 1, 1, 2, 3,  4,  4,  0,  0,
                           0, 5, 5, 5, 10, 11, 12, 12, 12};

std::map<std::string, std::vector<StringMatching::IndexLabelPairType>>
    nodes;                             // ahc search results
std::vector<std::string> entity_words; // entity words
std::vector<int> index;                // entity index in sentence
std::vector<int> label_ids;            // entity label id

// Add words
TEST(StringMatchUnitTest, AddWord) {
    // Add word to ahc
    for (std::size_t idx = 0; idx < keywords.size(); ++idx) {
        ahc->AddWord(keywords[idx], labels[idx]);
    }
    ahc->Build();
}

// Test1, single word match
TEST(StringMatchUnitTest, SingleWordMatch) {
    bool expected = true;
    std::string actual = "悟";
    bool isMatch = false;
    isMatch = ahc->IsMatch(actual);
    EXPECT_EQ(expected, isMatch);
}

// Test2, english match
TEST(StringMatchUnitTest, EnglishMatch) {
    bool expected = true;
    std::string w = "Tower";
    bool actual = ahc->IsMatch(w);
    EXPECT_NE(expected, actual);
}

// Test3, digital match
TEST(StringMatchUnitTest, DigitalMatch) {
    bool expected = true;
    std::string w = "13578921465";
    bool actual = ahc->IsMatch(w);
    EXPECT_EQ(expected, actual);
}

// Test4, digital not match
TEST(StringMatchUnitTest, DigitalNotMatch) {
    bool expected = true;
    std::string w = "13578";
    bool actual = ahc->IsMatch(w);
    EXPECT_NE(expected, actual);
}

// Test5, all keywords match
TEST(StringMatchUnitTest, AllSearch) {
    nodes.clear();
    std::vector<std::string> expected = {"王者", "王者荣耀", "万夫2"};
    std::string sentence = "打开王者荣耀，万夫2.0";
    ahc->Search(sentence, nodes);
    ahc->ParseSearchWords(nodes, entity_words, index, label_ids);

    EXPECT_EQ(expected.size(), entity_words.size());
    EXPECT_THAT(entity_words, ::testing::UnorderedElementsAreArray(expected));
}

// Test6, all keywords match
TEST(StringMatchUnitTest, AllCombinationSearch) {
    nodes.clear();
    std::vector<std::string> expected = {"IPHONE", "IPHONE13", "IPHONE13PRO"};
    std::string sentence = "我想买iphone13pro max手机";
    ahc->Search(sentence, nodes);
    ahc->ParseSearchWords(nodes, entity_words, index, label_ids);

    EXPECT_EQ(expected.size(), entity_words.size());
    EXPECT_THAT(entity_words, ::testing::UnorderedElementsAreArray(expected));
}

// Test7, longest keywords match
TEST(StringMatchUnitTest, LongestSearch) {
    nodes.clear();
    std::vector<std::string> expected = {"王者", "王者荣耀", "万夫2"};
    std::string sentence = "打开王者王者荣耀，万夫2.0";
    ahc->SearchLongest(sentence, nodes);
    ahc->ParseSearchWords(nodes, entity_words, index, label_ids);

    EXPECT_EQ(expected.size(), entity_words.size());
    EXPECT_THAT(entity_words, ::testing::UnorderedElementsAreArray(expected));
}
