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

namespace Scripts {

    int ABCtoDataset(){


        LOG_FUNC("ENTER");

        fs::path Source = R"(C:\Local_Data\ABC\ABC_parsed_files)";
        fs::path Target = R"(C:\Local_Data\ABC\ABC_Data_ks_16_pad_4_bw_5_vs_adaptive_n2)";

        int kernel_size = 16;
        int padding = 4;
        int bandwidth = 5;
        double voxel_size = 0.5;
        int n_k_min = 2;
        int max_threads = 8;

        // Limit TBB thread count to max_threads
        tbb::global_control control(tbb::global_control::max_allowed_parallelism, max_threads);
        openvdb::initialize();

        parseABCtoDataset(Source, Target, kernel_size, n_k_min, bandwidth, padding, max_threads);

        LOG_FUNC("EXIT");
        return 0;
    }

    int stripLinesFormOBJ(){
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


    int CopyAndRenameYMLandOBJ()
    {
        fs::path source_path = R"(C:\Local_Data\ABC\feat\abc_meta_files\abc_0000_feat_v00)";
        fs::path target_path = R"(C:\Local_Data\ABC\ABC_parsed_files)";

        CopyAndRenameToParsedStructure(source_path, target_path);

        return 0;

    }
    int StatisticsOnABC() {

        int thread_count = 16;

        std::string traget_dir = R"(C:\Local_Data\ABC\ABC_parsed_files)";
        std::string log_file = traget_dir + "/ABC_log.csv";
        int kernel_size = 32;
        int n_min_k = 2;
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
    int TestFixedGridSize() {
#pragma region IO


        std::string filename = "C:/Local_Data/CGAL-6.0.1_examples/data/meshes/bunny00.off";
        //std::string filename = "C:\\Local_Data\\SOLIDWORKS\\cut_cube.ply"
        Surface_mesh mesh;

        if (!MDH::readMesh(&filename, &mesh)) {
            std::cerr << "Cannot open file " << filename << std::endl;
            return EXIT_FAILURE;
        }

        auto [meshVertices, meshFaces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

        // Define narrow-band widths
        float exBandWidth = 5.0f;
        float inBandWidth = 5.0f;

        //define Voxel Dim
#pragma endregion

        std::vector<int> test_val;

        for (int i = 20; i <= 200; i++) {
            test_val.push_back(i);
        }

        for (auto targetMaxDim : test_val) {

            auto sdfGrid = Tools::OpenVDBbased::MeshToFloatGrid(meshVertices, meshFaces, targetMaxDim, exBandWidth, inBandWidth);

#pragma region Debug

            // Get the bounding box in **index space** (voxel coordinates)
            openvdb::CoordBBox bbox;
            //sdfGrid->tree().evalLeafBoundingBox(bbox);
            sdfGrid->tree().evalLeafBoundingBox(bbox);

            // Get dimensions in voxels
            int dimX = bbox.dim().x();
            int dimY = bbox.dim().y();
            int dimZ = bbox.dim().z();

            // Print voxel count in each dimension
            //std::cout << "World count: X = " << dimX << ", Y = " << dimY << ", Z = " << dimZ << std::endl;

            //sdfGrid->tree().evalLeafBoundingBox(bbox);
            sdfGrid->tree().evalActiveVoxelBoundingBox(bbox);

            // Get dimensions in voxels
            dimX = bbox.dim().x();
            dimY = bbox.dim().y();
            dimZ = bbox.dim().z();

            // Print voxel count in each dimension
            std::cout << "Traget Dim:" << targetMaxDim << " Active count: X = " << dimX << ", Y = " << dimY << ", Z = " << dimZ;
            if (targetMaxDim >= dimX)
            {
                std::cout << "->PASSED" << std::endl;
            }
            else
            {
                std::cout << "->FAILED" << std::endl;
            }


            //openvdb::CoordBBox bboxWorld, bboxActive;
            //sdfGrid->tree().evalLeafBoundingBox(bboxWorld);
            //sdfGrid->tree().evalActiveVoxelBoundingBox(bboxActive);

            //std::cout << "World Voxel BBox: " << bboxWorld << std::endl;
            //std::cout << "Active Voxel BBox: " << bboxActive << std::endl;
#pragma endregion

        }
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
    int FixedGridSizeTest() {
        std::string filename = "C:/Local_Data/CGAL-6.0.1_examples/data/meshes/bunny00.off";
        //std::string filename = "C:\\Local_Data\\SOLIDWORKS\\cut_cube.ply"

        int voxelgrid_dim = 202;

        float ex_band = 10.f;
        float in_band = 3.f;

        Surface_mesh mesh;

        if (!MDH::readMesh(&filename, &mesh)) {
            std::cerr << "Cannot open file " << filename << std::endl;
            return EXIT_FAILURE;
        }

        auto [meshVertices, meshFaces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

        

        // Extract dimensions
        CGAL::Bbox_3 CGAL_bbox = CGAL::Polygon_mesh_processing::bbox(mesh);

        double width = CGAL_bbox.xmax() - CGAL_bbox.xmin();
        double height = CGAL_bbox.ymax() - CGAL_bbox.ymin();
        double depth = CGAL_bbox.zmax() - CGAL_bbox.zmin();

        std::cout << "Mesh World:" << std::endl;
        std::cout << "X: " << width << std::endl;
        std::cout << "Y: " << height << std::endl;
        std::cout << "Z: " << depth << std::endl;

        float vox_res = std::max({ width, height, depth }) / (voxelgrid_dim);

        openvdb::FloatGrid::Ptr sdfGrid = Tools::OpenVDBbased::MeshToFloatGrid(
            meshVertices,
            meshFaces,
            vox_res,
            ex_band,
            in_band
        );

        // openvdb::FloatGrid::Ptr evenGrid = Tools::OpenVDBbased::resizeToEvenGrid(sdfGrid, 50, 50, 50);

        openvdb::CoordBBox fullBBox;
        sdfGrid->tree().evalLeafBoundingBox(fullBBox);

        openvdb::CoordBBox bbox = sdfGrid->evalActiveVoxelBoundingBox();

        std::cout << "LeafBox: " << fullBBox.dim().x() << ", " << fullBBox.dim().y() << ", " << fullBBox.dim().z() << std::endl;

        std::cout << "ActiveBox: " << bbox.dim().x() << ", " << bbox.dim().y() << ", " << bbox.dim().z() << std::endl;

        Tools::Float3DArray FloatArry = Tools::OpenVDBbased::Float3DArrayFromFloatGrid(sdfGrid);

        float test = sdfGrid->background();

        openvdb::Vec3d minWorld = sdfGrid->indexToWorld(openvdb::Vec3d(fullBBox.min().asVec3d()));
        openvdb::Vec3d maxWorld = sdfGrid->indexToWorld(openvdb::Vec3d(fullBBox.max().asVec3d()));

        // Compute the width (x-dimension)
        std::cout << "Grid World:" << std::endl;
        std::cout << "X: " << (maxWorld.x() - minWorld.x()) << std::endl;
        std::cout << "Y: " << (maxWorld.y() - minWorld.y()) << std::endl;
        std::cout << "Z: " << (maxWorld.z() - minWorld.z()) << std::endl;

        return 0;
    }
}



