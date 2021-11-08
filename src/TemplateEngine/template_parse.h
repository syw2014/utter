/*
 * @Author: your name
 * @Date: 2021-11-01 11:14:22
 * @LastEditTime: 2021-11-08 17:56:45
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\TemplateEngine\templ_parsed.h
 */
#ifndef TEMPLATE_PARSED_H_
#define TEMPLATE_PARSED_H_

#include "common.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

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
// *slot_type*: type of slot, there are only two D and W, D: means
// this word from dictionary may entity or keyword, W: means this was a
// wildcard, format like: [W:number-number], how many characters.
// *word_type*: means what type of this word, there are three types here,
// sys: system dictionary, user: user dictionary, kw: keyword dictionary
// *is_required*: whether the keywords is required.

typedef struct TextPieceInfo {
    int pid;
    int index;
    std::string slot_type;
    std::string word_type;
    bool is_required;
} TextPieceInfoType;

class TemplateParse {
  public:
    std::unordered_map<int, std::string>
        tid_to_templates_; // key:template_id, value: template sentence
    std::unordered_map<std::string, std::vector<TempalteInfoType>>
        intent_to_template_info_; // key: intention, value:template list
    std::unordered_map<int, std::string>
        templeteID_to_intention_; // key:template_id, intention
    std::unordered_map<std::string, int>
        textpart_to_id_; // key: sentence pieces, value: textpart id
    std::unordered_map<int, std::vector<TextPieceInfoType>>
        textpartID_to_info_; // key:sentence piece id, value: text piece info
    std::unordered_map<int, std::vector<int>>
        templateID_to_textparts_; // key: template id, value: fragment id list

  private:
    std::string temp_dir_; // template directory

  public:
    TemplateParse(const std::string &data_dir);
    ~TemplateParse();

    // Load tempalte from file
    int LoadTemplates();

    // Template line parse
    int ParseTemplateLine(const std::string &line);

    // Convert query to parameters based on keywords type and position
    int QueryParametric(const std::string &query,
                        std::vector<std::string> &keywords,
                        std::vector<int> &type_ids, std::vector<int> &indexes);

    // Cut template into pieces
    int Segment(const std::string &tstring,
                std::vector<std::string> &text_parts);

    // Search template based on inverted search index
    int Search(std::vector<std::string> &text_parts, std::vector<int> &tids);

    // Template filter based on rules
    int Filter(std::vector<int> &tids);

    // Template rank based on confidence between query and template
    int Rank(const std::string &, std::vector<int> &tids);

}; // TemplateParse class
} // namespace TemplateEngine

#endif // tempalte_parsed.h
