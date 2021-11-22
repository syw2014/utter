/*
 * @Author: your name
 * @Date: 2021-11-03 14:16:12
 * @LastEditTime: 2021-11-10 14:49:20
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\TemplateEngine\inverted_indexing.cc
 */
#include "inverted_index.h"

namespace TemplateEngine {
InvertedIndexing::InvertedIndexing() {
    invert_indexing.clear();
    docid_to_words.clear();
    vocabs.clear();
}

InvertedIndexing::~InvertedIndexing() {}

// Add one document into inverted indexing
void InvertedIndexing::AddDoc(std::vector<std::string> &token_list, int id) {
    if (id == -1) {
        id = docid_to_words.size(); // assign id for doc
    }

    // keep doc_id and token list
    docid_to_words.insert(std::make_pair(id, token_list));
    for (auto &iter : token_list) {
        if (vocabs.find(iter) == vocabs.end()) {
            // if word not in vocabs then insert
            vocabs.insert(iter);
        }

        // insert word to indexing table
        auto pos = invert_indexing.find(iter);
        if (pos != invert_indexing.end()) { // word exists
            // add doc id into doc list
            pos->second.insert(id);
        } else {
            std::set<int> doc_id = {id};
            // insert <word, doc_id_list> into
            invert_indexing.insert(std::make_pair(iter, doc_id));
        }
    }
    // std::cout << "After ADD total " <<  vocabs.size() << "words and " <<
    // docid_to_words.size()
    //         << " docs in inverted indexing tabel.\n";
}

// Given word list return all document ids which contain those word
void InvertedIndexing::Search(std::vector<std::string> &token_list,
                              std::set<int> &docids, std::string mode) {
    docids.clear();
    std::vector<std::set<int>> results;
    results.clear();
    for (auto &iter : token_list) {
        // TODO, currently ,we use the keyword full match, we also can
        // optimizated it with distance or similarity.
        // find word in inverted indexing table
        auto pos = invert_indexing.find(iter);
        if (pos != invert_indexing.end()) {
            // found word, keep doc ids
            results.push_back(pos->second);
        }
    }
    if (results.size() != (std::size_t)0) {
        if (mode == "union") {
            // Recall all the docids
            for (auto &it : results) {
                if (it != docids) {
                    docids.insert(it.begin(), it.end());
                }
            }
        } else if (mode == "intersection") {
            docids = results[0];
            std::sort(docids.begin(), docids.end());
            std::set<int> temp;
            for (int i = 1; i < int(results.size()); ++i) {
                temp.clear();
                std::sort(results[i].begin(), results[i].end());
                std::set_intersection(docids.begin(), docids.end(),
                                      results[i].begin(), results[i].end(),
                                      inserter(temp, temp.begin()));
                if (temp.size() == (std::size_t)0) {
                    // std::cout << "Inverted Indexing found none intersection
                    // document ids for input!!" << std::endl;
                    break;
                }
                docids.swap(temp);
            }
        } else {
            std::cout << "Unknown mode for inverted indexing search results "
                         "process!\n";
        }
    }
}

// Get document text given document id
void InvertedIndexing::GetDocText(int id,
                                  std::vector<std::string> &token_list) {
    auto pos = docid_to_words.find(id);
    if (pos != docid_to_words.end()) {
        token_list = pos->second;
    }
}

// Serialize data
void InvertedIndexing::Save() {}

// De-serialize data for fast load and use
void InvertedIndexing::Load() {}

} // namespace TemplateEngine