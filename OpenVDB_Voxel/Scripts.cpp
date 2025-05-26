#include "Tools.h"
#include <memory>
#include <openvdb/tools/Composite.h>
#include "MeshDataHandling.h"
#include "Scripts.h"

#include <CGAL/Bbox_3.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

#include <filesystem>
#include "DatasetToVoxel.h"

#include "DL_Preprocessing.h"
#include "GetABCStatistics.h"
#include "ABCProcessing.h"

#include "LOG.h"
#include <tbb/global_control.h>

#include "NoiseOnMesh.h"
#include "ProcessChildren.h"

namespace Scripts {
    //Work Scripts
    int ABCtoDataset() {


        LOG_FUNC("ENTER");

        fs::path Source = R"(C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark)";
        fs::path Target_dump = R"(C:\Local_Data\ABC\ABC_statistics\benchmarks\ABC_chunk_benchmark_dumptruck)";
        fs::path Target_default = R"(C:\Local_Data\ABC\ABC_statistics\benchmarks\ABC_chunk_benchmark_default)";

        int kernel_size = 16;
        int padding = 4;
        int bandwidth = 5;
        double voxel_size = 0.5;
        int n_k_min = 2;
        int max_threads;
        int openvdb_threads = 1;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        for (int i = 1; i < 7; i++) {

            fs::remove_all(Target_dump);
            fs::remove_all(Target_default);

            max_threads = i;

            LOG_FUNC("ENTER_dump_" + std::to_string(max_threads));

            ProcessingUtility::ProcessWithDumpTruck process_dump(Source, Target_dump, kernel_size, padding, bandwidth, n_k_min);
            parseABCtoDataset(&process_dump, max_threads);

            LOG_FUNC("EXIT_dump_" + std::to_string(max_threads));


            LOG_FUNC("ENTER_default_" + std::to_string(max_threads));

            ProcessingUtility::ProcessForDLLDataset process(Source, Target_default, kernel_size, padding, bandwidth, n_k_min);
            parseABCtoDataset(&process, max_threads);

            LOG_FUNC("EXIT_default_" + std::to_string(max_threads));

            LOG_FUNC("EXIT");

        }

        return 0;
    }
    int ABCtoDatasetAE() {
        LOG_FUNC("ENTER");

        fs::path Source = R"(C:\Local_Data\ABC\ABC_parsed_files)";
        fs::path Target = R"(C:\Local_Data\ABC\ABC_Data_ks_16_pad_4_bw_5_vs_adaptive_test)";

        int kernel_size = 16;
        int padding = 4;
        int bandwidth = 5;
        double voxel_size = 0.5;
        int n_k_min = 2;
        int max_threads = 1;
        int openvdb_threads = 1;

        double param_1 = 2.0;
        double param_2 = 2.0;
        double threshold = 0.25;
        int seed = 42;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, openvdb_threads);
        openvdb::initialize();

        ProcessingUtility::ProcessForDLLDatasetAE process(Source, Target, kernel_size, n_k_min, bandwidth, padding, param_1, param_2, threshold, seed);
        parseABCtoDataset(&process, max_threads);

        LOG_FUNC("EXIT");
        return 0;

    }
    int MeshToSdfSegments() {

        std::string f_name = R"(C:\Local_Data\Segmentation_Alex\samples\hx_gyroid_2.obj)";

        std::string target_dir = R"(C:\Local_Data\Segmentation_Alex\hx_gyroid_2)";

        Surface_mesh mesh;

        std::ifstream input(f_name);

        if (!input || !MDH::readMesh(&f_name, &mesh)) {
            std::cerr << "Failed to read mesh file!" << std::endl;
            return 1;
        }

        int k_size = 16;
        int n_min_k = 2;
        int padding = 4;
        int bandwidth = 5;

        auto rec_voxelsize = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, n_min_k, bandwidth, padding, mesh);

        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, rec_voxelsize, (float)bandwidth, std::numeric_limits<float>::max());

        int n_face = my_faces.size();

        int n_vert = my_verts.size();

        //create cropping Origins
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, k_size, padding);

        auto orgin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
        std::string origin_bin = target_dir + "/origins" + ".bin";
        Tools::util::saveFloatMatrix(orgin_list, origin_bin);

        //Create a Face to Grid centered index map and save it as binary
        auto face_centers = Tools::util::CalculateFaceCenters(my_faces, my_verts);

        auto FaceToGridIndex = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, face_centers);
        std::string FaceToGridIndex_bin = target_dir + "/FaceToGridIndex.bin";
        Tools::util::saveFloatMatrix(FaceToGridIndex, FaceToGridIndex_bin);


        Tools::Float3DArray clipped_array;

        double background = grid->tree().background();

        double voxel_size = (double)rec_voxelsize;

        float minVal = Tools::OpenVDBbased::getGridMinActiceValue(grid);

        Tools::LinearSDFMap lmap;

        std::cout << "minVal: " << minVal << " background: " << background << std::endl;

        //map 0-1 for sdf with holes 
        //map -1 - 1 for water tight sdfs
        lmap.create(minVal, background, -1, 1);

        for (size_t i = 0; i < crop_list.size(); ++i) {
            clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], k_size);
            Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap);
            std::string f_name = target_dir + "/segment_" + std::to_string(i) + ".bin";
            Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, background);
        }

        return 0;
    }
    int stripLinesFormOBJ() {
        std::vector<fs::path> found_files = Scripts::GetFilesOfType(R"(C:\Local_Data\ABC\ABC_parsed_files)", ".obj");
        std::vector<std::string> filter = { "vc" };

        //const unsigned int max_threads = std::thread::hardware_concurrency(); // or set manually, e.g., 4
        const unsigned int max_threads = 10;
        std::vector<std::thread> threads;

        size_t index = 0;

        while (index < found_files.size()) {
            while (threads.size() < max_threads && index < found_files.size()) {
                threads.emplace_back([file = found_files[index], &filter]() {
                    Tools::util::filterObjFile(file.generic_string(), filter);
                    std::cout << "Removed line containing " << filter[0] << " from " << file.filename() << std::endl;
                    });
                ++index;
            }

            // Join all running threads before starting new ones
            for (auto& t : threads) {
                if (t.joinable()) t.join();
            }
            threads.clear();
        }

        return 0;
    }
    int ABCgetFaceTypeMaps() {

        fs::path Source = R"(C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark)";
        fs::path Target = R"(C:\Local_Data\ABC\ABC_statistics\face_types_analysis\ABC_chunk_benchmark)";

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, 2);
        openvdb::initialize();

        int max_threads = 2;
         
        for (int i = 1; i < 11; i++) {

            LOG_FUNC("ENTER_" + std::to_string(max_threads));

            ProcessingUtility::ProcessForFaceTypeStats process_1(Source, Target);
            parseABCtoDataset(&process_1, max_threads);

            LOG_FUNC("EXIT_" + std::to_string(max_threads));
        }
  
        

        return 0;

    }
    //Testing Scripts
    int TestNoisedSDF() {

        std::string source_root = R"(C:\Local_Data\ABC\ABC_parsed_files)";

        std::string target_root = R"(C:\Local_Data\ABC\ABC_noised)";

        fs::create_directories(target_root);


        int ks = 16;
        int n_min_k = 2;
        int band_width = 5;
        int padding = 0;

        for (const auto& entry : fs::recursive_directory_iterator(source_root)) {
            if (fs::is_regular_file(entry)) {
                fs::path file_path = entry.path();
                fs::path parent_dir = file_path.parent_path().filename(); // e.g., 00000001
                std::string extension = file_path.extension().string();   // e.g., .obj, .yml

                fs::path new_dir = target_root / parent_dir;

                if (extension == ".obj") {

                    fs::create_directories(new_dir);

                    fs::path new_file_name = parent_dir.string() + "_noised" + extension; // e.g., 00000001.obj
                    fs::path dest_path = new_dir / new_file_name;

                    Surface_mesh mesh;

                    try {
                        std::string f_name = file_path.generic_string();

                        std::string out_name = dest_path.generic_string();

                        std::ifstream input(f_name);

                        if (!input || !MDH::readMesh(&f_name, &mesh)) {
                            std::cerr << "Failed to read mesh file!" << std::endl;
                            return 1;
                        }

                        int removed = NoiseOnMesh::CGALbased::applySwirlyNoise(&mesh, 2,2, 0.25, 42);

                        std::cout << "Removed " << removed << " Faces" << std::endl;

                        MDH::writeMesh(&out_name, &mesh);

                        double voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(ks, n_min_k, band_width, padding, mesh);

                        //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
                        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);
                        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)band_width, std::numeric_limits<float>::max());

                        auto min_val = Tools::OpenVDBbased::getGridMinActiceValue(grid);

                        auto background = grid->tree().background();

                        std::cout << "Min Val = " << min_val << std::endl;
                        std::cout << "Background = " << background << std::endl;

                    }
                    catch (const std::exception& e) {
                        std::cerr << "Failed to copy: " << file_path << " (" << e.what() << ")\n";
                    }

                }

                
            }
        }

    }
    int ApplySwirlOnMesh() {



        std::string f_name = R"(C:\Local_Data\ABC\ABC_parsed_files\00000002\00000002.obj)";

        std::string target_dir = "C:/Local_Data/cropping_test";

        Surface_mesh mesh;

        std::ifstream input(f_name);

        if (!input || !MDH::readMesh(&f_name, &mesh)) {
            std::cerr << "Failed to read mesh file!" << std::endl;
            return 1;
        }

        std::cout << "Applying noise pattern" << std::endl;

        int removed = NoiseOnMesh::CGALbased::applySwirlyNoise(&mesh, 10 , 10, 0.2, 42);

        std::cout << "Removed " << removed << " Faces" << std::endl;

        std::string out_name = target_dir + "/swirly_mesh.stl";

        MDH::writeMesh(&out_name, &mesh);

        //dertimine Reccomende voxel size 

        int ks = 16;
        int n_min_k = 2;
        int band_width = 5;
        int padding = 0;

        double voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(ks, n_min_k, band_width, padding, mesh);

        //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);
        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)band_width, std::numeric_limits<float>::max());

        auto min_val = Tools::OpenVDBbased::getGridMinActiceValue(grid);

        std::cout << "Min Val = " << min_val << std::endl;

    }
    int CopyAndRenameYMLandOBJ()
    {
        fs::path source_path = R"(C:\Local_Data\ABC\obj\abc_meta_files\abc_0000_obj_v00)";
        fs::path target_path = R"(C:\Local_Data\ABC\ABC_parsed_files)";

        CopyAndRenameToParsedStructure(source_path, target_path);

        return 0;

    }
    int StatisticsOnABC() {

        int thread_count = 16;

        std::string traget_dir = R"(C:\Local_Data\ABC\ABC_parsed_files)";
        std::string log_file = traget_dir + "/ABC_log.csv";
        int kernel_size = 32;
        int n_min_k = 1;
        int padding = 4;
        int bandwidth = 5;

        auto counter = Scripts::AnalyzeABCDir(traget_dir, ".stl", kernel_size, n_min_k, bandwidth, padding, 16);

        std::cout << "Filed sucessfully read " << counter;

        return 0;
    }
    int SdfToSegmentOnABC() {

        std::string yml_name = "C:/Local_Data/ABC/feat/abc_meta_files/abc_0000_feat_v00/00000002/00000002_1ffb81a71e5b402e966b9341_features_001.yml";
        std::string obj_name = "C:/Local_Data/ABC/obj/abc_meta_files/abc_0000_obj_v00/00000002/00000002_1ffb81a71e5b402e966b9341_trimesh_001.obj";
        std::string obj_out = "C:/Local_Data/ABC/trimesh_001.obj";

        Surface_mesh mesh;

        std::ifstream in(obj_name);
        std::ofstream out(obj_out);

        std::string line;
        while (std::getline(in, line)) {
            if (line.size() < 2 || line.substr(0, 2) != "vc") {
                out << line << '\n';
            }
        }

        std::ifstream input(obj_out);

        if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
            std::cerr << "Failed to read .obj file!" << std::endl;
            return 1;
        }

        int k_size = 32;
        int n_min_k = 2;
        int padding = 4;
        int bandwidth = 5;

        std::string target_dir = "C:/Local_Data/cropping_test";

        auto rec_voxelsize = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, n_min_k, bandwidth, padding, mesh);

        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, rec_voxelsize, (float)bandwidth, std::numeric_limits<float>::max());

        int n_face = my_faces.size();

        int n_vert = my_verts.size();

        auto vert_list = Tools::util::GetVertexToSurfTypeMapYAML(yml_name, n_vert);

        auto face_list = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_face);

        auto arr = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, my_verts);

        std::string vert_bin = target_dir + "/VertTypeMap" + ".bin";

        Tools::util::saveTypeMapToBinary(vert_list, vert_bin);

        std::string face_bin = target_dir + "/FaceTypeMap" + ".bin";

        Tools::util::saveTypeMapToBinary(face_list, face_bin);

        std::string arr_bin = target_dir + "/VertToGridIndex" + ".bin";

        Tools::util::saveFloatMatrix(arr, arr_bin);

        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, k_size, padding);

        auto orgin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);

        std::string origin_bin = target_dir + "/origins" + ".bin";

        Tools::util::saveFloatMatrix(orgin_list, origin_bin);


        Tools::Float3DArray clipped_array;

        double bckgrd = grid->tree().background();

        double voxel_size = (double)rec_voxelsize;

        for (size_t i = 0; i < crop_list.size(); ++i) {
            clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i] , k_size);
            std::string f_name = target_dir + "/cropped_" + std::to_string(i) + ".bin";
            Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, bckgrd);
        }

        return 0;
    }
    int ParseModelNet() {

        namespace fs = std::filesystem;

        fs::path srcDir = "C:/Local_Data/DL_Datasets/ModelNet10_RAW/ModelNet10"; // Change this to your source directory
        fs::path destDir = "C:/Local_Data/DL_Datasets/ModelNet10_SDF_32"; // Change this to your output directory
        std::string newExtension = ".bin"; // Change to the desired new extension

        ParseDataset(srcDir, destDir, newExtension);

        srcDir = "C:/Local_Data/DL_Datasets/ModelNet40_RAW/ModelNet40"; // Change this to your source directory
        destDir = "C:/Local_Data/DL_Datasets/ModelNet40_SDF_32"; // Change this to your output directory

        ParseDataset(srcDir, destDir, newExtension);

        
        return 0;
    }
    int Output4Python() {
        //TO DO Add binary rep
        std::string filename = "C:/Local_Data/DL_Datasets/ModelNet10_RAW/ModelNet10/bed/train/bed_0040.off";

        std::string out_dir = "C:/Local_Data/ReadWriteTest";

        Surface_mesh mesh;

        if (!MDH::readMesh(&filename, &mesh)) {
            std::cerr << "Cannot open file " << filename << std::endl;
            return EXIT_FAILURE;
        }

        auto [meshVertices, meshFaces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

        int voxel_count = 32;

        // Define narrow-band widths
        float exBandWidth = 5.0f;
        float inBandWidth = 5.0f;

        auto sdfGrid = Tools::OpenVDBbased::MeshToFloatGrid(meshVertices, meshFaces, voxel_count, exBandWidth, inBandWidth);

        int test = Tools::OpenVDBbased::ActivateInsideValues(sdfGrid);

        double voxelSize = sdfGrid->transform().voxelSize()[0];

        double background = sdfGrid->tree().background();

        bool grid_valid = Tools::OpenVDBbased::CheckIfGridHasValidInsideVoxel(sdfGrid);

        Tools::Float3DArray floatArray = Tools::OpenVDBbased::Float3DArrayFromFloatGrid(sdfGrid, voxel_count);

        Tools::LinearSDFMap map;

        float min = Tools::OpenVDBbased::getGridMinActiceValue(sdfGrid);

        map.create(min,background,-1, 1);

        Tools::OpenVDBbased::RemapFloat3DArray(floatArray, map);

        //auto NewGrid = Tools::OpenVDBbased::FloatGridFromFloat3DArray(floatArray);

        std::string fname = "DebugfloatArray.bin";

        Tools::util::saveFloat3DGridPythonic(out_dir, fname, floatArray, voxelSize, background);

        /*
        NewGrid->setTransform(openvdb::math::Transform::createLinearTransform(voxelSize));

        // Write the resulting grid to a file.
        openvdb::io::File file("C:/Local_Data/ReadWriteTest/out_" + std::to_string(voxel_count) + ".vdb");
        openvdb::GridPtrVec grids;
        grids.push_back(NewGrid);
        file.write(grids);
        file.close();
        */
        return 0;
    }
    int WaveFunctionTest() {

        openvdb::initialize();

        //std::string filename_0 = "C:/Local_Data/SOLIDWORKS/cut_cube.STL";
        //std::string filename_0 = "C:/Local_Data/CGAL-6.0.1_examples/data/meshes/cube.off";
        std::string filename = "C:/Local_Data/CGAL-6.0.1_examples/data/meshes/sphere.off";

        std::string out_dir = "C:/Local_Data/WaveFuncTest/";

        int dim = 50;

        Surface_mesh mesh;

        if (!MDH::readMesh(&filename, &mesh)) {
            std::cerr << "Cannot open file " << filename << std::endl;
            return EXIT_FAILURE;
        }

        //mesh to vdb

        auto [meshVertices, meshFaces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

        openvdb::FloatGrid::Ptr sdfGrid = Tools::OpenVDBbased::MeshToFloatGrid(meshVertices, meshFaces, 0.1f, 5.0f, 5.0f);

        //parameters

        std::vector<float> list_freq = { 0.5f, 1.f, 3.f };

        std::vector<float> list_amp = { 0.5f, 1.f, 2.f };

        std::vector<openvdb::FloatGrid::Ptr> output_grids;

        std::vector<std::string> output_names;

        output_names.push_back("default.vdb");

        output_grids.push_back(sdfGrid);

        for (auto f : list_freq) {
            for (auto a : list_amp)
            {
                openvdb::FloatGrid::Ptr copy = sdfGrid->deepCopy();

                Tools::OpenVDBbased::applyWaveDeformation(copy, a, f, Tools::SINE_WAVE);

                //openvdb::FloatGrid::Ptr copyB = openvdb::tools::levelSetRebuild(*copy);

                std::string name = "grid_amp=" + std::to_string(a) + "_freq=" + std::to_string(f) + ".vdb";

                output_names.push_back(name);

                std::cout << "Added Wave Deformation amp= " + std::to_string(a) + " freq= " + std::to_string(f) << std::endl;

                output_grids.push_back(copy);

            }
        }

        for (auto it = output_grids.begin(); it != output_grids.end(); ++it)
        {
            int index = std::distance(output_grids.begin(), it);

            std::string save_path = out_dir + output_names[index];

            // Write the resulting grid to a file.
            openvdb::io::File file(save_path);
            openvdb::GridPtrVec grids;
            grids.push_back(*it);
            file.write(grids);
            file.close();
        }

        return 0;
    }
}



