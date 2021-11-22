/*
 * @Author: your name
 * @Date: 2021-11-01 11:14:22
 * @LastEditTime: 2021-11-18 16:45:22
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\TemplateEngine\templ_parsed.h
 */
#ifndef TEMPLATE_PARSED_H_
#define TEMPLATE_PARSED_H_

#include "ahocorasick.h"
#include "common.h"
#include "inverted_index.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

// Template Pair sort by second desceding sort
template <class T1, class T2, class Pred = std::greater<T2>>
struct sort_pair_second {
    bool operator()(const std::pair<T1, T2> &left,
                    const std::pair<T1, T2> &right) {
        Pred p;
        return p(left.second, right.second);
    }
};

namespace TemplateEngine {

// Template infomation struct type, contain : intention, template_id, threshold,
// is_full_parsed, priority
typedef struct TempalteInfo {
    std::string intention; // intention the currect template belong to
    int tid;               // template id
    double threshold;      // template filter threshold
    int isfull_parsed;     // entity in query need to be full parsed
    int priority; // the priority of this template in the current intention

} TempalteInfoType;

// Text piece information for one text/keyword part, contian:
// *pid*: id of text piece
// *position*: position of text piece in template string, can be 0 or real index
// in string.
// *slot_type*: type of slot, there are only two D and W, D(dict): means
// this word from dictionary may entity or keyword, W(wildchar): means this was
// a wildcard, format like: [W:number-number], how many characters.
// *word_type*: means what type of this word, there are four types here, sys:
// system dictionary, user: user dictionary, kw: keyword dictionary, norm:
// normal text *wildchar*: what is the wildchar after slot, if no it will be "";
// *is_required*: whether the keywords is required.
// *text*: the real text of word piece
typedef struct TextPieceInfo {
    std::string text;
    int pid;
    int index;
    std::string slot_type;
    std::string word_type;
    std::string wildchar;
    bool is_required;
} TextPieceInfoType;

class TemplateParse {
  public:
    // Key: template id, value: textpart id list in this template
    std::unordered_map<int, std::vector<int>> templateID_to_textpartsID_;
    // Key: textpart id, value: text piece infos
    std::unordered_map<int, TextPieceInfoType> textpartID_to_info_;
    // Key:template_id, value: template whole sentence
    std::unordered_map<int, std::string> tid_to_templates_;
    // Key: word, value: textpartID
    std::unordered_map<std::string, int> textpart_to_id_;
    // Key: template id, value: template infos
    std::unordered_map<int, TempalteInfoType> templateID_to_info_;

    // InvertedIndex
    InvertedIndexing *invIndexing_;
    // Text piece search instance
    StringMatching::ACAutomaton *pieceAhc_;

  private:
    std::string temp_dir_; // template directory

  public:
    TemplateParse(const std::string &data_dir);
    ~TemplateParse();

    // Load tempalte from file
    int LoadTemplates();

    // Template line parse
    int ParseTemplateLine(const std::string &line);

    // Construct inverted indexing and automation
    int BuildIndexingAndAC(){};

    // Convert query to parameter format with slot type
    int QueryParametic(const std::string &query,
                       std::vector<std::string> entities,
                       std::vector<int> &indexes,
                       std::vector<std::string> &slot_types);

    // Extract text piece from query
    int ExtractTextParts(const std::string &query,
                         std::vector<std::string> &words,
                         std::vector<int> &indexes, std::vector<int> &labelIDs);

    // Search template based on inverted search index
    int SearchWithTextParts(std::vector<std::string> &text_parts,
                            std::set<int> &tids, std::string mode);

    // Template filter based on rules
    int
    Filter(std::vector<int> &tids, std::vector<std::string> &pieces,
           std::vector<int> &pieces_indexes,
           std::vector<std::string> &query_entity,
           std::vector<std::string> &slot_types, std::vector<int> &slot_indexes,
           std::unordered_map<int, std::vector<std::string>> &tid_pieces_res);

    // Template rank based on confidence between query and template
    int Rank(const std::string &query,
             std::unordered_map<int, std::vector<std::string>> tid_pieces_res,
             std::vector<std::pair<int, double>> &tid_score);

    // Calculate confidence
    double CalculateScore(const std::string &query,
                          std::vector<std::string> &pieces);

    // Template Search interface
    // Input query ,return intention search result and score
    int
    TemplateSearch(const std::string &query,
                   std::vector<std::pair<std::string, double>> &intents_score);

}; // TemplateParse class

} // namespace TemplateEngine

#endif // tempalte_parsed.h
