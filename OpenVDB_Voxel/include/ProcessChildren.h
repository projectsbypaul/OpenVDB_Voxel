#ifndef PROCESSFORDLLDATASET_H
#define PROCESSFORDLLDATASET_H

#include "GenericDirectoryProcess.h"
#include <string>
#include <vector>

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
        double voxel_size_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessForDLLDataset(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, int n_min_kernel);

        ProcessForDLLDataset(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, double voxel_size);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;
    };

    /// <summary>
    ///Implementation of ProcessForDLLDataset
    /// </summary>
    /**
     * @brief A class for processing DLL dataset within a specific subdirectory.
     *
     * This class inherits from GenericDirectoryProcess and adds a specific
     * parameters for ...
     */
    class ProcessWithDumpTruck : public GenericDirectoryProcess {
    private:
        int kernel_size_;
        int padding_;
        int bandwidth_;
        int n_min_kernel_;
        double voxel_size_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessWithDumpTruck(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, int n_min_kernel);

        ProcessWithDumpTruck(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, double voxel_size);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;
    };

    /// <summary>
    ///Implementation of ProcessForDLLDataset
    /// </summary>
    /**
     * @brief A class for processing DLL dataset within a specific subdirectory.
     *
     * This class inherits from GenericDirectoryProcess and adds a specific
     * parameters for ...
     */
    class ProcessForDLLDatasetAE : public GenericDirectoryProcess {
    private:
        int kernel_size_;
        int padding_;
        int bandwidth_;
        int n_min_kernel_;
        double voxel_size_;
        double param_1_;
        double param_2_;
        double threshold_;
        int random_seed_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessForDLLDatasetAE(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int n_min_kernel, int bandwidth, int padding, double param_1, double param_2, double threshold, int random_seed);

        ProcessForDLLDatasetAE(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, double voxel_size, int bandwidth, int padding, double param_1, double param_2, double threshold, int random_seed);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;
    };

    /// <summary>
    ///Implementation of ProcessForDLLDataset
    /// </summary>
    /**
     * @brief A class for processing DLL dataset within a specific subdirectory.
     *
     * This class inherits from GenericDirectoryProcess and adds a specific
     * parameters for ...
     */
    class ProcessObjStrip : public GenericDirectoryProcess {
    private:
        std::vector<std::string> filter_ = { "vc" };
        std::string extension_ = ".obj";
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessObjStrip(const fs::path& sourceDir, const fs::path& targetDir);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;

    };

    class ProcessSimpleSegmentation : public GenericDirectoryProcess {
    private:
        int kernel_size_;
        int padding_;
        int bandwidth_;
        int n_min_kernel_;
        double voxel_size_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessSimpleSegmentation(const fs::path& sourceDir, const fs::path& targetDir,  int kernel_size, int padding, int bandwidth, int n_min_kernel);

        ProcessSimpleSegmentation(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, double voxel_size);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;

    };

    class ProcessSegmentationFromVDB : public GenericDirectoryProcess {
    private:
        int kernel_size_;
        int padding_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessSegmentationFromVDB(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;

    };


    /// <summary>
    ///Implementation of ProcessForDLLDataset
    /// </summary>
    /**
     * @brief A class for processing DLL dataset within a specific subdirectory.
     *
     * This class inherits from GenericDirectoryProcess and adds a specific
     * parameters for ...
     */
    class ProcessGetStats : public GenericDirectoryProcess {
    private:
        std::string temp_file_name_;
        std::string target_file_name_ = "segmentation_data.dat";
        std::string target_bin_file_name_ = "segmentation_data_segments.bin";
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessGetStats(const fs::path& sourceDir, const fs::path& targetDir, std::string temp_file_name);
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