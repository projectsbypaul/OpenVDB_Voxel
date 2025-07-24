// Enhanced Scripts.h - Header file with all new function declarations
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Scripts {
    namespace fs = std::filesystem;

    // ======== EXISTING FUNCTIONS ========

    // Dataset statistics job
    int run_dataset_stats_job(fs::path target, fs::path job_dir, fs::path log_dir,
        std::string temp_file_name, fs::path process_location);

    // Basic subdir job (original)
    int run_subdir_job(fs::path source, fs::path target, fs::path job_dir,
        fs::path log_dir, fs::path process_location);

    // Basic subdir job with ZIP support (original)
    int run_subdir_job_zip(fs::path source_zip, fs::path output_dir, fs::path job_file,
        fs::path log_dir, fs::path process_location);

    // Strip job
    int run_subdir_job_strip(fs::path source, fs::path target, fs::path job_dir,
        fs::path log_dir, fs::path process_location);

    // ======== NEW VS (VOXEL SIZE) VARIANTS ========

    // VS subdir job - processes jobs with voxel size parameters
    int run_subdir_job_vs(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, double voxel_size);

    // VS subdir job with ZIP support
    int run_subdir_job_zip_vs(fs::path source_zip, fs::path output_dir, fs::path job_file, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, double voxel_size);

    // ======== NEW NK (N_K_MIN) VARIANTS ========

    // NK subdir job - processes jobs with n_k_min parameters
    int run_subdir_job_nk(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, int n_k_min);

    // NK subdir job with ZIP support
    int run_subdir_job_zip_nk(fs::path source_zip, fs::path output_dir, fs::path job_file, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, int n_k_min);

    // ======== UTILITY FUNCTIONS ========

    // Universal job runner that can handle all variants based on job_type string
    int run_universal_subdir_job(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir,
        fs::path process_location, const std::string& job_type,
        int kernel_size = 0, int padding = 0, int bandwidth = 0,
        double voxel_size = 0.0, int n_k_min = 0);


} // namespace Scripts

// ======== USAGE EXAMPLES ========
/*

// Basic usage (existing functionality):
Scripts::run_subdir_job(source_path, target_path, job_file, log_dir, process_location);

// VS (Voxel Size) variant:
Scripts::run_subdir_job_vs(source_path, target_path, job_file, log_dir, process_location,
                          kernel_size, padding, bandwidth, voxel_size);

// NK (N_K_Min) variant:
Scripts::run_subdir_job_nk(source_path, target_path, job_file, log_dir, process_location,
                          kernel_size, padding, bandwidth, n_k_min);

// ZIP variants:
Scripts::run_subdir_job_zip_vs(source_zip, output_dir, job_file, log_dir, process_location,
                              kernel_size, padding, bandwidth, voxel_size);

Scripts::run_subdir_job_zip_nk(source_zip, output_dir, job_file, log_dir, process_location,
                              kernel_size, padding, bandwidth, n_k_min);

// Universal runner (auto-detects job type):
Scripts::run_universal_subdir_job(source_path, target_path, job_file, log_dir, process_location,
                                  "vs", kernel_size, padding, bandwidth, voxel_size);

*/