/*
 * @Author: your name
 * @Date: 2021-10-13 15:06:42
 * @LastEditTime: 2021-11-10 14:25:49
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\ahocorasick\ahocorasick.h
 */
// An implementation of ahc-corasick algorithms for multi-pattern searching

#ifndef AHOCORASICK_H_
#define AHOCORASICK_H_

#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

#include <cstring>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "normalize.h"
#include <dirent.h>
#include <mutex>

namespace StringMatching {

typedef std::pair<int, int> IndexLabelPairType;

class TrieNode {
  public:
    // std::vector<UnicodeType> word; // token in document
    std::string word;
    // std::map<std::string, TrieNode*> next; // next node, can be opitimized
    // with DoubleArrayTree
    std::map<UnicodeType, TrieNode *> next;
    TrieNode *fail; // failur  e pointer
    bool isMatched; // wheather matched, match means root node
    int termFreq;   // term frequency
    int index;      // first position of  occr matched in string
    int wordLength; // word number in string

    int label; // label of word, to show the type of matched keywords

  public:
    TrieNode();  // construct of TrieNode
    ~TrieNode(); // destructor
};               // end TrieNode

// Create ACAutomaton
class ACAutomaton {
  public:
    TrieNode *root;                    // root node of ac
    std::vector<TrieNode *> instances; // store all tail nodels
    bool strToUpper; // whether all the keywords add into ahc convert to upper
                     // or not
  public:
    ACAutomaton(bool strToUpper); // construction
    ACAutomaton();                // construction

    ~ACAutomaton(); // destructor

    // Split text into word bytes, record word length and characters
    // void SplitWord(const std::string &text, int &wordLength,
    // std::vector<std::string> &characters);
    void SplitWord(const std::string &text, int &wordLength,
                   std::vector<UnicodeType> &characters);

    // Get next node of the current node
    // TrieNode* GetNext(TrieNode *current, std::string &character);
    TrieNode *GetNext(TrieNode *current, UnicodeType &character);

    // Add word(pattern) and it's label into trie tree
    void AddWord(const std::string &pattern, const int &label);

    // Create dictionary
    void Build();

    // Seach pattern in given text, if matched return true
    bool IsMatch(const std::string &text);

    // Get all matched nodes
    // void Search(const std::string &text, std::map<std::string, TrieNode*>
    // &nodes);

    // // Get the longest match nodes
    // void SearchLongest(const std::string &text, std::map<std::string,
    // TrieNode*> &nodes);

    // Get all matched nodes
    void Search(const std::string &text,
                std::map<std::string, std::vector<IndexLabelPairType>> &nodes);

    // Get the longest match nodes
    void SearchLongest(
        const std::string &text,
        std::map<std::string, std::vector<IndexLabelPairType>> &nodes);

    // Parse keyowrds from search results
    void ParseSearchWords(
        std::map<std::string, std::vector<IndexLabelPairType>> &nodes,
        std::vector<std::string> &keywords, std::vector<int> &indexes,
        std::vector<int> &labelIds);

    // Search and parse word text, index, labels with the specified mode
    // (longest or full search)
    void SearchParse(const std::string &text,
                     std::vector<std::string> &keywords,
                     std::vector<int> &indexes, std::vector<int> &labelIds,
                     const bool isLongest = true);

}; // end ACAutomaton class

} // namespace StringMatching

#endif // AHOCORASICK_H_
