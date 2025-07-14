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


namespace Scripts {
    //Work Scripts
    int run_stats_on_subdir(fs::path source, fs::path target, std::string subdir_name, std::string temp_file_name) {
        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessGetStats process(source, target, temp_file_name);
        processOnSubdirTimedNoCheck(&process, subdir_name, 1, 5);

        LOG_FUNC("EXIT");

        return 0;

    }

    int run_segmentation_fixed(fs::path source, fs::path target, double voxel_size) {

        LOG_FUNC("ENTER");

        int kernel_size = 16;
        int padding = 4;
        int bandwidth = 5;
        int max_threads = 1;
        int openvdb_threads = 1;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        LOG_FUNC("ENTER");

        ProcessingUtility::ProcessSimpleSegmentation process(source, target, kernel_size, padding, bandwidth, voxel_size);
        process.run();

        LOG_FUNC("EXIT");

        return 0;
    }

    int run_segmentation_adaptive(fs::path source, fs::path target, int n_k_min) {


        int kernel_size = 16;
        int padding = 4;
        int bandwidth = 5;
        int max_threads = 10;
        int openvdb_threads = 10;

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
}//namespace Scripts