//INCLUDE - project
//INCLUDE external
#include <filesystem>
#include "include/Scripts.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
namespace fs = std::filesystem;

void usage(const char* progname) {
    fs::path progpath = progname;
    std::cout << "Usage:\n";
    std::cout << "Basic modes:\n";
    std::cout << "  " << progpath.filename().generic_string() << " default <source_dir> <target_dir> <job_file> <log_dir> <process_exe>\n";
    std::cout << "  " << progpath.filename().generic_string() << " zip_mode <source_zip> <output_dir> <job_file> <log_dir> <process_exe>\n";
    std::cout << "  " << progpath.filename().generic_string() << " strip_obj <source_dir> <target_dir> <job_dir> <log_dir> <process_exe>\n";
    std::cout << "  " << progpath.filename().generic_string() << " dataset_stats <target_dir> <job_dir> <log_dir> <temp_file> <process_exe>\n";

    std::cout << "\nVoxel Size (VS) modes:\n";
    std::cout << "  " << progpath.filename().generic_string() << " vs_default <source_dir> <target_dir> <job_file> <log_dir> <process_exe> <kernel_size> <padding> <bandwidth> <voxel_size>\n";
    std::cout << "  " << progpath.filename().generic_string() << " vs_zip_mode <source_zip> <output_dir> <job_file> <log_dir> <process_exe> <kernel_size> <padding> <bandwidth> <voxel_size>\n";

    std::cout << "\nN_K_Min (NK) modes:\n";
    std::cout << "  " << progpath.filename().generic_string() << " nk_default <source_dir> <target_dir> <job_file> <log_dir> <process_exe> <kernel_size> <padding> <bandwidth> <n_k_min>\n";
    std::cout << "  " << progpath.filename().generic_string() << " nk_zip_mode <source_zip> <output_dir> <job_file> <log_dir> <process_exe> <kernel_size> <padding> <bandwidth> <n_k_min>\n";

    std::cout << "\nUniversal mode:\n";
    std::cout << "  " << progpath.filename().generic_string() << " universal <job_type> <source_dir> <target_dir> <job_file> <log_dir> <process_exe> [<kernel_size> <padding> <bandwidth> <voxel_size|n_k_min>]\n";
    std::cout << "    where job_type can be: basic, vs, nk\n";

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

    // ======== EXISTING BASIC MODES ========

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
        return result;
    }

    else if (mode == "zip_mode") {
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
        return result;
    }

    else if (mode == "strip_obj") {
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
        return result;
    }

    else if (mode == "dataset_stats") {
        if (argc != 8) {
            usage(argv[0]);
        }
        fs::path target = argv[2];
        fs::path job_dir = argv[3];
        fs::path log_dir = argv[4];
        std::string temp_file = argv[5];
        fs::path process = argv[6];

        int result = Scripts::run_dataset_stats_job(target, job_dir, log_dir, temp_file, process);
        std::cout << "Result: " << result << std::endl;
        return result;
    }

    // ======== NEW VOXEL SIZE (VS) MODES ========

    else if (mode == "vs_default") {
        if (argc != 11) {
            usage(argv[0]);
        }
        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path job_file = argv[4];
        fs::path log_dir = argv[5];
        fs::path process = argv[6];
        int kernel_size = std::stoi(argv[7]);
        int padding = std::stoi(argv[8]);
        int bandwidth = std::stoi(argv[9]);
        double voxel_size = std::stod(argv[10]);

        std::cout << "VS Parameters: kernel=" << kernel_size << ", padding=" << padding
            << ", bandwidth=" << bandwidth << ", voxel_size=" << voxel_size << std::endl;

        int result = Scripts::run_subdir_job_vs(source, target, job_file, log_dir, process,
            kernel_size, padding, bandwidth, voxel_size);
        std::cout << "Result: " << result << std::endl;
        return result;
    }

    else if (mode == "vs_zip_mode") {
        if (argc != 11) {
            usage(argv[0]);
        }
        fs::path source_zip = argv[2];
        fs::path output_dir = argv[3];
        fs::path job_file = argv[4];
        fs::path log_dir = argv[5];
        fs::path process = argv[6];
        int kernel_size = std::stoi(argv[7]);
        int padding = std::stoi(argv[8]);
        int bandwidth = std::stoi(argv[9]);
        double voxel_size = std::stod(argv[10]);

        std::cout << "VS ZIP Parameters: kernel=" << kernel_size << ", padding=" << padding
            << ", bandwidth=" << bandwidth << ", voxel_size=" << voxel_size << std::endl;

        int result = Scripts::run_subdir_job_zip_vs(source_zip, output_dir, job_file, log_dir, process,
            kernel_size, padding, bandwidth, voxel_size);
        std::cout << "Result: " << result << std::endl;
        return result;
    }

    // ======== NEW N_K_MIN (NK) MODES ========

    else if (mode == "nk_default") {
        if (argc != 11) {
            usage(argv[0]);
        }
        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path job_file = argv[4];
        fs::path log_dir = argv[5];
        fs::path process = argv[6];
        int kernel_size = std::stoi(argv[7]);
        int padding = std::stoi(argv[8]);
        int bandwidth = std::stoi(argv[9]);
        int n_k_min = std::stoi(argv[10]);

        std::cout << "NK Parameters: kernel=" << kernel_size << ", padding=" << padding
            << ", bandwidth=" << bandwidth << ", n_k_min=" << n_k_min << std::endl;

        int result = Scripts::run_subdir_job_nk(source, target, job_file, log_dir, process,
            kernel_size, padding, bandwidth, n_k_min);
        std::cout << "Result: " << result << std::endl;
        return result;
    }

    else if (mode == "nk_zip_mode") {
        if (argc != 11) {
            usage(argv[0]);
        }
        fs::path source_zip = argv[2];
        fs::path output_dir = argv[3];
        fs::path job_file = argv[4];
        fs::path log_dir = argv[5];
        fs::path process = argv[6];
        int kernel_size = std::stoi(argv[7]);
        int padding = std::stoi(argv[8]);
        int bandwidth = std::stoi(argv[9]);
        int n_k_min = std::stoi(argv[10]);

        std::cout << "NK ZIP Parameters: kernel=" << kernel_size << ", padding=" << padding
            << ", bandwidth=" << bandwidth << ", n_k_min=" << n_k_min << std::endl;

        int result = Scripts::run_subdir_job_zip_nk(source_zip, output_dir, job_file, log_dir, process,
            kernel_size, padding, bandwidth, n_k_min);
        std::cout << "Result: " << result << std::endl;
        return result;
    }

    // ======== UNIVERSAL MODE (FLEXIBLE) ========

    else if (mode == "universal") {
        if (argc < 8) {
            usage(argv[0]);
        }

        std::string job_type = argv[2];
        fs::path source = argv[3];
        fs::path target = argv[4];
        fs::path job_file = argv[5];
        fs::path log_dir = argv[6];
        fs::path process = argv[7];

        if (job_type == "basic") {
            if (argc != 8) {
                std::cerr << "Basic universal mode requires exactly 8 arguments" << std::endl;
                usage(argv[0]);
            }

            int result = Scripts::run_universal_subdir_job(source, target, job_file, log_dir, process, "basic");
            std::cout << "Universal Basic Result: " << result << std::endl;
            return result;
        }
        else if (job_type == "vs") {
            if (argc != 12) {
                std::cerr << "VS universal mode requires exactly 12 arguments" << std::endl;
                usage(argv[0]);
            }

            int kernel_size = std::stoi(argv[8]);
            int padding = std::stoi(argv[9]);
            int bandwidth = std::stoi(argv[10]);
            double voxel_size = std::stod(argv[11]);

            int result = Scripts::run_universal_subdir_job(source, target, job_file, log_dir, process, "vs",
                kernel_size, padding, bandwidth, voxel_size);
            std::cout << "Universal VS Result: " << result << std::endl;
            return result;
        }
        else if (job_type == "nk") {
            if (argc != 12) {
                std::cerr << "NK universal mode requires exactly 12 arguments" << std::endl;
                usage(argv[0]);
            }

            int kernel_size = std::stoi(argv[8]);
            int padding = std::stoi(argv[9]);
            int bandwidth = std::stoi(argv[10]);
            int n_k_min = std::stoi(argv[11]);

            int result = Scripts::run_universal_subdir_job(source, target, job_file, log_dir, process, "nk",
                kernel_size, padding, bandwidth, 0.0, n_k_min);
            std::cout << "Universal NK Result: " << result << std::endl;
            return result;
        }
        else {
            std::cerr << "Unknown job_type: " << job_type << ". Use 'basic', 'vs', or 'nk'" << std::endl;
            usage(argv[0]);
        }
    }

    // ======== ERROR HANDLING ========

    else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        usage(argv[0]);
    }

    return 1; // Should not reach here
}

/*
USAGE EXAMPLES:

Basic modes (existing):
./JobController default /source /target jobs.txt /logs ./OpenVDB_Voxel
./JobController zip_mode source.zip /output jobs.txt /logs ./OpenVDB_Voxel
./JobController strip_obj /source /target jobs.txt /logs ./OpenVDB_Voxel
./JobController dataset_stats /target jobs.txt /logs temp.txt ./OpenVDB_Voxel

New VS modes:
./JobController vs_default /source /target jobs.txt /logs ./OpenVDB_Voxel 10 5 3 0.1
./JobController vs_zip_mode source.zip /output jobs.txt /logs ./OpenVDB_Voxel 10 5 3 0.1

New NK modes:
./JobController nk_default /source /target jobs.txt /logs ./OpenVDB_Voxel 10 5 3 100
./JobController nk_zip_mode source.zip /output jobs.txt /logs ./OpenVDB_Voxel 10 5 3 100

Universal modes:
./JobController universal basic /source /target jobs.txt /logs ./OpenVDB_Voxel
./JobController universal vs /source /target jobs.txt /logs ./OpenVDB_Voxel 10 5 3 0.1
./JobController universal nk /source /target jobs.txt /logs ./OpenVDB_Voxel 10 5 3 100
*/