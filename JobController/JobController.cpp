//INCLUDE - project
//INCLUDE external
#include <filesystem>
#include "Scripts/Scripts.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>


namespace fs = std::filesystem;

void usage(const char* progname) {
    fs::path progpath = progname;

    std::cout << "Usage:\n";
    std::cout << "  " << progpath.filename().generic_string() << " default <source_dir> <target_dir> <job_file> <log_dir> <process_exe>\n";
    std::cout << "  " << progpath.filename().generic_string() << " zip_mode <source_zip> <output_dir> <job_file> <log_dir> <process_exe>\n";
    std::cout << "  " << progpath.filename().generic_string() << " strip_obj <source_dir> <target_dir> <job_dir> <log_dir> <process_exe>\n";
    std::cout << "  " << progpath.filename().generic_string() << " dataset_stats <target_dir> <job_dir> <log_dir> <temp_file> <process_exe>\n";

    
    exit(1);
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage(argv[0]);
    }

    std::string mode = argv[1];

    std::cout << "Mode: " << mode << ", argc: " << argc << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    if (mode == "default") {
        if (argc != 7) {
            usage(argv[0]);
        }
        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path job_dir = argv[4];
        fs::path log_dir = argv[5];
        fs::path process = argv[6];

        int result = Scripts::run_subdir_job(source, target, job_dir, log_dir, process);
        std::cout << "Result: " << result << std::endl;
    }

    if (mode == "zip_mode") {
        if (argc != 7) {
            usage(argv[0]);
        }
        fs::path source_zip = argv[2];
        fs::path output_dir = argv[3];
        fs::path job_file = argv[4];
        fs::path log_dir = argv[5];
        fs::path process = argv[6];

        int result = Scripts::run_subdir_job_zip(source_zip, output_dir, job_file, log_dir, process);
        std::cout << "Result: " << result << std::endl;
    }

    if (mode == "strip_obj") {
        if (argc != 7) {
            usage(argv[0]);
        }
        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path job_dir = argv[4];
        fs::path log_dir = argv[5];
        fs::path process = argv[6];

        int result = Scripts::run_subdir_job_strip(source, target, job_dir, log_dir, process);
        std::cout << "Result: " << result << std::endl;
    }

    if (mode == "dataset_stats") {
        if (argc != 8) {
            usage(argv[0]);
        }
        fs::path target = argv[3];
        fs::path job_dir = argv[4];
        fs::path log_dir = argv[5];
        std::string temp_file = argv[6];
        fs::path process = argv[7];

        int result = Scripts::run_dataset_stats_job(target, job_dir, log_dir, temp_file, process);
        std::cout << "Result: " << result << std::endl;
    }




}

