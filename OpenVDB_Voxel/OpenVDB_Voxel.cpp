// INCLUDE - project
#include "include/LOG.h"
#include "include/Scripts.h"
// INCLUDE external
#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
namespace fs = std::filesystem;

void usage(const char* progname) {
    fs::path progpath = progname;

    std::cout << "Usage:\n";
    std::cout << "Basic processing modes:\n";
    std::cout << "  " << progpath.filename().generic_string() << " subdirJob <source_dir> <target_dir> <sub_dir_name> <log_dir>\n";
    std::cout << "  " << progpath.filename().generic_string() << " subdirJobStrip <source_dir> <target_dir> <sub_dir_name> <log_dir>\n";

    std::cout << "\nParameterized processing modes:\n";
    std::cout << "  " << progpath.filename().generic_string() << " vs_subdirJob <source_dir> <target_dir> <sub_dir_name> <log_dir> <kernelsize> <padding> <bandwidth> <voxelsize>\n";
    std::cout << "  " << progpath.filename().generic_string() << " nk_subdirJob <source_dir> <target_dir> <sub_dir_name> <log_dir> <kernelsize> <padding> <bandwidth> <n_k_min>\n";

    std::cout << "\nSegmentation modes:\n";
    std::cout << "  " << progpath.filename().generic_string() << " segAdaptive <obj_path> <target_dir> <log_dir> <n_min_kernel>\n";
    std::cout << "  " << progpath.filename().generic_string() << " segFixed <obj_path> <target_dir> <log_dir> <voxel_size>\n";
    std::cout << "  " << progpath.filename().generic_string() << " segFromVDB <vdb_path> <target_dir> <log_dir>\n";

    std::cout << "\nUtility modes:\n";
    std::cout << "  " << progpath.filename().generic_string() << " stats_subdir <source_dir> <target_dir> <sub_dir_name> <log_dir> <temp_file_name>\n";
    std::cout << "  " << progpath.filename().generic_string() << " exportVDB <source_dir> <out_file> <log_dir>\n";
    std::cout << "  " << progpath.filename().generic_string() << " test <source_path> <log_dir>\n";

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

    // ======== BASIC PROCESSING MODES ========

    if (mode == "subdirJob") {
        if (argc != 6) {
            std::cerr << "subdirJob mode requires exactly 6 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        std::string subdir = argv[4];
        fs::path log_dir = argv[5];

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_subdir_to_dataset(source, target, subdir);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    else if (mode == "subdirJobStrip") {
        if (argc != 6) {
            std::cerr << "subdirJobStrip mode requires exactly 6 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        std::string subdir = argv[4];
        fs::path log_dir = argv[5];

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_strip_obj_batch_job(source, target, subdir);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    // ======== PARAMETERIZED PROCESSING MODES ========

    else if (mode == "vs_subdirJob") {
        if (argc != 10) {
            std::cerr << "vs_subdirJob mode requires exactly 10 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        std::string subdir = argv[4];
        fs::path log_dir = argv[5];
        int kernel_size = std::stoi(argv[6]);
        int padding = std::stoi(argv[7]);
        int bandwidth = std::stoi(argv[8]);
        double voxel_size = std::stod(argv[9]);

        std::cout << "VS Parameters: kernel=" << kernel_size << ", padding=" << padding
            << ", bandwidth=" << bandwidth << ", voxel_size=" << voxel_size << std::endl;

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_subdir_to_dataset(source, target, subdir, kernel_size, padding, bandwidth, voxel_size);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    else if (mode == "nk_subdirJob") {
        if (argc != 10) {
            std::cerr << "nk_subdirJob mode requires exactly 10 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        std::string subdir = argv[4];
        fs::path log_dir = argv[5];
        int kernel_size = std::stoi(argv[6]);
        int padding = std::stoi(argv[7]);
        int bandwidth = std::stoi(argv[8]);
        int n_k_min = std::stoi(argv[9]);

        std::cout << "NK Parameters: kernel=" << kernel_size << ", padding=" << padding
            << ", bandwidth=" << bandwidth << ", n_k_min=" << n_k_min << std::endl;

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_subdir_to_dataset(source, target, subdir, kernel_size, padding, bandwidth, n_k_min);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    // ======== SEGMENTATION MODES ========

    else if (mode == "segAdaptive") {
        if (argc != 6) {
            std::cerr << "segAdaptive mode requires exactly 6 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path log_dir = argv[4];
        int n_min_kernel = std::stoi(argv[5]);

        std::cout << "Adaptive Segmentation Parameters: n_min_kernel=" << n_min_kernel << std::endl;

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_segmentation_adaptive(source, target, n_min_kernel);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    else if (mode == "segFixed") {
        if (argc != 6) {
            std::cerr << "segFixed mode requires exactly 6 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path log_dir = argv[4];
        double voxel_size = std::stod(argv[5]);

        std::cout << "Fixed Segmentation Parameters: voxel_size=" << voxel_size << std::endl;

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_segmentation_fixed(source, target, voxel_size);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    else if (mode == "segFromVDB") {
        if (argc != 5) {
            std::cerr << "segFromVDB mode requires exactly 5 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path log_dir = argv[4];

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_segmentation_on_vdb(source, target);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    // ======== UTILITY MODES ========

    else if (mode == "stats_subdir") {
        if (argc != 7) {
            std::cerr << "stats_subdir mode requires exactly 7 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        std::string subdir = argv[4];
        fs::path log_dir = argv[5];
        std::string temp_file_name = argv[6];

        std::cout << "Stats Parameters: temp_file=" << temp_file_name << std::endl;

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_stats_on_subdir(source, target, subdir, temp_file_name);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    else if (mode == "exportVDB") {
        if (argc != 5) {
            std::cerr << "exportVDB mode requires exactly 5 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path log_dir = argv[4];

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_export_vdb(source, target);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
    }

    else if (mode == "test") {
        if (argc != 4) {
            std::cerr << "test mode requires exactly 4 arguments" << std::endl;
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path log_dir = argv[3];

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Tests::run_grid_test(source);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return result;
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

Basic processing:
./YourProgram subdirJob /source /target subdir_name /logs
./YourProgram subdirJobStrip /source /target subdir_name /logs

Parameterized processing:
./YourProgram vs_subdirJob /source /target subdir_name /logs 10 5 3 0.1
./YourProgram nk_subdirJob /source /target subdir_name /logs 10 5 3 100

Segmentation:
./YourProgram segAdaptive /path/to/obj /target /logs 50
./YourProgram segFixed /path/to/obj /target /logs 0.05
./YourProgram segFromVDB /path/to/vdb /target /logs

Utilities:
./YourProgram stats_subdir /source /target subdir_name /logs temp.txt
./YourProgram exportVDB /source /output.vdb /logs
./YourProgram test /source /logs
*/