/*
 * @Author: your name
 * @Date: 2021-10-14 10:37:16
 * @LastEditTime: 2021-10-14 14:07:36
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\unit_test.cpp
 */
/*************************************************************************
# File Name: entity_extract.cc
# Method: 
# Author: Jerry Shi
# Mail: jerryshi0110@gmail.com
# Created Time: 2020年07月16日  10:58:54
 ************************************************************************/
// #include "entity_extract.h"
// #include "slot_entity_extract.h"
// #include "inverted_index.h"
#include "ahocorasick.h"
#include <time.h>


void usage() {
	printf("Usage:\n");
	printf("\t ./exec-file -dict_dir [input dictionary path] -input [input document filename] \
            -output [output match result filename]\n");
}

int main(int argc, char* argv[]) {


	// TEST ahc keyword search for NER
	std::vector<std::string> keywords = {"王者荣耀","王者","天空之城","稻花香","哈利波特","悟","我想要","帮我","你好","您好"};
	std::vector<int> labels = {1, 1, 1, 2, 3, 4, 4, 0, 0, 0};
	ACAutomaton *ahc = new ACAutomaton();
	
	// add keywords to ahc
	for (int i = 0; i < keywords.size(); ++i) {
		ahc->AddWord(keywords[i], labels[i]);
	}

	// build automation
	ahc->Build();

	// test
	std::map<std::string, TrieNode*> nodes;

	std::vector<std::string> tstr1 = {"打开王者de王者荣耀", "打开王者荣耀", "打开王者", "帮我找下王者荣耀", "你好，王者","您好，我想要听稻花香", "帮我找含有悟的专辑，谢谢"};

	for (int i = 0; i < tstr1.size(); ++i) {
		nodes.clear();
		// ahc->Search(tstr1[i], nodes);
		ahc->SearchLongest(tstr1[i], nodes);

		// print search results
		std::cout << "Input sentence: " << tstr1[i] << std::endl;
			// parse search results
		for (std::map<std::string, TrieNode*>::iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
			std::cout << "Keywords: " << iter->second->word << "\tindex: " << iter->second->index <<
				"\tlabel id: " << iter->second->label << std::endl;
		}
		std::cout << "\n-------------------------------------------\n";
	}



	return 0;
}



