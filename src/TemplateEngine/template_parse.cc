/*
 * @Author: your name
 * @Date: 2021-11-03 14:28:34
 * @LastEditTime: 2021-11-18 17:20:01
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\src\TemplateEngine\template_parse.cc
 */

#include "template_parse.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <fstream>

namespace TemplateEngine {

namespace bfs = boost::filesystem;
//
TemplateParse::~TemplateParse() {
    delete this->invIndexing_;
    delete this->pieceAhc_;
}

TemplateParse::TemplateParse(const std::string &data_dir) {
    bfs::path pobj(data_dir);
    if (bfs::exists(pobj)) {
        temp_dir_ = data_dir; // directory of template files
    } else {
        std::cout << "ERROR: template directory was not exists!" << std::endl;
    }

    // Initialzation containers
    // key:template_id, value: template sentence
    tid_to_templates_.clear();
    // key: template id, value: text part id list
    templateID_to_textpartsID_.clear();
    // key: textpart id, value: textpart detail info
    textpartID_to_info_.clear();
    // key: textpart string, value: textpart id
    textpart_to_id_.clear();
    // key: templateid, value: template detail info
    templateID_to_info_.clear();

    // Invert indexing instance
    this->invIndexing_ = new InvertedIndexing();
    // String matching instance
    this->pieceAhc_ = new StringMatching::ACAutomaton();
}

// Parse each line in template file
int TemplateParse::ParseTemplateLine(const std::string &line) {
    std::vector<std::string> split_arrs; //
    // remove white space
    boost::trim(line);
    // remove special characters created in windows system
    boost::trim_right_if(line, boost::is_any_of("\r\n"));
    // split by tab
    boost::split(split_arrs, line, boost::is_any_of("\t"),
                 boost::token_compress_on);
    if (split_arrs.size() != 4) {
        std::cout << "ERROR: temlate was not completed!\n";
        return -1;
    }

    //--------------------Part1 Parse infos for template--------------------//
    // Template info
    TempalteInfoType info;
    // extract intention/threshold/need_full_parse/text_pieces
    boost::trim(split_arrs[0]);
    // Template: intention
    info.intention = split_arrs[0];
    // Template: threshold
    info.threshold = boost::lexical_cast<double>(split_arrs[1]);
    boost::trim(split_arrs[2]);
    // Template: template keywords
    std::string template_str = split_arrs[2];
    // Template: is full parsed
    info.isfull_parsed = boost::lexical_cast<bool>(split_arrs[3]);

    // Insert all infomations to different parts
    // A) template id
    info.tid = tid_to_templates_.size();
    info.priority = info.tid + 1; // priority starts from 1,2,...
    tid_to_templates_.insert(std::make_pair(info.tid, template_str));

    // Make template id and template info as pair
    templateID_to_info_.insert(std::make_pair(info.tid, info));

    //--------------------Part2 Parse infos for textpieces--------------------//
    split_arrs.clear();
    boost::split(split_arrs, template_str, boost::is_any_of("#@@##"));
    std::vector<std::string> tstrs;
    boost::regex barcket_rx("(?<=\[).*?(?=\]) ");
    TextPieceInfoType txtInfo;
    std::vector<int> textpartIDs;
    textpartIDs.clear();
    for (auto &it : split_arrs) {
        tstrs.clear();
        boost::split(tstrs, it, boost::is_any_of("#@##"));
        if (tstrs.size() != 3) {
            std::cout << "ERROR: text parts format error in template: " << it
                      << std::endl;
            return -1;
        }
        // Text piece
        boost::trim(tstrs[0]);
        std::string text_piece = tstrs[0];
        // Position of text
        int position = boost::lexical_cast<int>(tstrs[1]);
        // Is required
        bool is_required = boost::lexical_cast<bool>(tstrs[2]);
        // Check wildchar or keywords or entity was correct
        std::vector<std::string> vecs_left;
        std::vector<std::string> vecs_right;
        boost::find_all(vecs_left, text_piece, boost::is_any_of("["));
        boost::find_all(vecs_right, text_piece, boost::is_any_of("["));
        if (vecs_left.size() != vecs_right.size()) {
            std::cout << "ERROR: wrong text piece format!\n";
            return -1;
        }
        // If text piece was normal text then do not extract content from
        // barcket
        // Extract content for all format like []
        std::vector<std::string> vecs;
        boost::find_all_regex(vecs, text_piece, barcket_rx);
        std::string word_txt = "";
        if (vecs.size() == 2) {
            // means there was wildchar after entity/keyword slot
            txtInfo.wildchar = vecs[1]; // wildchar
            word_txt = vecs[0];         // text
        } else if (vecs.size() == 1) {  // means no wildchar after slot
            word_txt = vecs[0];
        } else { // means this is a normal text
            word_txt = text_piece;
        }

        // Check the word type: sys dict, user dict, keyword, normal text
        std::vector<std::string> arrs;
        std::string real_text = "";
        boost::split(arrs, word_txt, boost::is_any_of(":"));
        if (arrs.size() == 2) {
            if (arrs[0] == "D") {
                real_text = arrs[1];        // real text word
                txtInfo.slot_type = "dict"; // entity or keywords
            } else if (arrs[0] == "W") {
                real_text = "";
                txtInfo.slot_type = "wildchar"; // wildchar for regrex
            } else {
                std::cout << "ERROR: slot type not recognitized!\n";
            }
            if (boost::starts_with(arrs[1], "sys_")) {
                txtInfo.word_type = "sys";
            } else if (boost::starts_with(arrs[1], "user_")) {
                txtInfo.word_type = "user";
            } else if (boost::starts_with(arrs[1], "kw_")) {
                txtInfo.word_type = "kw";
            }
        } else if (arrs.size() == 0) {
            real_text = word_txt;
            txtInfo.slot_type = "";
            txtInfo.word_type = "norm";
            std::cout << "ERROR: format error!\n";
        }

        // Combination other infos for text
        int pid = textpart_to_id_.size();
        txtInfo.pid = pid; // assign id for text piece
        txtInfo.text = real_text;
        txtInfo.index = position;
        txtInfo.is_required = is_required;
        // Insert word_txt and it's id to map
        textpart_to_id_.insert(std::make_pair(word_txt, pid));

        // Collect textpart ids
        textpartIDs.push_back(pid);
    } // end word piece parse

    // Make template id and it's textpart ids as pair
    templateID_to_textpartsID_.insert(std::make_pair(info.tid, textpartIDs));
    return 0;
}

// Read template file and parse template compents
int TemplateParse::LoadTemplates() {
    // Get all template txt in folder
    std::vector<std::string> tfiles; // template file list
    GetFileWithoutPath(temp_dir_, tfiles);

    // If no templete files then raise error
    if (tfiles.size() == 0) {
        std::cout << "ERROR: there are no template *.txt files in folder: "
                  << temp_dir_ << std::endl;
        return -1;
    }

    std::ifstream fin;
    std::string line("");
    std::vector<std::string> split_arrs;
    std::string tmp_str("");

    // Tranverse all files and parse
    for (auto iter = tfiles.begin(); iter != tfiles.end(); ++iter) {
        fin.open(temp_dir_ + "/" + *iter);
        if (fin) {
            while (getline(fin, line)) {
                // read one line and parse
                ParseTemplateLine(line);
            }
        }
    }

    return 0;
}

// Build inverted indexing and Automation
int TemplateParse::BuildIndexingAndAC() {

    std::vector<std::string> words;
    std::set<std::string> dictions;
    // Construct inverted indexing
    // key: template id(document id), value: word id list
    for (auto &it : templateID_to_textpartsID_) {
        // get word string
        words.clear();
        for (auto &wit : it.second) {
            auto p = textpartID_to_info_.find(wit);
            if (p != textpartID_to_info_.end()) {
                // find words string, and remove wildchar
                if (p->second.text == "") {
                    continue;
                }
                dictions.insert(p->second.text);
                words.push_back(p->second.text);
            }
        }
        // Add to inverted indexing table
        invIndexing_->AddDoc(words, it.first);
    }

    // Create String match ahc
    for (auto &it : dictions) {
        pieceAhc_->AddWord(it, 0);
    }
    pieceAhc_->Build();
}

// Extract text piece from query
int TemplateParse::ExtractTextParts(const std::string &query,
                                    std::vector<std::string> &words,
                                    std::vector<int> &indexes,
                                    std::vector<int> &labelIDs) {
    // trim
    std::string nquery = query;
    boost::trim(nquery);
    words.clear();
    indexes.clear();
    labelIDs.clear();
    if (nquery.size() == 0) {
        std::cout << "WARN: input query is empty!\n";
        return -1;
    }

    // Extract text pieces from query string
    pieceAhc_->SearchParse(query, words, indexes, labelIDs);

    return 0;
}

// Seach template id with text parts
int TemplateParse::SearchWithTextParts(std::vector<std::string> &text_parts,
                                       std::set<int> &tids, std::string mode) {
    // Use inverted index searching to recall template with contain text part
    invIndexing_->Search(text_parts, tids, mode);

    return 0;
}

// Template filter with rules
int TemplateParse::Filter(
    std::vector<int> &tids, std::vector<std::string> &pieces,
    std::vector<int> &pieces_indexes, std::vector<std::string> &query_entity,
    std::vector<std::string> &slot_types, std::vector<int> &slot_indexes,
    std::unordered_map<int, std::vector<std::string>> &tid_pieces_res) {

    // Rule1, check all the required piece appears in query
    // and whether the order of required pieces was the same of them
    // in query.
    // Logic, there are only text piece and slot_types in template,
    // that we only need check whether them in query
    // and firstly we need extract text pieces from query with pieceAhc_
    // instance then we extract entity with EntityExtractAhc_ to get the slot
    // types of entity, slot types contain user_xxx, sys_xxx, kw_xxx;
    // std::vector<std::string> candidate_vec;
    // candidate_vec.insert(candidate_vec.end(), pieces.begin(), pieces.end());
    // candidate_vec.insert(candidate_vec.end(), slot_types.begin(),
    //                      slot_types.end());

    // The index of pieces in template
    std::vector<int> pieces_index;
    // Pieces and it's position in template
    std::vector<std::pair<std::string, int>> tpieces_index;
    // Pieces and it's position in query
    std::vector<std::pair<std::string, int>> qpieces_index;
    // Template id and it's text pieces appears in query
    // Key: template id, value: text pieces appears in query
    // std::unordered_map<int, std::vector<std::string>> tid_pieces_res;
    // Temp pieces found in query result vector
    std::vector<std::string> pieces_res;

    for (auto &it : tids) {
        bool is_template_right = false;
        // Get all pieces with the specified template id
        auto p = templateID_to_textpartsID_.find(it);
        if (p == templateID_to_textpartsID_.end()) {
            // Template id not exist
            std::cout << "ERROR: template id: " << it << std::endl;
            continue;
        }
        pieces_index.clear();
        qpieces_index.clear();
        tpieces_index.clear();
        pieces_res.clear();
        // Check all text pieces of template appear in query
        // If exists then record the pieces for template confidence calculation
        for (auto &pit : p->second) {
            // Get text piece detail info with txtpart id in template pieces
            // maps
            auto info_iter = textpartID_to_info_.find(pit);
            if (info_iter == textpartID_to_info_.end()) {
                // text part not exist
                std::cout << "ERROR: textpart not exist id: " << pit
                          << std::endl;
                continue;
            }

            // Whether text pieces appears in query
            // Check in query pieces and slots types
            auto q_pos_pieces =
                std::find(pieces.begin(), pieces.end(), info_iter->second.text);
            auto q_pos_slot = std::find(slot_types.begin(), slot_types.end(),
                                        info_iter->second.text);
            bool not_found = ((q_pos_pieces == pieces.end()) &&
                              (q_pos_slot == slot_types.end()));
            // Not found in query slot types and query pieces, and the pieces
            // was required then pass this template
            if (not_found) {
                if (info_iter->second.is_required) {
                    // Template pass
                    is_template_right = false;
                    break;
                } else {
                    // Check next text piece
                    continue;
                }
            } else {
                // Found text pieces or slot type in query
                is_template_right = true;
                // Step a and b was collect for compare the order in template
                // and query
                // a) Store the index of text piece of template
                tpieces_index.push_back(std::make_pair(
                    info_iter->second.text, info_iter->second.index));

                // b) Get the index of text piece or slot_types in query
                if (q_pos_pieces != pieces.end()) {
                    // Found in query text pieces
                    // TODO, pieces or slot_type should sort by ascending
                    // Store the index of text piece of query
                    qpieces_index.push_back(std::make_pair(
                        *q_pos_pieces, (int)(q_pos_pieces - pieces.begin())));

                    // get piece text for query
                    pieces_res.push_back(*q_pos_pieces);
                } else if (q_pos_slot == slot_types.end()) {
                    // Found in query entity slot types
                    // Store the index of text piece of query slot types
                    int idx = (int)(q_pos_slot - slot_types.begin());
                    qpieces_index.push_back(std::make_pair(*q_pos_slot, idx));

                    // get enttiy keyword for query
                    pieces_res.push_back(query_entity[idx]);
                }
            }
        } // end check template pieces

        if (is_template_right) {
            bool is_satisfied = false;
            // TODO, check the order of pieces in template and query are the
            // same. Here we only check the index of them in piece vector(sorted
            // by position) and template piece vector(sorted by position)
            if ((!tpieces_index.empty()) &&
                (qpieces_index.size() == tpieces_index.size())) {
                // Descending sort for query pieces
                std::sort(qpieces_index.begin(), qpieces_index.end(),
                          sort_pair_second<std::string, int>());

                // Descending sort for template pieces
                std::sort(tpieces_index.begin(), tpieces_index.end(),
                          sort_pair_second<std::string, int>());
                for (int i = 0; i < qpieces_index.size(); ++i) {
                    if (qpieces_index[i].first != tpieces_index[i].first) {
                        is_satisfied = false;
                        break;
                    } else {
                        is_satisfied = true;
                    }
                }

                if (is_satisfied) {
                    tid_pieces_res.insert(std::make_pair(it, pieces_res));
                }
            } else {
                tid_pieces_res.insert(std::make_pair(it, pieces_res));
            }
        }
    } // end template piece check

    return 0;
}

// Calculate score between query and text pieces
double TemplateParse::CalculateScore(const std::string &query,
                                     std::vector<std::string> &pieces) {
    if (query.empty() || pieces.empty()) {
        return 0.0;
    }
    double recognized_word_length = 0.0;
    for (auto &it : pieces) {
        // is digital
        if (boost::all(it, boost::is_digit())) {
            recognized_word_length += 0.5;
        } else if (boost::all(it, boost::is_alpha())) {
            recognized_word_length += 0.5;
        } else if (boost::all(it, boost::is_alnum())) {
            recognized_word_length += 0.5;
        } else {
            // Count how many chinese word in string
            std::vector<UnicodeType> characters;
            utf8::utf8to16(it.begin(), it.end(),
                           std::back_inserter(characters));
            recognized_word_length += characters.size();
        }
    }
    std::vector<UnicodeType> characters;
    utf8::utf8to16(query.begin(), query.end(), std::back_inserter(characters));
    double score = (double)recognized_word_length / characters.size();

    return score;
}

// Calculate score for ever template and sort by socre
int TemplateParse::Rank(
    const std::string &query,
    std::unordered_map<int, std::vector<std::string>> tid_pieces_res,
    std::vector<std::pair<int, double>> &tid_score) {

    // Rule2, sort by the priority of template
    if (tid_pieces_res.empty()) {
        std::cout << "WARN: no correct template recall!\n";
        return 0;
    }
    // Get the priority of template, key: template id, value: template priority
    std::vector<std::pair<int, int>> priorities;
    for (auto &it : tid_pieces_res) {
        auto p = templateID_to_info_.find(it.first);
        if (p != templateID_to_info_.end()) {
            priorities.push_back(std::make_pair(it.first, p->second.priority));
        }
    }

    // Descending Sort by priority
    std::sort(priorities.begin(), priorities.end(),
              sort_pair_second<int, int>());

    // Calculate confidence for each template

    for (auto &it : priorities) {
        double score = CalculateScore(query, tid_pieces_res[it.first]);
        auto ths = templateID_to_info_[it.first];
        if (score >= ths.threshold) {
            tid_score.push_back(std::make_pair(it.first, score));
        }
    }
}

// Template search interface for application
int TemplateParse::TemplateSearch(
    const std::string &query,
    std::vector<std::pair<std::string, double>> &intents_score) {

    // 1) template piece extraction
    std::vector<std::string> template_pieces;
    std::vector<int> template_pieces_index;
    std::vector<int> template_pieces_labelIds;
    ExtractTextParts(query, template_pieces, template_pieces_index,
                     template_pieces_labelIds);

    // 2) extract entities with
    // TODO
    std::vector<std::string> query_entities;
    std::vector<int> query_entity_index;
    std::vector<int> query_entity_labelIds;
    std::vector<std::string> query_slot_types;

    // 3) Template Recall
    // Use template pieces recall
    std::set<int> recall_template_ids;
    SearchWithTextParts(template_pieces, recall_template_ids, "intersection");

    // Use query slot types recall
    std::set<int> tmp;
    SearchWithTextParts(query_slot_types, tmp, "intersection");

    recall_template_ids.insert(tmp.begin(), tmp.end());

    // 4) Fillter template ids
    std::vector<int> tids(recall_template_ids.begin(),
                          recall_template_ids.end());
    std::unordered_map<int, std::vector<std::string>> template_recall_res;
    Filter(tids, template_pieces, template_pieces_index, query_entities,
           query_slot_types, query_entity_index, template_recall_res);

    // 5) Rank
    std::vector<std::pair<int, double>> tids_score;
    Rank(query, template_recall_res, tids_score);

    // 6) Extract intions
    for (auto &it : tids_score) {
        auto pos = templateID_to_info_.find(it.first);
        if (pos != templateID_to_info_.end()) {
            intents_score.push_back(
                std::make_pair(pos->second.intention, it.second));
        } else {
            std::cout << "WARN: template id not exist!!!\n";
        }
    }

    return 0;
}

} // namespace TemplateEngine