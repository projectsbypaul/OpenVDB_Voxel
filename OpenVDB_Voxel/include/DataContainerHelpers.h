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

#include <filesystem>
namespace fs = std::filesystem;


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
    void write_int_vector(std::ofstream& out, const std::vector<int> int_vec);
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
    bool read_int_vector(std::vector<std::string> lines, std::vector<int>& out);
    bool read_text_matrix(std::vector<std::string> lines, Tools::FloatMatrix& out);
    bool read_text_mapping_table(const std::vector<std::string>& lines, std::unordered_map<std::string, int>& out);
    bool read_indexed_vector_as_map(const std::vector<std::string>& lines, std::vector<std::vector<std::string>>& out);

    // ----helpers----------------------------------------------------
    template <typename T> constexpr uint8_t element_type_id();
    template <> constexpr uint8_t element_type_id<float>() { return 0; }
    template <> constexpr uint8_t element_type_id<double>() { return 1; }
    template <> constexpr uint8_t element_type_id<int>() { return 2; }

    //generic reader
    template <typename T>
    static void dump_list_of_3d_arrays_bin(
        const fs::path& bin_file_path,
        const std::vector<std::vector<std::vector<std::vector<T>>>>& container)
    {
        if (container.empty()) {
            std::cout << "Container is empty, skipping binary file generation.\n";
            return;
        }

        std::ofstream bin_out(bin_file_path, std::ios::binary);
        if (!bin_out.is_open()) {
            std::cerr << "Error: Could not open .bin file for writing: "
                << bin_file_path.string() << '\n';
            return;
        }

        static_assert(std::is_trivially_copyable_v<T>,
            "T must be trivially copyable for raw write.");

        const uint32_t magic_number = 0x5345474D; // 'SEGM'
        const uint16_t format_version = 1;
        const uint8_t  etid = element_type_id<T>();
        const uint32_t num_segments = static_cast<uint32_t>(container.size());

        // header
        write_binary(bin_out, magic_number);
        write_binary(bin_out, format_version);
        write_binary(bin_out, etid);
        write_binary(bin_out, num_segments);

        // shapes (d0,d1,d2) for each 3D array
        for (const auto& seg : container) {
            const uint32_t d0 = static_cast<uint32_t>(seg.size());
            const uint32_t d1 = static_cast<uint32_t>((d0 && !seg[0].empty()) ? seg[0].size() : 0);
            const uint32_t d2 = static_cast<uint32_t>((d0 && d1 && !seg[0][0].empty()) ? seg[0][0].size() : 0);
            write_binary(bin_out, d0); write_binary(bin_out, d1); write_binary(bin_out, d2);
        }

        // payloads
        for (const auto& seg : container) {
            for (const auto& plane : seg) {
                for (const auto& row : plane) {
                    if (!row.empty()) {
                        bin_out.write(reinterpret_cast<const char*>(row.data()),
                            row.size() * sizeof(T));
                    }
                }
            }
        }

        bin_out.close();
        std::cout << "Dumped to: " << bin_file_path.string() << '\n';
    }
    //generic loader
    template <typename T>
    static bool load_list_of_3d_arrays_bin(
        const fs::path& bin_file_path,
        std::vector<std::vector<std::vector<std::vector<T>>>>& out_container)
    {
        std::ifstream bin_in(bin_file_path, std::ios::binary);
        if (!bin_in.is_open()) {
            std::cerr << "Error: Could not open .bin for reading: " << bin_file_path.string() << "\n";
            return false;
        }

        const uint32_t expected_magic = 0x5345474D; // 'SEGM'
        uint32_t magic = 0;
        uint16_t version = 0;
        uint8_t  etid = 0;
        uint32_t num_segments = 0;

        if (!read_binary(bin_in, magic) || magic != expected_magic) {
            std::cerr << "Error: Bad magic in bin file.\n";
            return false;
        }
        if (!read_binary(bin_in, version) || version != 1) {
            std::cerr << "Error: Unsupported version " << version << ".\n";
            return false;
        }
        if (!read_binary(bin_in, etid) || etid != element_type_id<T>()) {
            std::cerr << "Error: Unexpected element type id. Expected "
                << int(element_type_id<T>()) << ", got " << int(etid) << ".\n";
            return false;
        }
        if (!read_binary(bin_in, num_segments)) {
            std::cerr << "Error: Failed reading num_segments.\n";
            return false;
        }

        // Read shapes
        struct Shape { uint32_t d0 = 0, d1 = 0, d2 = 0; };
        std::vector<Shape> shapes(num_segments);
        for (uint32_t s = 0; s < num_segments; ++s) {
            if (!read_binary(bin_in, shapes[s].d0) ||
                !read_binary(bin_in, shapes[s].d1) ||
                !read_binary(bin_in, shapes[s].d2)) {
                std::cerr << "Error: Failed reading shape for segment " << s << ".\n";
                return false;
            }
        }

        // Allocate + read data
        out_container.clear();
        out_container.resize(num_segments);
        for (uint32_t s = 0; s < num_segments; ++s) {
            auto& seg = out_container[s];
            seg.resize(shapes[s].d0);
            for (uint32_t p = 0; p < shapes[s].d0; ++p) {
                auto& plane = seg[p];
                plane.resize(shapes[s].d1);
                for (uint32_t r = 0; r < shapes[s].d1; ++r) {
                    auto& row = plane[r];
                    row.resize(shapes[s].d2);
                    if (shapes[s].d2 == 0) continue;
                    std::size_t bytes = sizeof(T) * shapes[s].d2;
                    if (!bin_in.read(reinterpret_cast<char*>(row.data()), bytes)) {
                        std::cerr << "Error: Unexpected EOF at segment " << s
                            << ", plane " << p << ", row " << r << ".\n";
                        return false;
                    }
                }
            }
        }

        return true;
    }

}//cppIOUtility
