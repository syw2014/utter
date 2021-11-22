/*
 * @Author: your name
 * @Date: 2021-11-03 13:56:27
 * @LastEditTime: 2021-11-10 14:49:55
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\TemplateEngine\inverted_index.h
 */
// An implementation of inverted index search engine, which can be used in many
// projects in NLP, here we use it to search for template ids based on input
// query(text pieces).

#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_h

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

// namespace TemplateEngine
namespace TemplateEngine {

class InvertedIndexing {
  public:
    // inverted index map, key:word, value: text id list which text contain this
    // word
    std::map<std::string, std::set<int>> invert_indexing;
    std::map<int, std::vector<std::string>>
        docid_to_words;           // doc id to word list
    std::set<std::string> vocabs; // contain all words

  public:
    InvertedIndexing();

    ~InvertedIndexing();

    // Add one document into inverted indexing
    void AddDoc(std::vector<std::string> &token_list, int id = -1);

    // Given word list return all document ids which contain those word
    void Search(std::vector<std::string> &token_list, std::set<int> &docids,
                std::string mode = "union");

    // Get document text given document id
    void GetDocText(int id, std::vector<std::string> &token_list);

    // Serialize data
    void Save();

    // De-serialize data for fast load and use
    void Load();

}; // end InvertedIndexing class

#endif // inverted_index.h
} // namespace TemplateEngine
