#ifndef PROCESSFORDLLDATASET_H
#define PROCESSFORDLLDATASET_H

#include "GenericDirectoryProcess.h"
#include <string>

namespace ProcessingUtility {

    /// <summary>
    ///Implementation of ProcessForDLLDataset
    /// </summary>
    /**
     * @brief A class for processing DLL dataset within a specific subdirectory.
     *
     * This class inherits from GenericDirectoryProcess and adds a specific
     * parameters for ...
     */
    class ProcessForDLLDataset : public GenericDirectoryProcess {
    private:
        int kernel_size_;
        int padding_;
        int bandwidth_;
        int n_min_kernel_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessForDLLDataset(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, int n_min_kernel);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;
    };
}
#endif // PROCESSFORDLLDATASET_H