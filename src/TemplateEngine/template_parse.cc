/*
 * @Author: your name
 * @Date: 2021-11-03 14:28:34
 * @LastEditTime: 2021-11-08 17:37:32
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
TemplateParse::~TemplateParse() {}

TemplateParse::TemplateParse(const std::string &data_dir) {
    bfs::path pobj(data_dir);
    if (bfs::exists(pobj)) {
        temp_dir_ = data_dir; // directory of template files
    } else {
        std::cout << "ERROR: template directory was not exists!" << std::endl;
    }

    // Initialzation containers
    tid_to_templates_.clear(); // key:template_id, value: template sentence
    intent_to_template_info_.clear(); // key: intention, value:template list
    templeteID_to_intention_.clear(); // key:template_id, intention
    textpart_to_id_.clear();          // key: sentence pieces, value: id
    textpartID_to_info_.clear();      // key:sentence piece id, value:
                                      // vector[tempalte_id, index, is_need]
    templateID_to_textparts_
        .clear(); // key: template id, value: fragment id list
}

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

    // Insert the current template into intention section
    auto pos = intent_to_template_info_.find(info.intention);
    if (pos != intent_to_template_info_.end()) {
        pos->second.push_back(info);
    }
    //--------------------Part2 Parse infos for textpieces--------------------//
    split_arrs.clear();
    boost::split(split_arrs, template_str, boost::is_any_of("#@@##"));
    std::vector<std::string> tstrs;
    boost::regex barcket_rx("(?<=\[).*?(?=\]) ");

    for (auto &it : split_arrs) {
        tstrs.clear();
        TextPieceInfoType txt_info;
        boost::split(tstrs, it, boost::is_any_of("#@##"));
        if (tstrs.size() != 3) {
            std::cout << "ERROR: text parts format error in template: " << it
                      << std::endl;
        }
        // Keyword text
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
        }
        // Extract content for all format like []
        std::vector<std::string> vecs;
        boost::find_all_regex(vecs, text_piece, barcket_rx);
    }
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
    }

    std::ifstream fin;
    std::string line("");
    std::vector<std::string> split_arrs;
    std::string tmp_str("");

    // Tranverse all files and parse
    for (auto iter = tfiles.begin(); iter != tfiles.end(); ++iter) {
        fin.open(temp_dir_ + "/" + *iter);
        if (fin) {
            while (getline(fin, line)) { // read one line and parse
            }
        }
    }
}

} // namespace TemplateEngine