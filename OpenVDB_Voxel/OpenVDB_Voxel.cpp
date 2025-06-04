//INCLUDE - project
#include "LOG.h"
#include "Scripts.h"
//INCLUDE external
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

void usage(const char* progname) {
    fs::path progpath = progname;

    std::cout << "Usage:\n";
    std::cout << "  " << progpath.filename().generic_string() << " --batchJob <source_dir> <target_dir> <job_loc> <log_dir>\n";
    std::cout << "  " << progpath.filename().generic_string() << " --subdirJob <source_dir> <target_dir> <sub_dir_name> <log_dir>\n";
    exit(1);
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage(argv[0]);
    }

    std::string mode = argv[1];

    if (mode == "--batchJob") {
        if (argc != 6) {
            usage(argv[0]);
        }
        fs::path source = argv[2];
        fs::path target = argv[3];
        fs::path job_loc = argv[4];
        fs::path log_dir = argv[5];

        initLogger(log_dir.generic_string());

        LOG_FUNC("ENTER");

        int result = Scripts::ABCtoDatasetBatchJobTimed(source, target, job_loc);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");
    }

    if (mode == "--subdirJob") {
        if (argc != 6) {
            usage(argv[0]);
        }
        fs::path source = argv[2];
        fs::path target = argv[3];
        std::string subdir = argv[4];
        fs::path log_dir = argv[5];

        initLogger(log_dir.generic_string());

        LOG_FUNC("ENTER");

        int result = Scripts::SubdirToDataset(source, target, subdir);
        std::cout << "Result: " << result << std::endl;

        LOG_FUNC("EXIT");

    }




}

