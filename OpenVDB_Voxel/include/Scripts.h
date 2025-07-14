#pragma once
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;
namespace Scripts {

	//Work Scripts
	int run_stats_on_subdir(fs::path source, fs::path target, std::string subdir_name, std::string temp_file_name);
	int run_segmentation_adaptive(fs::path source, fs::path target, int n_k_min);
	int run_segmentation_fixed(fs::path source, fs::path target, double voxel_size);
	int run_subdir_to_dataset(fs::path source, fs::path target, std::string subdir_name);
	int run_strip_obj_batch_job(fs::path source, fs::path target, fs::path job_location);
	//Test Scripts
}//namespace scripts