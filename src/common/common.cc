/*
 * @Author: yw.shi
 * @Date: 2020-07-16 11:17:14
 * @LastEditTime: 2020-11-17 10:01:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: usefull tools for data process
 */
 
#include "common.h"

// Get file list from given path
void GetFilenames(std::string path, std::vector<std::string> &files) {
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir= opendir(path.c_str()))) {
        perror(("Folder " + path + "doesn't exist!").c_str());
        return;
    }
    while((ptr = readdir(pDir)) != 0) {
        if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            // std::cout << ptr->d_name << std::endl;
            files.push_back(path+"/"+ptr->d_name);
        }
    }
    closedir(pDir);
}

// Only get the file name in the given path without path
void GetFileWithoutPath(std::string path, std::vector<std::string> &files) {
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir= opendir(path.c_str()))) {
        perror(("Folder " + path + " doesn't exist!").c_str());
        return;
    }
    while((ptr = readdir(pDir)) != 0) {
        if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            // std::cout << ptr->d_name << std::endl;
            files.push_back(ptr->d_name);
        }
    }
    closedir(pDir);
}


// String split with specific pattern
void SplitWord(const std::string &str, std::vector<std::string>& vec, const std::string& pattern) {
    std::string::size_type pos1, pos2;
	pos1 = 0;
	pos2 = str.find(pattern);
	while (std::string::npos != pos2) {
		vec.push_back(str.substr(pos1, pos2 - pos1));
		pos1 = pos2 + pattern.size();
		pos2 = str.find(pattern, pos1);
	}
	if (pos1 != str.length()) {
		vec.push_back(str.substr(pos1));
	}
}

// Parse arguments given from commond line
int ArgumentParse(char *str, int argc, char **argv) {
    int a;
	for (a = 1; a < argc; ++a) if (!strcmp(str, argv[a])) {
		if (a == argc - 1) {
			printf("Argument missing for %s\n", str);
			exit(1);
		}
		return a;
	}
	return -1;
}

wchar_t* MBCS2Unicode(wchar_t *buff, const char *str) {
    wchar_t* wp = buff;
    char* p = (char*)str;
    while(*p) {
        if(*p & 0x80) {
            *wp = *(wchar_t*)p;
            p++;
        } else {
            *wp = (wchar_t)*p;
        }
        wp++;
        p++;
    }
    *wp = 0x0000;
    return buff;
}

char* Unicode2MBCS(char *buff, const wchar_t *str) {
    wchar_t *wp = (wchar_t *)str;
    char *p = buff, *tmp;
    while(*wp) {
        tmp = (char *)wp;
        if(*wp & 0xFF00) {
            *p = *tmp;
            p++;
            tmp++;
            *p= *tmp;
            p++;
        } else {
            *p = *tmp;
            p++;
        }
        wp++;
    }
    *p = 0x00;
    return buff;
}


// Convert string to wstring based on wchar_t and char
std::wstring Str2Wstr(std::string const & src){
    std::wstring dest;
    //std::setlocale(LC_CTYPE, "");
    std::setlocale(LC_CTYPE, "en_US.utf8");
    size_t const wcs_len = mbstowcs(NULL, src.c_str(), 0);
    std::vector<wchar_t> tmp(wcs_len + 1);
    mbstowcs(&tmp[0], src.c_str(), src.size());
    dest.assign(tmp.begin(), tmp.end() - 1);
    return dest;
}

// Convert Wstring to string
std::string Wstr2Str(std::wstring const & src){
    std::string dest;
    std::setlocale(LC_CTYPE, "");
    size_t const mbs_len = wcstombs(NULL, src.c_str(), 0);
    std::vector<char> tmp(mbs_len + 1);
    wcstombs(&tmp[0], src.c_str(), tmp.size());
    dest.assign(tmp.begin(), tmp.end() - 1);
    return dest;
}
