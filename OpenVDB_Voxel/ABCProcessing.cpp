#include "ABCProcessing.h"
#include "NoiseOnMesh.h"
#include "GenericDirectoryProcess.h"
#include "ProcessChildren.h"
#include "Tools.h"
#include "LOG.h"
#include "DL_Preprocessing.h"

#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

namespace fs = std::filesystem;


namespace Scripts {
    /// <summary>
    /// Helper Functions
    /// </summary>
    bool checkIfDirWasProcessed(const std::string& targetDir) {
        if (fs::exists(targetDir) && fs::is_directory(targetDir)) {
             return true;
        }
        else {
             return false;
        }

    }

    void CopyAndRenameToParsedStructure(const fs::path& source_root, const fs::path& target_root) {
        // Create the target root if it doesn't exist
        fs::create_directories(target_root);

        for (const auto& entry : fs::recursive_directory_iterator(source_root)) {
            if (fs::is_regular_file(entry)) {

                fs::path file_path = entry.path();
                fs::path parent_dir = file_path.parent_path().filename(); // e.g., 00000001
                std::string extension = file_path.extension().string();   // e.g., .obj, .yml

                fs::path new_dir = target_root / parent_dir;


                fs::create_directories(new_dir);

                fs::path new_file_name = parent_dir.string() + extension; // e.g., 00000001.obj
                fs::path dest_path = new_dir / new_file_name;

                try {
                    fs::copy_file(file_path, dest_path, fs::copy_options::overwrite_existing);
                    std::cout << "Copied: " << file_path << " -> " << dest_path << '\n';
                }
                catch (const std::exception& e) {
                    std::cerr << "Failed to copy: " << file_path << " (" << e.what() << ")\n";
                }
            }
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

    /// <summary>
    /// parsing function to iterate over jobfiles 
    /// </summary>
    void processOnJobFile(ProcessingUtility::GenericDirectoryProcess* Process, fs::path job_location, int max_threads) {

        if (!Process) {
            LOG_LEVEL("ERROR", "Received a null GenericDirectoryProcess pointer.");
            return;
        }

        LOG_FUNC("ENTER" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);

        if (!(fs::exists(Process->getTargetDir()))) {
            fs::create_directories(Process->getTargetDir());
        }

        if (max_threads == 0) {
            max_threads = std::thread::hardware_concurrency();
        }

        std::cout << "Running on thread count: " << max_threads << std::endl;

        std::vector<std::string> job_list = read_job_file(job_location);

        for (const auto& job_name : job_list) {

            fs::path subdirPath = Process->getSourceDir() / job_name;

            if (!fs::is_directory(subdirPath)) continue;

            std::string subdirName = subdirPath.filename().string();

            bool hasObj = false;
            bool hasYml = false;

            for (const auto& file : fs::directory_iterator(subdirPath)) {
                if (!fs::is_regular_file(file.status())) continue;

                std::string ext = file.path().extension().string();
                if (ext == ".obj") hasObj = true;
                if (ext == ".yml") hasYml = true;
            }

            if (hasObj && hasYml) {
                fs::path newOutputDir = Process->getTargetDir() / subdirName;

                if (checkIfDirWasProcessed(newOutputDir.generic_string())) {
                    std::cout << "Directory " << newOutputDir.filename() << " already processed -> skipped" << std::endl;
                    LOG_FUNC("EXIT " << " Directory " << newOutputDir.filename() << " already processed->skipped");
                    continue;
                }

                LOG_LEVEL("INFO", "Started processing " + subdirName);

                try {
                    Process->run(subdirName);

                    LOG_LEVEL("INFO", "Finished processing " + subdirName);
                }
                catch (const std::exception& e) {
                    LOG_LEVEL("ERROR", "Error processing " + subdirName + ": " + e.what());
                }
            }
        }

        LOG_FUNC("EXIT" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);
    }


    /// <summary>
    /// parsing functions to iterate over dataset 
    /// </summary>
    void parseABCtoDataset(ProcessingUtility:: GenericDirectoryProcess* Process, int max_threads = 0) {

        if (!Process) {
            LOG_LEVEL("ERROR", "Received a null GenericDirectoryProcess pointer.");
            return;
        }

        LOG_FUNC("ENTER" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);

        fs::create_directories(Process->getTargetDir());

        if (max_threads == 0) {
            max_threads = std::thread::hardware_concurrency();
        }

        std::cout << "Running on thread count: " << max_threads << std::endl;


        for (const auto& entry : fs::directory_iterator(Process->getSourceDir())) {
            if (!fs::is_directory(entry.status())) continue;

            fs::path subdirPath = entry.path();
            std::string subdirName = subdirPath.filename().string();

            bool hasObj = false;
            bool hasYml = false;

            for (const auto& file : fs::directory_iterator(subdirPath)) {
                if (!fs::is_regular_file(file.status())) continue;

                std::string ext = file.path().extension().string();
                if (ext == ".obj") hasObj = true;
                if (ext == ".yml") hasYml = true;
            }

            if (hasObj && hasYml) {
                fs::path newOutputDir = Process->getTargetDir() / subdirName;

                if (checkIfDirWasProcessed(newOutputDir.generic_string())) {
                    std::cout << "Directory " << newOutputDir.filename() << " already processed -> skipped" << std::endl;
                    LOG_FUNC("EXIT " << " Directory " << newOutputDir.filename() << " already processed->skipped");
                    continue;
                }

                LOG_LEVEL("INFO", "Started processing " + subdirName);

                try {
                    Process->run(subdirName);

                    LOG_LEVEL("INFO", "Finished processing " + subdirName);
                }
                catch (const std::exception& e) {
                    LOG_LEVEL("ERROR", "Error processing " + subdirName + ": " + e.what());
                }       
            }
        }

        LOG_FUNC("EXIT" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);
    }

   


}