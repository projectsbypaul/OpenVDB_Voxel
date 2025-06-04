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
    }
    namespace Macros {

    }
}