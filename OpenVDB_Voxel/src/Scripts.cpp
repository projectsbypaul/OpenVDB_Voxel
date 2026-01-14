#include "../include/Tools.h"
#include <memory>
#include <openvdb/tools/Composite.h>
#include "../include/MeshDataHandling.h"
#include "../include/Scripts.h"

#include <CGAL/Bbox_3.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

#include <filesystem>
#include "../include/DatasetToVoxel.h"

#include "../include/DL_Preprocessing.h"
#include "../include/GetABCStatistics.h"
#include "../include/ABCProcessing.h"

#include "../include/LOG.h"
#include <tbb/global_control.h>

#include "../include/NoiseOnMesh.h"
#include "../include/ProcessChildren.h"
#include "../include/DatasetStats.h"

#include "../include/DataContainer.h"


namespace Scripts {
    //Work Scripts
    int run_purge_obj_by_surf_type(fs::path source, fs::path target, fs::path job_location) {

        int max_threads = 1;
        std::vector<std::string> filter = { "Revolution", "Extrusion",  "Other"};

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessPurgeBySurfType process(source, target, filter);
        processOnSubdirTimedNoCheck(&process, job_location, max_threads);

        LOG_FUNC("EXIT");

        return 0;
    }


    int run_stats_on_subdir(fs::path source, fs::path target, std::string subdir_name, std::string temp_file_name) {
        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessGetStats process(source, target, temp_file_name);
        processOnSubdirTimedNoCheck(&process, subdir_name, 1, 5);

        LOG_FUNC("EXIT");

        return 0;

    }

    int run_segmentation_on_vdb(fs::path source, fs::path target, int kernel_size, int padding, int openvdb_threads) {

        LOG_FUNC("ENTER");

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessSegmentationFromVDB process(source, target, kernel_size, padding);
        process.run();

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_segmentation_fixed(fs::path source, fs::path target, int kernel_size, int padding, int bandwidth, double voxel_size) {

        LOG_FUNC("ENTER");

        int max_threads = 18;
        int openvdb_threads = 18;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessSimpleSegmentation process(source, target, kernel_size, padding, bandwidth, voxel_size);
        process.run();

        LOG_FUNC("EXIT");
        
        return 0;
    }



    int run_segmentation_adaptive(fs::path source, fs::path target, int kernel_size, int padding, int bandwidth , int n_k_min) {


        int max_threads = 18;
        int openvdb_threads = 18;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessSimpleSegmentation process(source, target, kernel_size, padding, bandwidth, n_k_min);
        process.run();

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_subdir_to_dataset(fs::path source, fs::path target, std::string subdir_name) {

        int kernel_size = 16;
        int padding = 4;
        int bandwidth = 5;
        double voxel_size = 1.0;
        int n_k_min = 3;
        int max_threads = 1;
        int openvdb_threads = 1;
        int timeout_min = 5;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessWithDumpTruck process_dump(source, target, kernel_size, padding, bandwidth, n_k_min);
        processOnSubdirTimed(&process_dump, subdir_name, max_threads, 5);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_subdir_to_dataset(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, int n_k_min, int max_threads, int openvdb_threads, int timeout_min) {

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessWithDumpTruck process_dump(source, target, kernel_size, padding, bandwidth, n_k_min, 1500);

        processOnSubdirTimed(&process_dump, subdir_name, max_threads, timeout_min);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_subdir_to_dataset_rand_aug(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, int n_k_min, int max_threads, int openvdb_threads, int timeout_min) {

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessWithDumpTruck process_dump(source, target, kernel_size, padding, bandwidth, n_k_min, 1500);
        process_dump.set_apply_random_rotation(true);
        process_dump.set_rotation_probability(1.0);
        process_dump.set_apply_random_flip(true);
        process_dump.set_flip_probability(1.0);
        process_dump.set_apply_random_scale(true);
        process_dump.set_scale_probability(1.0);
        process_dump.set_apply_sdf_noise(true);
        process_dump.set_noise_sdtv(0.025);
        process_dump.set_apply_origin_jitter(true);
        process_dump.set_jitter_magnitude(2);

        processOnSubdirTimed(&process_dump, subdir_name, max_threads, timeout_min);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_subdir_to_dataset(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, double voxel_size, int max_threads, int openvdb_threads, int timeout_min) {

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessWithDumpTruck process_dump(source, target, kernel_size, padding, bandwidth, voxel_size, 1500);
        processOnSubdirTimed(&process_dump, subdir_name, max_threads, timeout_min);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_subdir_to_dataset_rand_aug(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, double voxel_size, int max_threads, int openvdb_threads, int timeout_min) {

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessWithDumpTruck process_dump(source, target, kernel_size, padding, bandwidth, voxel_size, 1500);
        process_dump.set_apply_random_rotation(true);
        process_dump.set_rotation_probability(1.0);
        process_dump.set_apply_random_flip(true);
        process_dump.set_flip_probability(1.0);
        process_dump.set_apply_random_scale(true);
        process_dump.set_scale_probability(1.0);
        process_dump.set_scaling_magnitude(0.1);
        process_dump.set_apply_sdf_noise(true);
        process_dump.set_noise_sdtv(0.025);
        process_dump.set_apply_origin_jitter(true);
        process_dump.set_jitter_magnitude(2);

        processOnSubdirTimed(&process_dump, subdir_name, max_threads, timeout_min);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_subdir_to_dataset_maxseg(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, int n_k_min, int max_threads, int openvdb_threads, int timeout_min, int seglimit)
    {
        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessWithDumpTruck process_dump(source, target, kernel_size, padding, bandwidth, n_k_min, seglimit);
        processOnSubdirTimed(&process_dump, subdir_name, max_threads, timeout_min);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_subdir_to_dataset_maxseg(fs::path source, fs::path target, std::string subdir_name, int kernel_size, int padding, int bandwidth, double voxel_size, int max_threads, int openvdb_threads, int timeout_min, int seglimit)
    {
        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessWithDumpTruck process_dump(source, target, kernel_size, padding, bandwidth, voxel_size, seglimit);
        processOnSubdirTimed(&process_dump, subdir_name, max_threads, timeout_min);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_strip_obj_batch_job(fs::path source, fs::path target, fs::path job_location) {

        int max_threads = 1;
        int openvdb_threads = 1;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessObjStrip process(source, target);
        processOnSubdirTimed(&process, job_location, max_threads);

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_export_vdb(fs::path source, fs::path out_file) {

        LOG_FUNC("ENTER");

        Tools::Macros::export_prediction_vdb(source.generic_string(), out_file.generic_string());

        LOG_FUNC("EXIT");
        return 0;
    }
    int run_labels_from_subdir(fs::path source, fs::path target, std::string subdir_name, std::string class_tempalte, float surface_threshold)
    {
        int max_threads = 1;
        int openvdb_threads = 1;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessLabelling process(source, target, class_tempalte, surface_threshold);
        processOnSubdirTimedNoCheck(&process, subdir_name);

        LOG_FUNC("EXIT");
        return 0;
    }
}//namespace Scripts

namespace Tests {
    int run_rw_bin_test() {
        fs::path LoadDir = R"(H:\ws_seg_debug\debug_output\00000004)";
        fs::path DumpDir = R"(H:\ws_seg_debug\debug_output\00000004_reloaded)";
        cppIOUtility::SegmentationDataContainer test_container;
        test_container.load(LoadDir);

        test_container.dump(DumpDir);
        return 0;
    }

    int run_grid_test() {
        fs::path filename = "";
        Tools::Macros::test_grid_vdb(filename);
        return 0;
    }
}//namespace Test

