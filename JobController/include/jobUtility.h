#pragma once
#include <vector>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
namespace fs = std::filesystem;

namespace jobUtilitiy {

    namespace Functions {
        std::vector<std::string> read_job_file(const fs::path& file_path);
        void write_remaining_jobs(const fs::path& job_dir, const std::vector<std::string>& remaining_jobs);
        void clean_target_dir(const fs::path& dir_path);
    }
    namespace Macros {

    }
}