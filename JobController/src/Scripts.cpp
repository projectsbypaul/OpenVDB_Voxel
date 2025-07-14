#include "../include/Scripts.h"
#include "../include/jobUtility.h"
#include "../include/ZipUtility.h"

#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
namespace fs = std::filesystem;

namespace Scripts {

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
	int run_subdir_job_zip(fs::path source_zip, fs::path output_dir, fs::path job_file, fs::path log_dir, fs::path process_location) {

		//reading job file
		std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_file);
		std::vector<std::string> remaining_jobs = jobs;

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

		//iterate over jobs
		size_t processed_count = 0;
		for (size_t i = 0; i < jobs.size(); ++i) {
			const std::string& j = jobs[i];
			//extract files from zip to temp_input
			ZIPutil::Functions::zip_extract_subfolder(source_zip, temp_input / j, j);

			std::string c0 = process_location.generic_string();
			std::string c1 = temp_input.generic_string();
			std::string c2 = temp_output.generic_string();
			std::string c3 = log_dir.generic_string();

			std::string cmd = c0 + " subdirJob " + c1 + " " + c2 + " " + j + " " + c3;

			std::cout << "Running: " << cmd << std::endl;
			int result = std::system(cmd.c_str());
			if (result == 0) {
				remaining_jobs[i] = ""; // mark as processed
				++processed_count;

				if (fs::exists(temp_output / j)) {
					//write output data to zip  
					ZIPutil::Functions::zip_write_subfolder(out_archive_path, temp_output / j, j);
				}

				//clean up temp_files
				fs::remove_all(temp_input / j);
				fs::remove_all(temp_output / j);

				

			}
			else {
				std::cerr << "Subprocess failed for mesh: " << j << std::endl;
			}

			//overwirte jobfiel every 50 jobs to avoid timeouts and odd sized jobs 
			//when rescheduling and restarting
			if (processed_count % 50 == 0 || i == jobs.size() - 1) {
				std::vector<std::string> filtered_jobs;
				for (const auto& job : remaining_jobs) {
					if (!job.empty()) {
						filtered_jobs.push_back(job);
					}
				}

				jobUtilitiy::Functions::write_remaining_jobs(job_file, filtered_jobs);
				remaining_jobs = filtered_jobs;
			}
		}
		//remove temp dir 
		fs::remove_all(temp_dir);

		return 0;
	}

	int run_subdir_job(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir, fs::path process_location) {

		std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_dir);
		std::vector<std::string> remaining_jobs = jobs;

		size_t processed_count = 0;
		for (size_t i = 0; i < jobs.size(); ++i) {
			const std::string& j = jobs[i];

			std::string c0 = process_location.generic_string();
			std::string c1 = source.generic_string();
			std::string c2 = target.generic_string();
			std::string c3 = log_dir.generic_string();

			std::string cmd = c0 + " subdirJob " + c1 + " " + c2 + " " + j + " " + c3;

			std::cout << "Running: " << cmd << std::endl;
			int result = std::system(cmd.c_str());
			if (result == 0) {
				remaining_jobs[i] = ""; // mark as processed
				++processed_count;
			}
			else {
				std::cerr << "Subprocess failed for mesh: " << j << std::endl;
			}

            //overwirte jobfiel every 50 jobs to avoid timeouts and odd sized jobs 
			//when rescheduling and restarting
			if (processed_count % 50 == 0 || i == jobs.size() - 1) {
				std::vector<std::string> filtered_jobs;
				for (const auto& job : remaining_jobs) {
					if (!job.empty()) {
						filtered_jobs.push_back(job);
					}
				}

				jobUtilitiy::Functions::write_remaining_jobs(job_dir, filtered_jobs);
				remaining_jobs = filtered_jobs;
			}
		}

		return 0;
	}

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

	
}