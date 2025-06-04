#include "Scripts.h"
#include "../Tools/jobUtility.h"

#include <filesystem>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
namespace fs = std::filesystem;

namespace Scripts {

	int run_subdir_job(fs::path source, fs::path target, fs::path job_dir, fs::path log_dir, fs::path process_location) {

		std::vector<std::string> jobs = jobUtilitiy::Functions::read_job_file(job_dir);

		for (const std::string j : jobs) {
			std::string c0 = process_location.generic_string();
			std::string c1 = source.generic_string();
			std::string c2 = target.generic_string();
			std::string c3 = log_dir.generic_string();

			std::string cmd = c0 + " " + "--subdirJob" + " " + c1 + " " + c2 + " " + j + " " + c3;

			std::cout << "Running: " << cmd << std::endl;
			int result = std::system(cmd.c_str());
			if (result != 0) {
				std::cerr << "Subprocess failed for mesh: " << j << std::endl;
			}
		}

		return 0;
	}
}