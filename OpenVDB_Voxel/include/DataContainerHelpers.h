#pragma once
#include "../include/MyTypes.h"
#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <cstdint>
#include <unordered_map>
#include <sstream>
#include <regex>

namespace cppIOUtility {
    // --- small string utils ---
    static inline std::string ltrim(std::string s) { s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); })); return s; }
    static inline std::string rtrim(std::string s) { s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c); }).base(), s.end()); return s; }
    static inline std::string trim(std::string s) { return rtrim(ltrim(std::move(s))); }

    static inline bool starts_with(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
    }

    static inline bool split_kv(const std::string& line, std::string& key, std::string& value) {
        auto pos = line.find(':');
        if (pos == std::string::npos) return false;
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        return true;
    }

    // --- binary write helper  ---
    template<typename T>
    void write_binary(std::ofstream& out, const T& value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    void write_text_matrix(std::ofstream& out, const Tools::FloatMatrix& matrix);
    void write_text_mapping_table(std::ofstream& out, const std::unordered_map<std::string, int>& table);
    void write_indexed_vector_as_map(std::ofstream& out, const std::vector<std::vector<std::string>>& indexed_table);

    // --- binary read helper  ---
    template<typename T>
    bool read_binary(std::ifstream& in, T& value) {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
        return static_cast<bool>(in);
    }

    std::unordered_map<std::string, std::vector<std::string>> read_dat_sections(std::istream& in);
    bool read_text_matrix(std::vector<std::string> lines, Tools::FloatMatrix& out);
    bool read_text_mapping_table(const std::vector<std::string>& lines, std::unordered_map<std::string, int>& out);
    bool read_indexed_vector_as_map(const std::vector<std::string>& lines, std::vector<std::vector<std::string>>& out);

}//cppIOUtility
