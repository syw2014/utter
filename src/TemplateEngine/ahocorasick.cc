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

TrieNode::TrieNode():word(""),fail(0),isMatched(false),termFreq(0),wordLength(0), \
index(-1),label(-1) {
}

TrieNode::~TrieNode() {
    delete this->fail; 
}

// ACAutomaton class
// Construct
ACAutomaton::ACAutomaton() {
    this->root = new TrieNode;
}

// Destructor
ACAutomaton::~ACAutomaton() {
    delete this->root;
}

void ACAutomaton::SplitWord(const std::string &text, int &word_length, std::vector<UnicodeType> &characters) {

	std::string nstr = text;
	// Normalize text
	Normalize::ToUpper(nstr);
	bool valid = Normalize::IsValidUTF8(nstr);
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


TrieNode* ACAutomaton::GetNext(TrieNode *current, UnicodeType &character) {
    std::map<UnicodeType, TrieNode*>::iterator iter = current->next.find(character); // get next iterator
    if (iter != current->next.end()) {
        return iter->second;    // if found return trie node pointer
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
	// std::cout << "add test: " << pattern << " number of word: " << word_length  <<std::endl;

	TrieNode *tmp = this->root;
	// std::cout << "TEST" << std::endl;
	int i = 1;		// word length counter
	std::vector<UnicodeType> path_word;
	// UnicodeType path_word;
	for (std::vector<UnicodeType>::iterator iter = characters.begin(); iter != characters.end(); ++iter) {
		path_word.push_back(*iter);
		// std::cout <<"Debug->" << *iter << std::endl;
		std::map<UnicodeType, TrieNode*>::iterator item = tmp->next.find(*iter);	// check strings in node path
		if (item != tmp->next.end()) {
			tmp = item->second;		// if exist return trie node
		} else {						// if not exist then create new node in the graph or tree
			TrieNode *n = new TrieNode;
			// convert unicode to word
			std::string ww;
			bool f = Normalize::UnicodeToUTF8Str(path_word, ww);
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
	tmp->label = label;		// pattern label
}


void ACAutomaton::Build() {

	std::queue<TrieNode*> que;	// 

	// set all fail pointer as root of the node which depth=1, and add all the nodes into queue
	for (std::map<UnicodeType, TrieNode*>::iterator iter = root->next.begin(); iter != root->next.end(); ++iter) {
		iter->second->fail = root;
		que.push(iter->second);
	}

	instances.clear();
	//instances.push_back(root);

	while (!que.empty()) {
		TrieNode* temp = que.front();
		que.pop();

		// loop the next node of current node
		for (std::map<UnicodeType, TrieNode*>::iterator iter = temp->next.begin(); iter != temp->next.end(); ++iter) {
			UnicodeType character = iter->first;	// get the string

			// push the current node into queue
			que.push(iter->second);

			// set fail pointer, search from parent node
			TrieNode  *parent = temp->fail;
			while (!GetNext(parent, character)) parent = parent->fail;
			iter->second->fail = GetNext(parent, character);
			if (!GetNext(parent, character)) throw 1;
		}

		// store tail node
		if (temp->isMatched) {
			instances.push_back(temp);
		}
	}
}

bool ACAutomaton::IsMatch(const std::string &text){
    int buf_length = 0;
	std::vector<UnicodeType> characters;
	SplitWord(text, buf_length, characters);

	TrieNode *tmp = root;
	for (std::vector<UnicodeType>::iterator iter = characters.begin(); iter != characters.end(); ++iter) {
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
void ACAutomaton::Search(const std::string &text, std::map<std::string, TrieNode*> &nodes) {
	int bufLength = 0;
	std::vector<UnicodeType> characters;
	SplitWord(text, bufLength, characters);
	int index = 0;

	TrieNode *temp = root;
		 
	for (std::vector<UnicodeType>::iterator character = characters.begin(); character != characters.end(); ++character) {
		while (!GetNext(temp, *character)) {
			temp = temp->fail;
		}

		temp = GetNext(temp, *character);

		if (temp->isMatched) { // match
			std::map<std::string, TrieNode*>::iterator nodeFind = nodes.find(temp->word);
			if (nodeFind == nodes.end()) {
				temp->termFreq = 1;
				temp->index = index + 1 - temp->wordLength;
				nodes.insert(std::make_pair(temp->word, temp));
			} else {
				nodeFind->second->termFreq += 1;
			}
		}
		++index;
	}
}

// return all the matched nodes
void ACAutomaton::SearchLongest(const std::string &text, std::map<std::string, TrieNode*> &nodes) {
	int bufLength = 0;
	std::vector<UnicodeType> characters;
	SplitWord(text, bufLength, characters);
	int index = 0;

	TrieNode *temp = root;
		 
	for (std::vector<UnicodeType>::iterator character = characters.begin(); character != characters.end(); ++character) {
		while (!GetNext(temp, *character)) {
			temp = temp->fail;
		}

		temp = GetNext(temp, *character);
		// std::cout << "Debug->" << temp->word << "\t" <<temp->isMatched << std::endl;

		if (temp->isMatched) { // match, store matched results
			std::map<std::string, TrieNode*>::iterator nodeFind = nodes.find(temp->word); // deduplication
			if (nodeFind == nodes.end()) {
				temp->termFreq = 1;
				temp->index = index + 1 - temp->wordLength;
				nodes.insert(std::make_pair(temp->word, temp));
			} else {
				nodeFind->second->termFreq += 1;
			}
		}
		++index;
	}

	// keep the longest match string
	// TODO, optimized
	std::map<std::string, TrieNode*> clean_nodes = nodes;
	nodes.clear();
	// find the longest matching
	bool flag = false;
	for(auto i = clean_nodes.begin(); i != clean_nodes.end(); ++i) {
		for(auto j = std::next(i, 1); j != clean_nodes.end(); ++j) {
			if (i->first == j->first) {
				continue;
			}
			// TODO, fix bugs, the distance between i->first and j->first should > 1
			auto pos = j->first.find(i->first);
			if (pos != std::string::npos) {
				flag = true;
				break;
			}
		}
		if (!flag) {	// not find word in other words, means word i was the longest in string
			nodes.insert(std::make_pair(i->first, i->second));
		} else {	// found word i appears in other words,so not keep ,jump next word
			flag = false;
			continue;
		}
	}
#ifdef _DEBUG
	std::cout << "Before clean longest matching:\n";
	for(auto it = clean_nodes.begin(); it != clean_nodes.end(); ++it) {
		std::cout << it->first << ",";
	}
	std::cout << "After clean longest matching: \n";
	for(auto it = nodes.begin(); it != nodes.end(); ++it) {
		std::cout << it->first << ",";
	}
#endif
}
