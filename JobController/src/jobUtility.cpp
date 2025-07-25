#include "../include/jobUtility.h"

#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

namespace jobUtilitiy {

	namespace Functions {

        void clean_target_dir(const fs::path& dir_path) {
            if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
                std::cerr << "Error: target is not a directory or does not exist" << std::endl;
                return;
            }

            try {
                for (const auto& entry : fs::directory_iterator(dir_path)) {
                    // Remove all directories, and all non-zip files
                    if (fs::is_directory(entry) || entry.path().extension() != ".zip") {
                        fs::remove_all(entry);
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Exception while cleaning directory: " << e.what() << std::endl;
            }
        }

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

        void write_remaining_jobs(const fs::path& job_dir, const std::vector<std::string>& remaining_jobs) {
            std::ofstream outfile(job_dir, std::ios::trunc);
            if (!outfile.is_open()) {
                std::cerr << "Error: Could not write to job file: " << job_dir << std::endl;
                return;
            }
            for (const auto& job : remaining_jobs) {
                if (!job.empty()) {
                    outfile << job << "\n";
                }
            }
            outfile.close();
        }



	}
	namespace Macros {

	}
}