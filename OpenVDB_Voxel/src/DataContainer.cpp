#include "../include/DataContainerHelpers.h"
#include "../include/DataContainer.h"
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
                catch (const std::exception& e) { std::cerr << "Error creating directory " << DumpDir.string() << ": " << e.what() << "\n"; return; }
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
                    catch (const std::exception& e) { std::cerr << "Error creating directory " << base_path.string() << ": " << e.what() << "\n"; return; }
                }
            }
        }

        fs::path dat_file_path = base_path / (base_filename_stem + ".dat");

        std::string segment_bin_filename_str = base_filename_stem + "_segments.bin";
        fs::path bin_file_path = base_path / segment_bin_filename_str;

        std::string label_bin_filename_str = base_filename_stem + "_labels.bin";
        fs::path label_bin_file_path = base_path / label_bin_filename_str;

        std::string prediction_bin_filename_str = base_filename_stem + "_predictions.bin";
        fs::path prediction_bin_file_path = base_path / prediction_bin_filename_str;

        this->dump_info(dat_file_path, segment_bin_filename_str);

        this->dump_segments_bin(bin_file_path);
        this->dump_labels_bin(label_bin_file_path);
        this->dump_predictions_bin(prediction_bin_file_path);
        
    }

    void SegmentationDataContainer::load(const fs::path& LoadDir)
    {
        fs::path base_path;
        std::string base_filename_stem = "segmentation_data";

        if (LoadDir.empty()) {
            base_path = fs::current_path();
        }
        else if (fs::is_directory(LoadDir)) {
            base_path = LoadDir;
        }
        else {
            base_path = LoadDir.parent_path();
            if (base_path.empty()) base_path = fs::current_path();
            if (!LoadDir.stem().empty())
                base_filename_stem = LoadDir.stem().string();
        }

        fs::path dat_file_path = base_path / (base_filename_stem + ".dat");
        if (!fs::exists(dat_file_path)) {
            std::cerr << "Error: .dat file not found at " << dat_file_path << "\n";
            return;
        }

        fs::path bin_filename; // relative name from .dat, if present
        this->load_info(dat_file_path, &bin_filename);

        // If .dat named the binary file, prefer that; else default convention
        
        fs::path bin_file_path = base_path / (base_filename_stem + "_segments.bin");
        if (fs::exists(bin_file_path)) {
            this->load_segments_bin(bin_file_path);
        }
        else {
            std::cout << "Note: binary segments file not found (" << bin_file_path
                << "); leaving segment_container_ empty.\n";
            segment_container_.clear();
        }

        fs::path label_bin_file_path = base_path / (base_filename_stem + "_labels.bin");
        if (fs::exists(label_bin_file_path)) {
            this->load_labels_bin(label_bin_file_path);
        }
        else {
            std::cout << "Note: binary labels file not found (" << bin_file_path
                << "); leaving label_container_ empty.\n";
            label_container_.clear();
        }

        fs::path prediction_bin_file_path = base_path / (base_filename_stem + "_predictions.bin");
        if (fs::exists(prediction_bin_file_path)) {
            this->load_predictions_bin(prediction_bin_file_path);
        }
        else {
            std::cout << "Note: binary labels file not found (" << bin_file_path
                << "); leaving prediction_container_ empty.\n";
            prediction_container_.clear();
        }


    }

    void SegmentationDataContainer::dump_info(const fs::path& dat_file_path, const fs::path& segment_bin_filename_str)
    {
        std::ofstream dat_out(dat_file_path);
        if (!dat_out.is_open()) {
            std::cerr << "Error: Could not open .dat file for writing: " << dat_file_path.string() << "\n";
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

        dat_out << "[VERT_TO_GRID_INDEX_CONTAINER]\n";
        write_text_matrix(dat_out, VertexToGridIndex_conatiner_);
        dat_out << "[END_VERT_TO_GRID_INDEX_CONTAINER]\n\n";

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
        std::cout << "SegmentationDataContainer text data dumped to: " << dat_file_path.string() << "\n";
    }

    void SegmentationDataContainer::dump_segments_bin(const fs::path& p)
    {
        dump_list_of_3d_arrays_bin(p, segment_container_);
    }

    void SegmentationDataContainer::dump_labels_bin(const fs::path& p)
    {
        dump_list_of_3d_arrays_bin(p, label_container_);
    }

    void SegmentationDataContainer::dump_predictions_bin(const fs::path& p)
    {
        dump_list_of_3d_arrays_bin(p, prediction_container_);
    }

    void SegmentationDataContainer::dump_segments_h5(const fs::path& p)
    {
      //TODO 
    }

    void SegmentationDataContainer::load_info(const fs::path& dat_file_path, fs::path* out_segment_bin_filename)
    {
        if (out_segment_bin_filename) *out_segment_bin_filename = fs::path();

        std::ifstream dat_in(dat_file_path);
        if (!dat_in.is_open()) {
            std::cerr << "Error: Could not open .dat for reading: " << dat_file_path << "\n";
            return;
        }

        // Light header read to catch "segment_binary_file"
        // We'll parse sections anyway.
        auto sections = read_dat_sections(dat_in);
        dat_in.close();

        // ---- scalars ----
        if (sections.count("SCALARS")) {
            for (const auto& line : sections["SCALARS"]) {
                std::string k, v;
                if (!split_kv(line, k, v)) continue;
                try {
                    if (k == "background") background_ = static_cast<float>(std::stod(v));
                    else if (k == "voxel_size") voxel_size_ = static_cast<float>(std::stod(v));
                    else if (k == "minVal") minVal_ = static_cast<float>(std::stod(v));
                }
                catch (...) {}
            }
        }
        else {
            std::cerr << "Warning: [SCALARS] section missing.\n";
        }

        // ---- matrix sections ----
        if (sections.count("ORIGIN_CONTAINER")) {
            if (!read_text_matrix(sections["ORIGIN_CONTAINER"], origin_container_)) {
                std::cerr << "Warning: Failed to parse [ORIGIN_CONTAINER].\n";
            }
        }
        if (sections.count("FACE_TO_GRID_INDEX_CONTAINER")) {
            if (!read_text_matrix(sections["FACE_TO_GRID_INDEX_CONTAINER"], FaceToGridIndex_container_)) {
                std::cerr << "Warning: Failed to parse [FACE_TO_GRID_INDEX_CONTAINER].\n";
            }
        }
        if (sections.count("VERT_TO_GRID_INDEX_CONTAINER")) {
            if (!read_text_matrix(sections["VERT_TO_GRID_INDEX_CONTAINER"], VertexToGridIndex_conatiner_)) {
                std::cerr << "Warning: Failed to parse [VERT_TO_GRID_INDEX_CONTAINER].\n";
            }
        }

        // ---- maps ----
        if (sections.count("FACE_TYPE_MAP")) {
            if (!read_indexed_vector_as_map(sections["FACE_TYPE_MAP"], FaceTypeMap_)) {
                std::cerr << "Warning: Failed to parse [FACE_TYPE_MAP].\n";
            }
        }
        if (sections.count("TYPE_COUNT_MAP")) {
            if (!read_text_mapping_table(sections["TYPE_COUNT_MAP"], TypeCount_)) {
                std::cerr << "Warning: Failed to parse [TYPE_COUNT_MAP].\n";
            }
        }
        if (sections.count("VERT_TYPE_MAP")) {
            if (!read_indexed_vector_as_map(sections["VERT_TYPE_MAP"], VertTypeMap_)) {
                std::cerr << "Warning: Failed to parse [VERT_TYPE_MAP].\n";
            }
        }

        // ---- segment_container_count / segment_binary_file from the header preface ----
        // The header lines before sections aren't captured by read_dat_sections().
        // So we re-open quickly to sniff those specific keys.
        {
            std::ifstream header_in(dat_file_path);
            if (header_in.is_open()) {
                std::string line;
                while (std::getline(header_in, line)) {
                    auto t = trim(line);
                    if (t.empty()) continue;
                    if (t[0] == '[') break; // sections start
                    std::string k, v;
                    if (!split_kv(t, k, v)) continue;
                    if (k == "segment_binary_file" && out_segment_bin_filename) {
                        *out_segment_bin_filename = fs::path(v);
                    }
                }
            }
        }

        std::cout << "SegmentationDataContainer text data loaded from: " << dat_file_path.string() << "\n";
    }
    void SegmentationDataContainer::load_segments_bin(const fs::path& p)
    {
        if (load_list_of_3d_arrays_bin<float>(p, segment_container_)) {
            std::cout << "SegmentationDataContainer segments loaded from: "
                << p.string() << "\n";
        }
    }

    void SegmentationDataContainer::load_labels_bin(const fs::path& p)
    {
        if (load_list_of_3d_arrays_bin<int>(p, label_container_)) {
            std::cout << "SegmentationDataContainer labels loaded from: "
                << p.string() << "\n";
        }
    }

    void SegmentationDataContainer::load_predictions_bin(const fs::path& p)
    {
        if (load_list_of_3d_arrays_bin<int32_t>(p, prediction_container_)) {
            std::cout << "SegmentationDataContainer predictions loaded from: "
                << p.string() << "\n";
        }
    }
} // namespace cppIOUtility