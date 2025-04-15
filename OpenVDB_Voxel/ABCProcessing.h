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



namespace Scripts {
	namespace fs = std::filesystem;
	void CopyAndRenameToParsedStructure(const fs::path& source_root, const fs::path& target_root);
	void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, double& voxel_size, int& band_width, int& padding);
	void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding);
	void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding, int max_threads);
}