#include "jobUtility.h"

#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

namespace jobUtilitiy {

	namespace Functions {
        std::vector<std::string> read_job_file(const fs::path& file_path) {
            std::vector<std::string> job_lines;
            std::ifstream infile(file_path);

            if (!infile.is_open()) {
                // Handle error: unable to open file
                // You could throw an exception, return an empty vector,
                // or print an error message.
                std::cerr << "Error: Could not open file " << file_path << std::endl;
                return job_lines; // Return empty vector on error
            }

            std::string line;
            while (std::getline(infile, line)) {
                job_lines.push_back(line);
            }

            infile.close();
            return job_lines;
        }
	}
	namespace Macros {

	}
}