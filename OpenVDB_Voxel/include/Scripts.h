#pragma once
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;
namespace Scripts {

	//Work Scripts
	int run_segmentation_on_vdb(fs::path source, fs::path target);
	int run_stats_on_subdir(fs::path source, fs::path target, std::string subdir_name, std::string temp_file_name);
	int run_segmentation_adaptive(fs::path source, fs::path target, int n_k_min);
	int run_segmentation_fixed(fs::path source, fs::path target, double voxel_size);
	int run_subdir_to_dataset(fs::path source, fs::path target, std::string subdir_name);
	int run_subdir_to_dataset(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, int n_k_min, int max_threads = 1, int openvdb_threads = 1, int timeout_min = 5);
	int run_subdir_to_dataset(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, double voxel_size, int max_threads = 1, int openvdb_threads = 1, int timeout_min = 5);
	int run_strip_obj_batch_job(fs::path source, fs::path target, fs::path job_location);
	int run_export_vdb(fs::path source, fs::path target);
	//Test Scripts
}//namespace scripts
namespace Tests {
	int run_grid_test(fs::path filename);
}