#pragma once
#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;


namespace Scripts {
	int run_subdir_job(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir, fs::path process_location);
}