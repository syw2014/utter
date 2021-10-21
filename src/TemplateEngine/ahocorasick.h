/*
 * @Author: your name
 * @Date: 2021-10-13 15:06:42
 * @LastEditTime: 2021-10-14 10:29:13
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\ahocorasick\ahocorasick.h
 */
/*************************************************************************
# File Name: ahocorasick.h
# Method: 
# Author: Jerry Shi
# Mail: jerryshi0110@gmail.com
# Created Time: 2020年07月13日 11:09:56
 ************************************************************************/
// An implementation of ahc-corasick algorithms for multi-pattern searching

#ifndef AHOCORASICK_H_
#define AHOCORASICK_H_

#include <map>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include<ctype.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <dirent.h>
#include <mutex>
#include "normalize.h"



class TrieNode {
public:
    // std::vector<UnicodeType> word; // token in document
    std::string word;
    // std::map<std::string, TrieNode*> next; // next node, can be opitimized with DoubleArrayTree
    std::map<UnicodeType, TrieNode*> next;
    TrieNode *fail; // failur  e pointer
    bool isMatched; // wheather matched, match means root node
    int termFreq;   // term frequency
    int index;  // first position of  occr matched in string
    int wordLength;			// word number in string 

    int label;  // label of word, to show the type of matched keywords

public:
    TrieNode(); // construct of TrieNode
    ~TrieNode();    // destructor
};   // end TrieNode



// Create ACAutomaton
class ACAutomaton{
public:
    TrieNode *root; // root node of ac
    std::vector<TrieNode*> instances;   // store all tail nodels
public:
    ACAutomaton();  // construction

    ~ACAutomaton(); // destructor

    // Split text into word bytes, record word length and characters
    // void SplitWord(const std::string &text, int &wordLength, std::vector<std::string> &characters);
    void SplitWord(const std::string &text, int &wordLength, std::vector<UnicodeType> &characters);

    // Get next node of the current node
    // TrieNode* GetNext(TrieNode *current, std::string &character);
    TrieNode* GetNext(TrieNode *current, UnicodeType &character);
    
    // Add word(pattern) and it's label into trie tree
    void AddWord(const std::string &pattern, const int  &label);

    // Create dictionary
    void Build();


    // Seach pattern in given text, if matched return true
    bool IsMatch(const std::string &text);

    // Get all matched nodes
    // void Search(const std::string &text, std::map<std::string, TrieNode*> &nodes);

    // // Get the longest match nodes
    // void SearchLongest(const std::string &text, std::map<std::string, TrieNode*> &nodes);

    // Get all matched nodes
    void Search(const std::string &text, std::map<std::string, TrieNode*> &nodes);

    // Get the longest match nodes
    void SearchLongest(const std::string &text, std::map<std::string, TrieNode*> &nodes);

}; // end ACAutomaton class


#endif // AHOCORASICK_H_