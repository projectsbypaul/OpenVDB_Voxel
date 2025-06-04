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
    std::cout << "  " << progpath.filename().generic_string() << " --default <source_dir> <target_dir> <job_dir> <log_dir> <process_exe>\n";
    exit(1);
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage(argv[0]);
    }

    std::string mode = argv[1];

    if (mode == "--default") {
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




}

