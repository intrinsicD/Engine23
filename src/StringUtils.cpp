//
// Created by alex on 20.02.24.
//

#include "StringUtils.h"
#include <algorithm>
#include <vector>
#include <cstring>

namespace Bcg {

    // see https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    void String::Ltrim(std::string &s, char c) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [c](int ch) {
            return ch != c;
        }));
    }

    void String::Rtrim(std::string &s, char c) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [c](int ch) {
            return ch != c;
        }).base(), s.end());
    }

    void String::Trim(std::string &s, char c) {
        Ltrim(s, c);
        Rtrim(s, c);
    }

    bool String::Contains(const std::string &s, const std::string &substring) {
        if (s.find(substring) != std::string::npos) {
            return true;
        }
        return false;
    }

    std::string String::Join(const std::string &str1, const std::string &str2) {
        return std::string(str1 + str2);
    }

    std::string String::Join(const std::vector<std::string> &strs, const std::string &sep) {
        std::string result;
        for (const auto &str: strs) {
            result += str + sep;
        }
        return result;
    }

    std::vector<std::string> String::Join(const std::string &prep, const std::vector<std::string> &strs) {
        std::vector<std::string> result;
        result.reserve(strs.size());
        for (const auto &str: strs) {
            result.push_back(prep + str);
        }
        return result;
    }

    std::vector<std::string> String::Split(const std::string &str, const std::vector<std::string> &delims) {
        std::vector<std::string> tokens;
        size_t pos;
        std::string s = str;
        for (const auto &delimiter: delims) {
            while ((pos = s.find(delimiter)) != std::string::npos) {
                tokens.push_back(s.substr(0, pos));
                s.erase(0, pos + delimiter.length());
            }
        }
        if (!s.empty()) {
            tokens.push_back(s);
        }
        return tokens;
    }

    std::vector<std::string> String::Split(const std::string &str, const char *delim) {
        std::vector<std::string> delims(strlen(delim));
        for (size_t i = 0; i < delims.size(); ++i) {
            delims[i] = delim[i];
        }
        return Split(str, delims);
    }

    void String::RemoveChars(std::string &s, const std::string &chars) {
        s.erase(std::remove_if(s.begin(), s.end(), [&chars](const char &c) {
            return chars.find(c) != std::string::npos;
        }), s.end());
    }

    void String::RemoveNonDigits(std::string &s) {
        s.erase(std::remove_if(s.begin(), s.end(), [](const char &c) {
            return !std::isdigit(c);
        }), s.end());
    }
}