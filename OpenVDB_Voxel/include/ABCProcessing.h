#pragma once
#include <iostream>
#include <filesystem>
#ifndef SCRIPTS_H
#define SCRIPTS_H

#include <filesystem>
#include <string>
#include "GenericDirectoryProcess.h"

namespace Scripts {

    namespace fs = std::filesystem;

    /// <summary>
    /// Helper Functions
    /// </summary>
    bool checkIfDirWasProcessed(const std::string& targetDir);
    /// <summary>
    /// executes process on spefict subdir 
    /// </summary>
    void processOnSubdir(ProcessingUtility::GenericDirectoryProcess* Process, fs::path subdir_name, int max_threads);
    /// <summary>
    /// executes process on spefict subdir 
    /// </summary>
    void processOnSubdirTimed(ProcessingUtility::GenericDirectoryProcess* Process, fs::path subdir_name, int max_threads = 1, int timeout_min = 10);
    /// <summary>
    /// executes process on spefict subdir 
    /// </summary>
    void processOnSubdirTimedNoCheck(ProcessingUtility::GenericDirectoryProcess* Process, fs::path subdir_name, int max_threads = 1, int timeout_min = 10);

}

#endif // SCRIPTS_H