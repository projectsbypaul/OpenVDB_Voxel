//INCLUDE - project
#include "LOG.h"
#include "Scripts.h"
//INCLUDE external
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    // Safely print initial argc
    std::cout << "Running main() with " << argc << " arguments (program name + " << argc - 1 << " user arguments)" << std::endl;

    // Safely print each argument
    if (argc > 0) {
        std::cout << "process (argv[0]): " << argv[0] << std::endl;
    }
    if (argc > 1) {
        std::cout << "Potential log file (argv[1]): " << argv[1] << std::endl;
    }
    if (argc > 2) {
        std::cout << "Potential source dir (argv[2]): " << argv[2] << std::endl;
    }
    if (argc > 3) {
        std::cout << "Potential target dir (argv[3]): " << argv[3] << std::endl;
    }
    if (argc > 4) {
        std::cout << "Potential job location (argv[4]): " << argv[4] << std::endl;
    }
    // Add more if you expect more, always checking argc

    std::cout << "--- Finished printing received arguments ---" << std::endl;

    if (argc == 5) {
        std::cout << "[INFO] Correct number of arguments ("<< argc << " user arguments) received. Proceeding." << std::endl;
        initLogger(argv[1]); // Use argv[1] as the log file path
        LOG_FUNC("ENTER");
        //Scripts::ABCtoDatasetBatch(argv[2], argv[3]); // Use argv[2] and argv[3]
        Scripts::ABCtoDatasetBatchJobTimed(argv[2], argv[3], argv[4]);
        LOG_FUNC("EXIT");
    }
    else {
        std::cerr << "[ERROR] Incorrect number of arguments(" << argc << " user arguments) received!" << std::endl;
        std::cerr << "Expected 3 user arguments (argc should be 4), but received " << argc - 1 << " user arguments (argc is " << argc << ")." << std::endl;
        std::cerr << "Usage: " << (argc > 0 ? argv[0] : "program.exe") << " <log_file_path> <source_directory> <target_directory>" << std::endl;
        // Optionally, print the arguments that were received if any:
        if (argc > 0) {
            std::cerr << "Arguments received were:" << std::endl;
            for (int i = 0; i < argc; ++i) {
                std::cerr << "  argv[" << i << "]: " << argv[i] << std::endl;
            }
        }
        return 1; // Indicate an error
    }

    std::cout << "[INFO] Program finished successfully." << std::endl;
    return 0;


}

