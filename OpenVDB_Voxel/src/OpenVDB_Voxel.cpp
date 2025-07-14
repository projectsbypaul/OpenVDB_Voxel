// INCLUDE - project
#include "../include/LOG.h"
#include "../include/Scripts.h"
// INCLUDE external
#include <iostream>
#include <filesystem>
#include <string>   // for std::stod
namespace fs = std::filesystem;

void usage(const char* progname) {
    fs::path progpath = progname;

    std::cout << "Usage:\n";
    std::cout << "  " << progpath.filename().generic_string() << " subdirJob <source_dir> <target_dir> <sub_dir_name> <log_dir>\n";
    std::cout << "  " << progpath.filename().generic_string() << " subdirJobStrip <source_dir> <target_dir> <sub_dir_name> <log_dir>\n";
    std::cout << "  " << progpath.filename().generic_string() << " segAdaptive <obj_path> <target_dir> <log_dir> <n_min_kernel>\n";
    std::cout << "  " << progpath.filename().generic_string() << " segFixed <obj_path> <target_dir> <log_dir> <voxel_size>\n";
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

    if (mode == "subdirJob") {
        if (argc != 6) {
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
        return 0;
    }
    else if (mode == "subdirJobStrip") {
        if (argc != 6) {
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
        return 0;
    }
    else if (mode == "segAdaptive") {
        if (argc != 6) {
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path log_dir = argv[4];
        int n_min_kernel = std::stoi(argv[5]);

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_segmentation_adaptive(source, target, n_min_kernel);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return 0;
    }
    else if (mode == "segFixed") {
        if (argc != 6) {
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path log_dir = argv[4];
        double voxel_size = std::stod(argv[5]);

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_segmentation_fixed(source, target, voxel_size);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return 0;
    }
    else if (mode == "stats_subdir") {
        if (argc != 7) {
            usage(argv[0]);
        }

        fs::path source = argv[2];
        fs::path target = argv[3];
        std::string subdir = argv[4];
        fs::path log_dir = argv[5];
        std::string temp_file_name = argv[6];

        initLogger(log_dir.generic_string());
        LOG_FUNC("ENTER");

        int result = Scripts::run_stats_on_subdir(source, target, subdir, temp_file_name);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
        return 0;
    }
    else {
        usage(argv[0]); // Unrecognized mode
    }

    return 1; // Should not reach here
}
