#include "../include/DataContainerHelpers.h"
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

    // Text writing helper for matrices (remains the same)
    void write_text_matrix(std::ofstream& out, const Tools::FloatMatrix& matrix) {
        // ... (implementation as before) ...
        if (matrix.empty()) {
            out << "rows: 0\n";
            out << "cols: 0\n";
            return;
        }
        out << "rows: " << matrix.size() << "\n";
        out << "cols: " << (matrix[0].empty() ? 0 : matrix[0].size()) << "\n";
        for (const auto& row : matrix) {
            for (size_t i = 0; i < row.size(); ++i) {
                out << row[i] << (i == row.size() - 1 ? "" : " ");
            }
            out << "\n";
        }
    }

    // --- write_text_mapping_table overloads ---

    // 1. For TypeCount_ (std::unordered_map<std::string, int>)
    void write_text_mapping_table(std::ofstream& out, const std::unordered_map<std::string, int>& table) {
        out << "count: " << table.size() << "\n";
        for (const auto& pair : table) {
            out << pair.first << ": " << pair.second << "\n";
        }
    }

    // 2. *** REVISED function for FaceTypeMap_ and VertTypeMap_ ***
    //    Input: std::vector<std::vector<std::string>> where outer index is the key (face/vertex ID)
    //           and inner vector is the list of types for that ID.
    //    Output format: "ID: type1,type2,type3"
    void write_indexed_vector_as_map(std::ofstream& out, const std::vector<std::vector<std::string>>& indexed_table) {
        // The "count" is the number of indices (vertices/faces) for which we are providing information.
        // This corresponds to the size of the outer vector, which was pre-sized or resized.
        out << "count: " << indexed_table.size() << "\n";

        for (size_t index = 0; index < indexed_table.size(); ++index) {
            const std::vector<std::string>& types = indexed_table[index]; // This is the list of types for the current index

            out << index << ": "; // The key is the numeric index itself

            for (size_t i = 0; i < types.size(); ++i) {
                out << types[i];
                if (i < types.size() - 1) {
                    out << ","; // Comma-separated types
                }
            }
            out << "\n"; // Newline after each index and its types (e.g., "0: typeA,typeB" or "1: ")
        }
    }

    // --- parse [SECTION] ... [END_SECTION] blocks into a map ---
    std::unordered_map<std::string, std::vector<std::string>>
    read_dat_sections(std::istream& in)
    {
        std::unordered_map<std::string, std::vector<std::string>> sections;
        std::string line, current;
        std::regex begin_re(R"(^\[(.+)\]\s*$)");
        std::regex end_re(R"(^\[END_(.+)\]\s*$)");
        while (std::getline(in, line)) {
            std::smatch m;
            if (std::regex_match(line, m, begin_re)) {
                current = m[1].str();
                sections[current]; // create
                continue;
            }
            if (!current.empty() && std::regex_match(line, m, end_re)) {
                // sanity: m[1] should match current, but don't hard fail
                current.clear();
                continue;
            }
            if (!current.empty()) {
                sections[current].push_back(line);
            }
        }
        return sections;
    }

    // --- read matrix written by write_text_matrix ---
    bool read_text_matrix(std::vector<std::string> lines, Tools::FloatMatrix& out)
    {
        // Expect:
        // rows: N
        // cols: M
        // N lines of M space-separated floats
        if (lines.size() < 2) return false;
        size_t rows = 0, cols = 0;
        {
            std::string k, v;
            if (!split_kv(lines[0], k, v) || k != "rows") return false;
            rows = static_cast<size_t>(std::stoul(v));
        }
        {
            std::string k, v;
            if (!split_kv(lines[1], k, v) || k != "cols") return false;
            cols = static_cast<size_t>(std::stoul(v));
        }
        out.clear();
        if (rows == 0 || cols == 0) return true; // empty ok

        if (lines.size() < 2 + rows) return false;
        out.resize(rows, std::vector<float>(cols, 0.0f));
        for (size_t r = 0; r < rows; ++r) {
            std::istringstream iss(lines[2 + r]);
            for (size_t c = 0; c < cols; ++c) {
                if (!(iss >> out[r][c])) return false;
            }
        }
        return true;
    }

    // --- read TypeCount_ map: first line "count: N", then "key: int" N lines (order free) ---
    bool read_text_mapping_table(const std::vector<std::string>& lines, std::unordered_map<std::string, int>& out)
    {
        out.clear();
        if (lines.empty()) return false;
        std::string k, v;
        if (!split_kv(lines[0], k, v) || k != "count") return false;
        size_t count = static_cast<size_t>(std::stoul(v));
        for (size_t i = 1; i < lines.size(); ++i) {
            if (trim(lines[i]).empty()) continue;
            std::string key, val;
            if (!split_kv(lines[i], key, val)) continue;
            try {
                int n = std::stoi(val);
                out[key] = n;
            }
            catch (...) {
                // ignore malformed
            }
        }
        // don't hard-enforce count; just best-effort
        (void)count;
        return true;
    }

    // --- read Face/Vert type maps written by write_indexed_vector_as_map ---
    bool read_indexed_vector_as_map(const std::vector<std::string>& lines, std::vector<std::vector<std::string>>& out)
    {
        out.clear();
        if (lines.empty()) return false;
        std::string k, v;
        if (!split_kv(lines[0], k, v) || k != "count") return false;
        size_t count = static_cast<size_t>(std::stoul(v));
        out.resize(count);

        for (size_t i = 1; i < lines.size(); ++i) {
            auto line = trim(lines[i]);
            if (line.empty()) continue;
            // "index: a,b,c"
            std::string key, value;
            if (!split_kv(line, key, value)) continue;
            size_t idx = 0;
            try {
                idx = static_cast<size_t>(std::stoull(key));
            }
            catch (...) {
                continue;
            }
            if (idx >= out.size()) continue;
            // split by comma (allow empty -> zero items)
            std::vector<std::string> types;
            std::stringstream ss(value);
            std::string tok;
            while (std::getline(ss, tok, ',')) {
                tok = trim(tok);
                if (!tok.empty()) types.push_back(tok);
            }
            out[idx] = std::move(types);
        }
        return true;
    }

}//cppIOUtility