// Enhanced Scripts.cpp with robust job tracking using set
#include "../include/Scripts.h"
#include "../include/jobUtility.h"
#include "../include/ZipUtility.h"

#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <iostream>
namespace fs = std::filesystem;

namespace Scripts {

    // Helper to write set to job file as vector
    inline void write_job_set(const fs::path& job_file, const std::set<std::string>& remaining_jobs) {
        std::vector<std::string> jobs(remaining_jobs.begin(), remaining_jobs.end());
        jobUtilitiy::Functions::write_remaining_jobs(job_file, jobs);
    }

    // Existing function - unchanged
    int run_dataset_stats_job(fs::path target, fs::path job_dir, fs::path log_dir, std::string temp_file_name, fs::path process_location) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_dir);

        for (const std::string j : jobs) {
            std::string c0 = process_location.generic_string();
            std::string c1 = target.generic_string();
            std::string c2 = log_dir.generic_string();
            std::string c3 = temp_file_name;
            std::string cmd = c0 + " " + "stats_subdir" + " " + c1 + " " + c1 + " " + j + " " + c2 + " " + c3;

            std::cout << "Running: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result != 0) {
                std::cerr << "Subprocess failed for mesh: " << j << std::endl;
            }
        }

        return 0;
    }

    // Original run_subdir_job (basic version) - now with set-based job tracking
    int run_subdir_job(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir, fs::path process_location) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_dir);
        std::set<std::string> remaining_jobs(jobs.begin(), jobs.end());

        size_t processed_count = 0;
        for (const auto& j : jobs) {
            if (remaining_jobs.count(j) == 0) continue;

            std::string c0 = process_location.generic_string();
            std::string c1 = source.generic_string();
            std::string c2 = target.generic_string();
            std::string c3 = log_dir.generic_string();

            std::string cmd = c0 + " subdirJob " + c1 + " " + c2 + " " + j + " " + c3;

            std::cout << "Running: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result == 0) {
                remaining_jobs.erase(j);
                ++processed_count;
            }
            else {
                std::cerr << "Subprocess failed for mesh: " << j << std::endl;
            }

            if (processed_count % 10 == 0 || processed_count == jobs.size()) {
                write_job_set(job_dir, remaining_jobs);
            }
        }
        write_job_set(job_dir, remaining_jobs);
        return 0;
    }

    // VS variant of run_subdir_job
    int run_subdir_job_vs(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, double voxel_size) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_dir);
        std::set<std::string> remaining_jobs(jobs.begin(), jobs.end());

        std::cout << "Starting VS job with parameters: kernel=" << kernel_size
            << ", padding=" << padding << ", bandwidth=" << bandwidth
            << ", voxel_size=" << voxel_size << std::endl;

        size_t processed_count = 0;
        for (const auto& j : jobs) {
            if (remaining_jobs.count(j) == 0) continue;

            std::string c0 = process_location.generic_string();
            std::string c1 = source.generic_string();
            std::string c2 = target.generic_string();
            std::string c3 = log_dir.generic_string();
            std::string c4 = std::to_string(kernel_size);
            std::string c5 = std::to_string(padding);
            std::string c6 = std::to_string(bandwidth);
            std::string c7 = std::to_string(voxel_size);

            std::string cmd = c0 + " vs_subdirJob " + c1 + " " + c2 + " " + j + " " + c3 + " " + c4 + " " + c5 + " " + c6 + " " + c7;

            std::cout << "Running VS: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result == 0) {
                remaining_jobs.erase(j);
                ++processed_count;
            }
            else {
                std::cerr << "VS Subprocess failed for mesh: " << j << std::endl;
            }

            if (processed_count % 10 == 0 || processed_count == jobs.size()) {
                write_job_set(job_dir, remaining_jobs);
            }
        }
        write_job_set(job_dir, remaining_jobs);
        return 0;
    }

    // NK variant of run_subdir_job
    int run_subdir_job_nk(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, int n_k_min) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_dir);
        std::set<std::string> remaining_jobs(jobs.begin(), jobs.end());

        std::cout << "Starting NK job with parameters: kernel=" << kernel_size
            << ", padding=" << padding << ", bandwidth=" << bandwidth
            << ", n_k_min=" << n_k_min << std::endl;

        size_t processed_count = 0;
        for (const auto& j : jobs) {
            if (remaining_jobs.count(j) == 0) continue;

            std::string c0 = process_location.generic_string();
            std::string c1 = source.generic_string();
            std::string c2 = target.generic_string();
            std::string c3 = log_dir.generic_string();
            std::string c4 = std::to_string(kernel_size);
            std::string c5 = std::to_string(padding);
            std::string c6 = std::to_string(bandwidth);
            std::string c7 = std::to_string(n_k_min);

            std::string cmd = c0 + " nk_subdirJob " + c1 + " " + c2 + " " + j + " " + c3 + " " + c4 + " " + c5 + " " + c6 + " " + c7;

            std::cout << "Running NK: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result == 0) {
                remaining_jobs.erase(j);
                ++processed_count;
            }
            else {
                std::cerr << "NK Subprocess failed for mesh: " << j << std::endl;
            }

            if (processed_count % 10 == 0 || processed_count == jobs.size()) {
                write_job_set(job_dir, remaining_jobs);
            }
        }
        write_job_set(job_dir, remaining_jobs);
        return 0;
    }

    // Original run_subdir_job_zip (basic version) - now with set-based job tracking
    int run_subdir_job_zip(fs::path source_zip, fs::path output_dir, fs::path job_file, fs::path log_dir, fs::path process_location) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_file);
        std::set<std::string> remaining_jobs(jobs.begin(), jobs.end());

        //clean traget dir 
        jobUtilitiy::Functions::clean_target_dir(output_dir);

        //create target zip 
        fs::path out_archive_name = job_file.stem().string() + ".zip";
        fs::path out_archive_path = output_dir / out_archive_name;
        ZIPutil::Functions::zip_create_archive(out_archive_path);

        //create tmp dir 
        fs::path temp_dir = output_dir / ("temp_" + out_archive_name.stem().generic_string());
        fs::path temp_input = temp_dir / "inputs";
        fs::path temp_output = temp_dir / "outputs";

        if (!fs::exists(temp_input)) {
            fs::create_directories(temp_input);
        }
        if (!fs::exists(temp_output)) {
            fs::create_directories(temp_output);
        }

        size_t processed_count = 0;
        for (const auto& j : jobs) {
            if (remaining_jobs.count(j) == 0) continue;

            ZIPutil::Functions::zip_extract_subfolder(source_zip, temp_input / j, j);

            std::string c0 = process_location.generic_string();
            std::string c1 = temp_input.generic_string();
            std::string c2 = temp_output.generic_string();
            std::string c3 = log_dir.generic_string();

            std::string cmd = c0 + " subdirJob " + c1 + " " + c2 + " " + j + " " + c3;

            std::cout << "Running: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result == 0) {
                remaining_jobs.erase(j);
                ++processed_count;

                if (fs::exists(temp_output / j)) {
                    ZIPutil::Functions::zip_write_subfolder(out_archive_path, temp_output / j, j);
                }

                fs::remove_all(temp_input / j);
                fs::remove_all(temp_output / j);
            }
            else {
                std::cerr << "Subprocess failed for mesh: " << j << std::endl;
            }

            if (processed_count % 10 == 0 || processed_count == jobs.size()) {
                write_job_set(job_file, remaining_jobs);
            }
        }
        write_job_set(job_file, remaining_jobs);
        fs::remove_all(temp_dir);

        return 0;
    }

    // VS variant of run_subdir_job_zip
    int run_subdir_job_zip_vs(fs::path source_zip, fs::path output_dir, fs::path job_file, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, double voxel_size) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_file);
        std::set<std::string> remaining_jobs(jobs.begin(), jobs.end());

        //clean traget dir 
        jobUtilitiy::Functions::clean_target_dir(output_dir);

        std::cout << "Starting VS ZIP job with parameters: kernel=" << kernel_size
            << ", padding=" << padding << ", bandwidth=" << bandwidth
            << ", voxel_size=" << voxel_size << std::endl;

        fs::path out_archive_name = job_file.stem().string() + "_vs.zip";
        fs::path out_archive_path = output_dir / out_archive_name;
        ZIPutil::Functions::zip_create_archive(out_archive_path);

        fs::path temp_dir = output_dir / ("temp_vs_" + out_archive_name.stem().generic_string());
        fs::path temp_input = temp_dir / "inputs";
        fs::path temp_output = temp_dir / "outputs";

        if (!fs::exists(temp_input)) {
            fs::create_directories(temp_input);
        }
        if (!fs::exists(temp_output)) {
            fs::create_directories(temp_output);
        }

        size_t processed_count = 0;
        for (const auto& j : jobs) {
            if (remaining_jobs.count(j) == 0) continue;

            ZIPutil::Functions::zip_extract_subfolder(source_zip, temp_input / j, j);

            std::string c0 = process_location.generic_string();
            std::string c1 = temp_input.generic_string();
            std::string c2 = temp_output.generic_string();
            std::string c3 = log_dir.generic_string();
            std::string c4 = std::to_string(kernel_size);
            std::string c5 = std::to_string(padding);
            std::string c6 = std::to_string(bandwidth);
            std::string c7 = std::to_string(voxel_size);

            std::string cmd = c0 + " vs_subdirJob " + c1 + " " + c2 + " " + j + " " + c3 + " " + c4 + " " + c5 + " " + c6 + " " + c7;

            std::cout << "Running VS ZIP: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result == 0) {
                remaining_jobs.erase(j);
                ++processed_count;

                if (fs::exists(temp_output / j)) {
                    ZIPutil::Functions::zip_write_subfolder(out_archive_path, temp_output / j, j);
                }

                fs::remove_all(temp_input / j);
                fs::remove_all(temp_output / j);
            }
            else {
                std::cerr << "VS ZIP Subprocess failed for mesh: " << j << std::endl;
            }

            if (processed_count % 10 == 0 || processed_count == jobs.size()) {
                write_job_set(job_file, remaining_jobs);
            }
        }
        write_job_set(job_file, remaining_jobs);
        fs::remove_all(temp_dir);

        return 0;
    }

    // NK variant of run_subdir_job_zip
    int run_subdir_job_zip_nk(fs::path source_zip, fs::path output_dir, fs::path job_file, fs::path log_dir,
        fs::path process_location, int kernel_size, int padding, int bandwidth, int n_k_min) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_file);
        std::set<std::string> remaining_jobs(jobs.begin(), jobs.end());

        //clean traget dir 
        jobUtilitiy::Functions::clean_target_dir(output_dir);

        std::cout << "Starting NK ZIP job with parameters: kernel=" << kernel_size
            << ", padding=" << padding << ", bandwidth=" << bandwidth
            << ", n_k_min=" << n_k_min << std::endl;

        fs::path out_archive_name = job_file.stem().string() + "_nk.zip";
        fs::path out_archive_path = output_dir / out_archive_name;
        ZIPutil::Functions::zip_create_archive(out_archive_path);

        fs::path temp_dir = output_dir / ("temp_nk_" + out_archive_name.stem().generic_string());
        fs::path temp_input = temp_dir / "inputs";
        fs::path temp_output = temp_dir / "outputs";

        if (!fs::exists(temp_input)) {
            fs::create_directories(temp_input);
        }
        if (!fs::exists(temp_output)) {
            fs::create_directories(temp_output);
        }

        size_t processed_count = 0;
        for (const auto& j : jobs) {
            if (remaining_jobs.count(j) == 0) continue;

            ZIPutil::Functions::zip_extract_subfolder(source_zip, temp_input / j, j);

            std::string c0 = process_location.generic_string();
            std::string c1 = temp_input.generic_string();
            std::string c2 = temp_output.generic_string();
            std::string c3 = log_dir.generic_string();
            std::string c4 = std::to_string(kernel_size);
            std::string c5 = std::to_string(padding);
            std::string c6 = std::to_string(bandwidth);
            std::string c7 = std::to_string(n_k_min);

            std::string cmd = c0 + " nk_subdirJob " + c1 + " " + c2 + " " + j + " " + c3 + " " + c4 + " " + c5 + " " + c6 + " " + c7;

            std::cout << "Running NK ZIP: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result == 0) {
                remaining_jobs.erase(j);
                ++processed_count;

                if (fs::exists(temp_output / j)) {
                    ZIPutil::Functions::zip_write_subfolder(out_archive_path, temp_output / j, j);
                }

                fs::remove_all(temp_input / j);
                fs::remove_all(temp_output / j);
            }
            else {
                std::cerr << "NK ZIP Subprocess failed for mesh: " << j << std::endl;
            }

            if (processed_count % 10 == 0 || processed_count == jobs.size()) {
                write_job_set(job_file, remaining_jobs);
            }
        }
        write_job_set(job_file, remaining_jobs);
        fs::remove_all(temp_dir);

        return 0;
    }

    // Existing function - unchanged
    int run_subdir_job_strip(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir, fs::path process_location) {

        std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_dir);

        for (const std::string j : jobs) {
            std::string c0 = process_location.generic_string();
            std::string c1 = source.generic_string();
            std::string c2 = target.generic_string();
            std::string c3 = log_dir.generic_string();

            std::string cmd = c0 + " " + "subdirJobStrip" + " " + c1 + " " + c2 + " " + j + " " + c3;

            std::cout << "Running: " << cmd << std::endl;
            int result = std::system(cmd.c_str());
            if (result != 0) {
                std::cerr << "Subprocess failed for mesh: " << j << std::endl;
            }
        }

        return 0;
    }

    // UTILITY: Universal job runner that can handle all variants
    int run_universal_subdir_job(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir,
        fs::path process_location, const std::string& job_type,
        int kernel_size, int padding, int bandwidth,
        double voxel_size, int n_k_min) {

        if (job_type == "basic" || job_type == "subdirJob") {
            return run_subdir_job(source, target, job_dir, log_dir, process_location);
        }
        else if (job_type == "vs" || job_type == "vs_subdirJob") {
            return run_subdir_job_vs(source, target, job_dir, log_dir, process_location,
                kernel_size, padding, bandwidth, voxel_size);
        }
        else if (job_type == "nk" || job_type == "nk_subdirJob") {
            return run_subdir_job_nk(source, target, job_dir, log_dir, process_location,
                kernel_size, padding, bandwidth, n_k_min);
        }
        else {
            std::cerr << "Unknown job type: " << job_type << std::endl;
            return 1;
        }
    }

} // namespace Scripts
