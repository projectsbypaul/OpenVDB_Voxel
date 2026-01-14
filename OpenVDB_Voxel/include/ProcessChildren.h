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
        //sdf parameters
        int kernel_size_;
        int padding_;
        int bandwidth_;
        int n_min_kernel_;
        double voxel_size_;
        int segment_limit_;
        //augmentations
        bool apply_random_rotatio_ = false;
        float rotation_probability_ = 0.5f;

        bool apply_random_flip_ = false;
        float flip_probability_ = 0.5f;

        bool apply_random_scale_ = false;
        float scale_probability_ = 0.5f;
        float scaling_magnitude_ = 0.1;

        bool apply_origin_jitter_ = false;
        float jitter_probability_ = 1.0f;
        int jitter_magnitude_ = 2; 

        bool apply_sdf_noise_ = false;
        float noise_stdv_ = 0.025;

    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessWithDumpTruck(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, int n_min_kernel, int segment_limit = 250);

        ProcessWithDumpTruck(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, double voxel_size, int segment_limit = 250);
        /**
         * @brief Executes the DLL dataset processing logic for the specified subdirectory.
         *
         * @param subDirPath The specific subdirectory path within the source directory to process.
         * @override
         */
        void run(const std::string& subDirName = "") override;

        //Setter for process parameters
        void set_voxel_size(double voxel_size) {
            voxel_size_ = voxel_size;
        }

        //Setters for augmentation 
        void set_apply_random_rotation(bool apply_rotation) {
            apply_random_rotatio_ = apply_rotation;
        }

        void set_rotation_probability(float probability) {
            rotation_probability_ = probability;
        }

        void set_apply_random_flip(bool apply_flip) {
            apply_random_flip_ = apply_flip;
        }

        void set_flip_probability(float probability) {
            flip_probability_ = probability;
        }

        void set_apply_random_scale(bool apply_scale) {
            apply_random_scale_ = apply_scale;
        }

        void set_scale_probability(float probability) {
            scale_probability_ = probability;
        }
        void set_scaling_magnitude(float magnitude) {
            scaling_magnitude_ = magnitude;
        }


        void set_apply_origin_jitter(bool apply_jitter) {
            apply_origin_jitter_ = apply_jitter;
        }

        void set_jitter_probability(float probability) {
            jitter_probability_ = probability;
        }

        void set_jitter_magnitude(int magnitude) {
            jitter_magnitude_ = magnitude;
        }

        void set_apply_sdf_noise(bool apply_noise) {
            apply_sdf_noise_ = apply_noise;
        }

        void set_noise_sdtv(float noise_sdtv) {
            noise_stdv_ = noise_sdtv;
        }
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
    class ProcessLabelling : public GenericDirectoryProcess {
    private:
        std::string label_template_;
        float surface_threshold_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessLabelling(const fs::path& sourceDir, const fs::path& targetDir, const std::string label_template, const float surface_threshold = 1.0f);
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

    /// <summary>
    ///Implementation of ProcessForDLLDataset
    /// </summary>
    /**
     * @brief A class for processing DLL dataset within a specific subdirectory.
     *
     * This class inherits from GenericDirectoryProcess and adds a specific
     * parameters for ...
     */
    class ProcessPurgeBySurfType : public GenericDirectoryProcess {
    private:
        std::vector<std::string> filter_;
    public:
        /**
         * @brief Constructs a ProcessForDLLDataset object.
         *
         * @param sourceDir The path to the main source directory.
         * @param targetDir The path to the main target directory.
         */
        ProcessPurgeBySurfType(const fs::path& sourceDir, const fs::path& targetDir, std::vector<std::string>& filter);
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
}// PROCESSFORDLLDATASET_H
#endif 