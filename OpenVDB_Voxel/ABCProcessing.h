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
    /// Processing Function for Subdirectories
    /// </summary>
    void processingForDLLDataset(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, double voxel_size, int band_width, int padding);
    void processingForDLLDataset(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, int min_n_kernel, int band_width, int padding);
    void processingForDLLDatasetAE(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, int min_n_kernel, int band_width, int padding, double param_1, double param_2, double threshold, int random_seed);

    /// <summary>
    /// parsing functions to iterate over dataset
    /// </summary>
    void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, double& voxel_size, int& band_width, int& padding);
    void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding, int max_threads);
    void parseABCtoDatasetAE(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding, double param_1, double param_2, double threshold, int random_seed, int max_threads);
    void parseABCtoDataset(ProcessingUtility::GenericDirectoryProcess* Process, int max_threads);
}

#endif // SCRIPTS_H