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
    void CopyAndRenameToParsedStructure(const fs::path& source_root, const fs::path& target_root);
    /// <summary>
    /// parsing functions to iterate over job fils
    /// </summary>
    void processOnJobFile(ProcessingUtility::GenericDirectoryProcess* Process, fs::path job_location, int max_threads = 1);
    /// <summary>
    /// parsing functions to iterate over dataset
    /// </summary>
    void parseABCtoDataset(ProcessingUtility::GenericDirectoryProcess* Process, int max_threads);
}
#endif // SCRIPTS_H