/*************************************************************************
# File Name: ahocorasick.cc
# Method:
# Author: Jerry Shi
# Mail: jerryshi0110@gmail.com
# Created Time: 2020年07月13日 11:10:16
 ************************************************************************/
//#include <cstring>
//#include <stdio.h>
//#include <stdlib.h>
//#include <ctype>

//#include <iostream>
#include "ahocorasick.h"
#include "common.h"

namespace StringMatching {
TrieNode::TrieNode()
    : word(""), fail(0), isMatched(false), termFreq(0), wordLength(0),
      index(-1), label(-1) {}

TrieNode::~TrieNode() { delete this->fail; }

// ACAutomaton class
// Construct
ACAutomaton::ACAutomaton() { this->root = new TrieNode; }

// Destructor
ACAutomaton::~ACAutomaton() { delete this->root; }

void ACAutomaton::SplitWord(const std::string &text, int &word_length,
                            std::vector<UnicodeType> &characters) {

    std::string nstr = text;
    // Normalize text
    STRNormalize::Normalize::ToUpper(nstr);
    bool valid = STRNormalize::Normalize::IsValidUTF8(nstr);
    // convert string to unicodes
    utf8::utf8to16(nstr.begin(), nstr.end(), std::back_inserter(characters));
    word_length = characters.size();

    // int word_size = text.size();    // get number of bytes

    // // ref: https://en.wikipedia.org/wiki/UTF-8#Description
    // for(int i = 0; i < word_size;) {
    //     int char_len = 1;
    //     if ((text[i] & 0x80) == 0xf0) { // 11111000 ~ 11110000
    //         char_len = 4;
    //     } else if((text[i] & 0xf0) == 0xe0) {   // 11100000
    //         char_len = 3;
    //     } else if((text[i] & 0xe0) == 0xc0) {   // 11000000
    //         char_len = 2;
    //     }
    //     if((i+char_len) > word_size) {  // last word
    //         char_len = 1;
    //     }
    //     characters.push_back(text.substr(i, char_len));
    //     i += char_len;
    //     ++word_length;  // record how many words
    // }
}

TrieNode *ACAutomaton::GetNext(TrieNode *current, UnicodeType &character) {
    std::map<UnicodeType, TrieNode *>::iterator iter =
        current->next.find(character); // get next iterator
    if (iter != current->next.end()) {
        return iter->second; // if found return trie node pointer
    }

    // check whether current node was root
    if (current == this->root) {
        return this->root;
    }

    return 0;
}

void ACAutomaton::AddWord(const std::string &pattern, const int &label) {
    int word_length = 0;
    std::vector<UnicodeType> characters;

    SplitWord(pattern, word_length, characters);
    // std::cout << "add test: " << pattern << " number of word: " <<
    // word_length  <<std::endl;

    TrieNode *tmp = this->root;
    // std::cout << "TEST" << std::endl;
    int i = 1; // word length counter
    std::vector<UnicodeType> path_word;
    // UnicodeType path_word;
    for (std::vector<UnicodeType>::iterator iter = characters.begin();
         iter != characters.end(); ++iter) {
        path_word.push_back(*iter);
        // std::cout <<"Debug->" << *iter << std::endl;
        std::map<UnicodeType, TrieNode *>::iterator item =
            tmp->next.find(*iter); // check strings in node path
        if (item != tmp->next.end()) {
            tmp = item->second; // if exist return trie node
        } else { // if not exist then create new node in the graph or tree
            TrieNode *n = new TrieNode;
            // convert unicode to word
            std::string ww;
            bool f = STRNormalize::Normalize::UnicodeToUTF8Str(path_word, ww);
            n->word = ww;
            n->wordLength = i;
            // std::cout <<"Debug->2" << *iter << std::endl;
            tmp->next.insert(std::make_pair(*iter, n));
            tmp = n;
        }

        if (iter + 1 == characters.end()) {
            tmp->isMatched = true;
            // cout << "full word: " << tmp->word << endl;
        }
        ++i;
    }
    tmp->label = label; // pattern label
}

void ACAutomaton::Build() {

    std::queue<TrieNode *> que; //

    // set all fail pointer as root of the node which depth=1, and add all the
    // nodes into queue
    for (std::map<UnicodeType, TrieNode *>::iterator iter = root->next.begin();
         iter != root->next.end(); ++iter) {
        iter->second->fail = root;
        que.push(iter->second);
    }

    instances.clear();
    // instances.push_back(root);

    while (!que.empty()) {
        TrieNode *temp = que.front();
        que.pop();

        // loop the next node of current node
        for (std::map<UnicodeType, TrieNode *>::iterator iter =
                 temp->next.begin();
             iter != temp->next.end(); ++iter) {
            UnicodeType character = iter->first; // get the string

            // push the current node into queue
            que.push(iter->second);

            // set fail pointer, search from parent node
            TrieNode *parent = temp->fail;
            while (!GetNext(parent, character))
                parent = parent->fail;
            iter->second->fail = GetNext(parent, character);
            if (!GetNext(parent, character))
                throw 1;
        }

        // store tail node
        if (temp->isMatched) {
            instances.push_back(temp);
        }
    }
}

bool ACAutomaton::IsMatch(const std::string &text) {
    int buf_length = 0;
    std::vector<UnicodeType> characters;
    SplitWord(text, buf_length, characters);

    TrieNode *tmp = root;
    for (std::vector<UnicodeType>::iterator iter = characters.begin();
         iter != characters.end(); ++iter) {
        while (!GetNext(tmp, *iter)) {
            tmp = tmp->fail;
        }
        tmp = GetNext(tmp, *iter);
        if (tmp->isMatched) {
            return true;
        }
    }
    return false;
}

// return all the matched nodes
void ACAutomaton::Search(
    const std::string &text,
    std::map<std::string, std::vector<IndexLabelPairType>> &nodes) {
    int bufLength = 0;
    std::vector<UnicodeType> characters;
    SplitWord(text, bufLength, characters);
    int index = 0;

    TrieNode *temp = root;

    for (std::vector<UnicodeType>::iterator character = characters.begin();
         character != characters.end(); ++character) {
        while (!GetNext(temp, *character)) {
            temp = temp->fail;
        }

        temp = GetNext(temp, *character);
        // std::cout << "Debug->" << temp->word << "\t" <<temp->isMatched <<
        // std::endl;

        if (temp->isMatched) { // match, store matched results
            std::map<std::string, std::vector<IndexLabelPairType>>::iterator
                nodeFind = nodes.find(temp->word); // deduplication
            if (nodeFind == nodes.end()) {
                temp->termFreq = 1;
                temp->index = index + 1 - temp->wordLength;
                IndexLabelPairType res_tmp =
                    std::make_pair(temp->index, temp->label);
                std::vector<IndexLabelPairType> tmp_node = {res_tmp};
                nodes.insert(std::make_pair(temp->word, tmp_node));
                std::cout << "Not Found->index: " << temp->index << std::endl;
            } else {
                temp->termFreq = 1;
                temp->index = index + 1 - temp->wordLength;
                IndexLabelPairType res_tmp =
                    std::make_pair(temp->index, temp->label);
                std::cout << "Found->index: " << temp->index << std::endl;
                nodes[temp->word].push_back(res_tmp);
            }
        }
        ++index;
    }
}

// return all the matched nodes
void ACAutomaton::SearchLongest(
    const std::string &text,
    std::map<std::string, std::vector<IndexLabelPairType>> &nodes) {
    // int bufLength = 0;
    // std::vector<UnicodeType> characters;
    // SplitWord(text, bufLength, characters);
    // int index = 0;

    // TrieNode *temp = root;

    // for (std::vector<UnicodeType>::iterator character = characters.begin();
    //      character != characters.end(); ++character) {
    //     while (!GetNext(temp, *character)) {
    //         temp = temp->fail;
    //     }

    //     temp = GetNext(temp, *character);
    //     // std::cout << "Debug->" << temp->word << "\t" <<temp->isMatched <<
    //     // std::endl;

    //     if (temp->isMatched) { // match, store matched results
    //         std::map<std::string, std::vector<IndexLabelPairType>>::iterator
    //             nodeFind = nodes.find(temp->word); // deduplication
    //         if (nodeFind == nodes.end()) {
    //             temp->termFreq = 1;
    //             temp->index = index + 1 - temp->wordLength;
    //             IndexLabelPairType res_tmp =
    //                 std::make_pair(temp->index, temp->label);
    //             std::vector<IndexLabelPairType> tmp_node = {res_tmp};
    //             nodes.insert(std::make_pair(temp->word, tmp_node));
    //             std::cout << "Not Found->index: " << temp->index <<
    //             std::endl;
    //         } else {
    //             temp->termFreq = 1;
    //             temp->index = index + 1 - temp->wordLength;
    //             IndexLabelPairType res_tmp =
    //                 std::make_pair(temp->index, temp->label);
    //             std::cout << "Found->index: " << temp->index << std::endl;
    //             nodes[temp->word].push_back(res_tmp);
    //         }
    //     }
    //     ++index;
    // }
    Search(text, nodes);

    // Find the longest words
    std::map<std::string, std::vector<IndexLabelPairType>> clean_nodes = nodes;
    nodes.clear();
    // find the longest matching
    std::set<std::size_t> removed_idx; // store the index will be removed
    for (auto i = clean_nodes.begin(); i != clean_nodes.end(); ++i) {
        removed_idx.clear();
        for (auto j = std::next(i, 1); j != clean_nodes.end(); ++j) {
            // *Note*,we check whether the word was the longest rule decribe as
            // bellow, we assume have w1 and w2 1) w1 should be the substring of
            // the w2 2) the start index in the original sentence should be the
            // same both 1) and 2) ture we decide the w2 was the longest word
            // Rule1
            auto pos = j->first.find(i->first);
            if (pos != std::string::npos) {
                // whether the keywords start at the same position/index
                // eg: 打开王者de王者荣耀, 王者:index=2,王者荣耀:index=6, not
                // the same eg: 打开王者荣耀, 王者:index=2,王者荣耀:index=2, we
                // only choose the longest keyword
                // Rule 2
                // Get all the index from the second keywords vector
                std::set<int> index_set;
                for (auto p : j->second) {
                    index_set.insert(p.first);
                }
                for (auto ii : i->second) {
                    if (index_set.find(ii.first) != index_set.end()) {
                        removed_idx.insert(ii.first);
                    }
                }
            }
        }

        // remove duplicated index
        std::vector<IndexLabelPairType> keep_idx;
        for (auto k : i->second) {
            auto pos = removed_idx.find(k.first);
            if (pos == removed_idx.end()) {
                keep_idx.push_back(k);
            }
        }
        // if there left keywords then insert to the final result
        // other while all the keywords in i->first are not the longest keywords
        if (keep_idx.size() != 0) {
            nodes.insert(std::make_pair(i->first, keep_idx));
        }
    }
#ifdef DEBUG
    std::cout << "Before clean longest matching:\n";
    for (auto it = clean_nodes.begin(); it != clean_nodes.end(); ++it) {
        std::cout << it->first << "\tindex: ";
        for (std::size_t itt = 0; itt < it->second.size(); ++itt) {
            std::cout << it->second[itt].first << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "\nAfter clean longest matching: \n";
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        std::cout << it->first << "\tindex: ";
        for (std::size_t itt = 0; itt < it->second.size(); ++itt) {
            std::cout << it->second[itt].first << ",";
        }
        std::cout << std::endl;
    }
#endif
}

void ACAutomaton::ParseSearchWords(
    std::map<std::string, std::vector<IndexLabelPairType>> &nodes,
    std::vector<std::string> &keywords, std::vector<int> &indexes,
    std::vector<int> &labelIds) {
    keywords.clear();
    indexes.clear();
    labelIds.clear();
    for (std::map<std::string, std::vector<IndexLabelPairType>>::iterator iter =
             nodes.begin();
         iter != nodes.end(); ++iter) {
        for (std::size_t it = 0; it < iter->second.size(); ++it) {
            keywords.push_back(iter->first);
            indexes.push_back(iter->second[it].first);
            labelIds.push_back(iter->second[it].second);
        }
    }
}

} // namespace StringMatching
