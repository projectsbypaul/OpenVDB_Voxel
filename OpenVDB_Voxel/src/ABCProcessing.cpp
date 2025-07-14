#include "../include/ABCProcessing.h"
#include "../include/NoiseOnMesh.h"
#include "../include/GenericDirectoryProcess.h"
#include "../include/ProcessChildren.h"
#include "../include/Tools.h"
#include "../include/LOG.h"
#include "../include/DL_Preprocessing.h"

#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <type_traits>

#include <future>
#include <optional>
#include <chrono>
#include <type_traits>

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
    /// temlate to implement timout 
    /// </summary>

    // Non-void version
    template <typename Func, typename... Args>
    auto run_with_timeout(Func&& func, std::chrono::milliseconds timeout, Args&&... args)
        -> std::enable_if_t<!std::is_void_v<std::invoke_result_t<Func, Args...>>,
        std::optional<std::invoke_result_t<Func, Args...>>>
    {
        using ReturnType = std::invoke_result_t<Func, Args...>;
        std::packaged_task<ReturnType(Args...)> task(std::forward<Func>(func));
        auto future = task.get_future();
        std::thread(std::move(task), std::forward<Args>(args)...).detach();

        if (future.wait_for(timeout) == std::future_status::ready) {
            return future.get();
        }
        else {
            return std::nullopt;
        }
    }

    // Void version
    template <typename Func, typename... Args>
    auto run_with_timeout(Func&& func, std::chrono::milliseconds timeout, Args&&... args)
        -> std::enable_if_t<std::is_void_v<std::invoke_result_t<Func, Args...>>, bool>
    {
        using ReturnType = std::invoke_result_t<Func, Args...>;
        std::packaged_task<ReturnType(Args...)> task(std::forward<Func>(func));
        auto future = task.get_future();
        std::thread(std::move(task), std::forward<Args>(args)...).detach();

        if (future.wait_for(timeout) == std::future_status::ready) {
            future.get(); // returns void
            return true;
        }
        else {
            return false;
        }
    }
    /// <summary>
   /// parsing function to iterate over jobfiles 
   /// </summary>
    void processOnSubdirTimed(ProcessingUtility::GenericDirectoryProcess* Process, fs::path subdir_name, int max_threads, int timeout_min) {

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


        fs::path subdirPath = Process->getSourceDir() / subdir_name;

        if (!fs::is_directory(subdirPath)) return;

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
                return;
            }

            LOG_LEVEL("INFO", "Started processing " + subdirName);

            try {
                // Correct: pass the lambda capturing both Process and subdirName
                auto result = Scripts::run_with_timeout(
                    [Process, subdirName]() { Process->run(subdirName); },
                    std::chrono::minutes(timeout_min)
                );

                LOG_LEVEL("INFO", "Finished processing " + subdirName);
            }
            catch (const std::exception& e) {
                LOG_LEVEL("ERROR", "Error processing " + subdirName + ": " + e.what());
            }
        }
        LOG_FUNC("EXIT" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);
    }

  /// <summary>
  /// parsing function to iterate over jobfiles 
  /// </summary>
    void processOnSubdirTimedNoCheck(ProcessingUtility::GenericDirectoryProcess* Process, fs::path subdir_name, int max_threads, int timeout_min) {

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


        fs::path subdirPath = Process->getSourceDir() / subdir_name;

        if (!fs::is_directory(subdirPath)) return;

        std::string subdirName = subdirPath.filename().string();

        LOG_LEVEL("INFO", "Started processing " + subdirName);

        try {
            // Correct: pass the lambda capturing both Process and subdirName
            auto result = Scripts::run_with_timeout(
                [Process, subdirName]() { Process->run(subdirName); },
                std::chrono::minutes(timeout_min)
            );

            LOG_LEVEL("INFO", "Finished processing " + subdirName);
        }
        catch (const std::exception& e) {
            LOG_LEVEL("ERROR", "Error processing " + subdirName + ": " + e.what());
        }
        
        LOG_FUNC("EXIT" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);
    }

    /// <summary>
    /// parsing function to iterate over jobfiles 
    /// </summary>
    void processOnSubdir(ProcessingUtility::GenericDirectoryProcess* Process, fs::path subdir_name, int max_threads) {

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


        fs::path subdirPath = Process->getSourceDir() / subdir_name;

        if (!fs::is_directory(subdirPath)) return;

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
                return;
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
        LOG_FUNC("EXIT" << " Source_Dir = " << Process->getSourceDir().filename() << ", Output_Dir = " << Process->getTargetDir().filename() << ", Threads = " << max_threads);
    }
}