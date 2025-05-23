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

    /// <summary>
    /// Processing Function for Subdirectories
    /// </summary>

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

        std::vector<std::thread> threads;
        std::mutex mtx;
        std::condition_variable cv;
        int active_threads = 0;

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

                // Wait until there is a free thread slot
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]() { return active_threads < max_threads; });
                ++active_threads;
                lock.unlock();

                threads.emplace_back([&, subdirPath, newOutputDir, subdirName]() {

                        LOG_LEVEL("INFO", "Started processing " + subdirName);

                        try {
                            Process->run(subdirName);

                            LOG_LEVEL("INFO", "Finished processing " + subdirName);
                        }
                        catch (const std::exception& e) {
                            LOG_LEVEL("ERROR", "Error processing " + subdirName + ": " + e.what());
                        }

                        {
                            std::lock_guard<std::mutex> guard(mtx);
                            --active_threads;
                        }
                        cv.notify_one();
                    });
            }
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        LOG_FUNC("EXIT" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);
    }

   


}