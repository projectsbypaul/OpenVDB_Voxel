#pragma once
#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;


namespace Scripts {
	int run_subdir_job_zip(fs::path source_zip, fs::path output_dir, fs::path job_file, fs::path log_dir, fs::path process_location);
	int run_dataset_stats_job(fs::path target, fs::path job_dir, fs::path log_dir, std::string temp_file_name, fs::path process_location);
	int run_subdir_job(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir, fs::path process_location);
	int run_subdir_job_strip(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir, fs::path process_location);
}