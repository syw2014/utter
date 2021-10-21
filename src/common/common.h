/*
 * @Author: yw.shi
 * @Date: 2020-07-16 11:48:36
 * @LastEditTime: 2020-08-17 08:45:27
 * @LastEditors: Please set LastEditors
 * @Description: Common contains usefull tools for string parse, parameters parse etc
 * @FilePath: \dialog-system\engine\thirdparty\ahocorasick\common.h
 */
// Common contains usefull tools for string parse, parameters parse etc
#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <string.h>
#include <locale.h> 



// Get file list from given path
void GetFilenames(std::string path, std::vector<std::string> &files);

// Only get the file name in the given path without path
void GetFileWithoutPath(std::string path, std::vector<std::string> &files);

// String split with specific pattern
void SplitWord(const std::string &str, std::vector<std::string>& vec, const std::string& pattern);

// Parse arguments given from commond line
int ArgumentParse(char *str, int argc, char **argv);


// Encode transform for chinese character(unicode)
wchar_t* MBCS2Unicode(wchar_t *buff, const char *str);

// Convert Unicode encode to MBCS(multi-byte characters set)
char* Unicode2MBCS(char *buff, const wchar_t* str);

// Convert string to wstring
std::wstring Str2Wstr(std::string const & src);

// Convert wstring to string
std::string Wstr2Str(std::wstring const & src);


#endif // end common.h
