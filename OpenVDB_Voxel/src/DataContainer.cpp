
#include "../include/DataContainer.h"
#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <cstdint>
#include <unordered_map>
// No need for <sstream> for this simple comma joining

// Helper for writing fundamental types in binary (remains the same)
template<typename T>
void write_binary(std::ofstream& out, const T& value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

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


namespace cppIOUtility {

    GenericDataContainer::GenericDataContainer() {}

    SegmentationDataContainer::SegmentationDataContainer()
        : background_(0.0),
        voxel_size_(0.0),
        minVal_(0.0) {
        // Members like FaceTypeMap_ (std::vector<std::vector<std::string>>)
        // and TypeCount_ (std::unordered_map<std::string, int>)
        // will be default constructed.
    }

    void SegmentationDataContainer::dump(const fs::path& DumpDir /* = "" */) {
        // ... (path setup logic as before) ...
        fs::path base_path;
        std::string base_filename_stem = "segmentation_data";

        if (DumpDir.empty()) {
            base_path = fs::current_path();
        }
        else {
            if (!fs::exists(DumpDir) && !DumpDir.has_extension() && !DumpDir.stem().empty()) { // Check if it looks like a dir name
                try { fs::create_directories(DumpDir); }
                catch (const std::exception& e) { std::cerr << "Error creating directory " << DumpDir.string() << ": " << e.what() << std::endl; return; }
            }

            if (fs::is_directory(DumpDir)) {
                base_path = DumpDir;
            }
            else { // Assume DumpDir is a full path to a file or just a base name
                base_path = DumpDir.parent_path();
                if (!DumpDir.stem().empty()) { // Use stem if DumpDir was path/to/file.ext or just filename
                    base_filename_stem = DumpDir.stem().string();
                }
                if (base_path.empty()) base_path = fs::current_path(); // If DumpDir was just "filename"

                if (!fs::exists(base_path)) {
                    try { fs::create_directories(base_path); }
                    catch (const std::exception& e) { std::cerr << "Error creating directory " << base_path.string() << ": " << e.what() << std::endl; return; }
                }
            }
        }

        fs::path dat_file_path = base_path / (base_filename_stem + ".dat");
        std::string segment_bin_filename_str = base_filename_stem + "_segments.bin";
        fs::path bin_file_path = base_path / segment_bin_filename_str;


        std::ofstream dat_out(dat_file_path);
        if (!dat_out.is_open()) {
            std::cerr << "Error: Could not open .dat file for writing: " << dat_file_path.string() << std::endl;
            return;
        }

        dat_out << std::fixed << std::setprecision(6);
        dat_out << "# Data Container Dump V4.0\n\n";

        dat_out << "segment_container_count: " << segment_container_.size() << "\n";
        if (!segment_container_.empty()) {
            dat_out << "segment_binary_file: " << segment_bin_filename_str << "\n";
        }
        dat_out << "\n";

        dat_out << "[SCALARS]\n";
        dat_out << "background: " << background_ << "\n";
        dat_out << "voxel_size: " << voxel_size_ << "\n";
        dat_out << "minVal: " << minVal_ << "\n";
        dat_out << "[END_SCALARS]\n\n";

        dat_out << "[ORIGIN_CONTAINER]\n";
        write_text_matrix(dat_out, origin_container_);
        dat_out << "[END_ORIGIN_CONTAINER]\n\n";

        dat_out << "[FACE_TO_GRID_INDEX_CONTAINER]\n";
        write_text_matrix(dat_out, FaceToGridIndex_container_);
        dat_out << "[END_FACE_TO_GRID_INDEX_CONTAINER]\n\n";

        // Use the new function for FaceTypeMap_ and VertTypeMap_
        dat_out << "[FACE_TYPE_MAP]\n";
        write_indexed_vector_as_map(dat_out, FaceTypeMap_);
        dat_out << "[END_FACE_TYPE_MAP]\n\n";

        dat_out << "[TYPE_COUNT_MAP]\n";
        // This calls the overload for std::unordered_map<std::string, int>
        write_text_mapping_table(dat_out, TypeCount_);
        dat_out << "[END_TYPE_COUNT_MAP]\n\n";

        dat_out << "[VERT_TYPE_MAP]\n";
        write_indexed_vector_as_map(dat_out, VertTypeMap_);
        dat_out << "[END_VERT_TYPE_MAP]\n\n";

        dat_out.close();
        std::cout << "SegmentationDataContainer text data dumped to: " << dat_file_path.string() << std::endl;

        // --- Write the .bin binary file (logic unchanged) ---
        if (!segment_container_.empty()) {
            // ... (binary writing logic as before) ...
            std::ofstream bin_out(bin_file_path, std::ios::binary);
            if (!bin_out.is_open()) {
                std::cerr << "Error: Could not open .bin file for writing: " << bin_file_path.string() << std::endl;
                return;
            }
            const uint32_t magic_number = 0x5345474D;
            const uint16_t format_version = 1;
            using ElementType = float;
            uint8_t element_type_id = 0;
            if (std::is_same<ElementType, double>::value) { element_type_id = 1; }
            uint32_t num_segments_val = static_cast<uint32_t>(segment_container_.size());
            write_binary(bin_out, magic_number);
            write_binary(bin_out, format_version);
            write_binary(bin_out, element_type_id);
            write_binary(bin_out, num_segments_val);
            for (const auto& segment : segment_container_) {
                uint32_t d0 = static_cast<uint32_t>(segment.size());
                uint32_t d1 = static_cast<uint32_t>((d0 > 0 && !segment[0].empty()) ? segment[0].size() : 0);
                uint32_t d2 = static_cast<uint32_t>((d0 > 0 && d1 > 0 && !segment[0][0].empty()) ? segment[0][0].size() : 0);
                write_binary(bin_out, d0); write_binary(bin_out, d1); write_binary(bin_out, d2);
            }
            for (const auto& segment : segment_container_) {
                for (const auto& plane : segment) {
                    for (const auto& row : plane) {
                        if (!row.empty()) {
                            bin_out.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(ElementType));
                        }
                    }
                }
            }
            bin_out.close();
            std::cout << "SegmentationDataContainer segments dumped to: " << bin_file_path.string() << std::endl;
        }
        else {
            std::cout << "Segment container is empty, skipping binary file generation." << std::endl;
        }
    }
} // namespace cppIOUtility