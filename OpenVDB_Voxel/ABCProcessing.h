#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <cstdio>

#include "Tools.h"
#include "DL_Preprocessing.h"
#include "LOG.h"



namespace Scripts {
	namespace fs = std::filesystem;
	void CopyAndRenameToParsedStructure(const fs::path& source_root, const fs::path& target_root);
	void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, double& voxel_size, int& band_width, int& padding);
	void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding);
	void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding, int max_threads);
	void parseABCtoDatasetAE(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding, double param_1, double param_2, double threshold, int random_seed, int max_threads);
	void processingForDLLDataset(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, int min_n_kernel, int band_width, int padding);
	void processingForDLLDatasetAE(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, int min_n_kernel, int band_width, int padding, double param_1, double param_2, double threshold, int random_seed);
    bool checkIfDirWasProcessed(const std::string& targetDir);
    void processingForDLLDataset(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, int min_n_kernel, int band_width, int padding);

}