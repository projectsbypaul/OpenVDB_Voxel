#include "ABCProcessing.h"
#include "NoiseOnMesh.h"

#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>



namespace fs = std::filesystem;


namespace Scripts {

    bool checkIfDirWasProcessed(const std::string& targetDir) {
        if (fs::exists(targetDir) && fs::is_directory(targetDir)) {
             return true;
        }
        else {
             return false;
        }

    }

    void CopyAndRenameToParsedStructure(const fs::path& source_root, const fs::path& target_root) {
        // Create the target root if it doesn't exist
        fs::create_directories(target_root);

        for (const auto& entry : fs::recursive_directory_iterator(source_root)) {
            if (fs::is_regular_file(entry)) {
                fs::path file_path = entry.path();
                fs::path parent_dir = file_path.parent_path().filename(); // e.g., 00000001
                std::string extension = file_path.extension().string();   // e.g., .obj, .yml

                fs::path new_dir = target_root / parent_dir;


                fs::create_directories(new_dir);

                fs::path new_file_name = parent_dir.string() + extension; // e.g., 00000001.obj
                fs::path dest_path = new_dir / new_file_name;

                try {
                    fs::copy_file(file_path, dest_path, fs::copy_options::overwrite_existing);
                    std::cout << "Copied: " << file_path << " -> " << dest_path << '\n';
                }
                catch (const std::exception& e) {
                    std::cerr << "Failed to copy: " << file_path << " (" << e.what() << ")\n";
                }
            }
        }
    }

    // Stub for your actual processing function
    void processingForDLLDataset(const fs::path& sourceDir, const fs::path& outputDir,const std::string& subdirName, int k_size, double voxel_size, int band_width, int padding) {
        std::cout << "Processing: " << subdirName << " -> Output: " << outputDir << std::endl;
     
        //Define source file and traget file location
        std::string yml_name = (sourceDir / subdirName).generic_string() + ".yml";
        std::string obj_name = (sourceDir / subdirName).generic_string() + ".obj";
        std::string obj_out = (outputDir / subdirName).generic_string() + ".obj";
        std::string target_dir = (outputDir / subdirName).generic_string();

        //load obj into cgal surface mesh
        //remember clean obj from "vc" lines 
        std::ifstream input(obj_name);
        Surface_mesh mesh;

        if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
            std::cerr << "Failed to read .obj file!" << std::endl;
           
            return;
        }

        //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);
        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)band_width, std::numeric_limits<float>::max());

       
        //Create face to type map and save it a binary
        int n_face = my_faces.size();
        auto face_list = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_face);
        std::string face_bin = outputDir.generic_string() + "/FaceTypeMap" + ".bin";
        Tools::util::saveTypeMapToBinary(face_list, face_bin);

        //Create vertex to type map and save it a binary
        int n_vert = my_verts.size();
        auto vert_list = Tools::util::GetVertexToSurfTypeMapYAML(yml_name, n_vert);
        std::string vert_bin = outputDir.generic_string() + "/VertTypeMap" + ".bin";
        Tools::util::saveTypeMapToBinary(vert_list, vert_bin);


        //Create a Vertex to Grid centered index map and save it as binary
        /*
        auto arr = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, my_verts);
        std::string arr_bin = target_dir + "/VertToGridIndex" + ".bin";
        Tools::util::saveFloatMatrix(arr, arr_bin);
        */
        //Create a Face to Grid centered index map and save it as binary
        auto face_centers = Tools::util::CalculateFaceCenters(my_faces, my_verts);

        auto FaceToGridIndex = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, face_centers);
        std::string FaceToGridIndex_bin = outputDir.generic_string() + "/FaceToGridIndex.bin";
        Tools::util::saveFloatMatrix(FaceToGridIndex, FaceToGridIndex_bin);

        //Count Faces per SurfaceType and save counts binary
        auto counts = Tools::util::CountFacesPerSurfaceType(face_list);
        auto count_bin = outputDir.generic_string() + "/TypeCounts.bin";
        Tools::util::saveTypeCountsToBinary(counts, count_bin);
        
        //based on the cropping parameter -> calculate a origin for each cropping segemnent 
        //save origin as binary for reconstruction of labled data a remapping of segmentation resulst 
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, k_size, padding);
        auto orgin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
        std::string origin_bin = outputDir.generic_string() + "/origins" + ".bin";
        Tools::util::saveFloatMatrix(orgin_list, origin_bin);

        //Set up linear map for normalization
        Tools::LinearSDFMap lmap;

        double background = grid->tree().background();
        float minVal = Tools::OpenVDBbased::getGridMinActiceValue(grid);

        lmap.create(minVal, background, -1, 1);

        //Create a an array that holds cropping results
        Tools::Float3DArray clipped_array;

        //crop sdf grid and write cropping result into 3D float array
        //save cropped segments into binary file
        for (size_t i = 0; i < crop_list.size(); ++i) {
            clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], k_size);
            Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap); //normalize clipped array 
            std::string f_name = outputDir.generic_string() + "_" + std::to_string(i) + ".bin";
            Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, background);
        }

    }

    // Stub for your actual processing function
    void processingForDLLDataset(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, int min_n_kernel, int band_width, int padding) {


        LOG_FUNC("ENTER" << " subdirName = " << subdirName << ", outputDir = " << outputDir);

        std::cout << "Processing: " << subdirName << " -> Output: " << outputDir << std::endl;
       
        //Define source file and traget file location
        std::string yml_name = (sourceDir / subdirName).generic_string() + ".yml";
        std::string obj_name = (sourceDir / subdirName).generic_string() + ".obj";
        std::string obj_out = (outputDir / subdirName).generic_string() + ".obj";
        std::string target_dir = (outputDir / subdirName).generic_string();

        //load obj into cgal surface mesh
        //remember clean obj from "vc" lines 
        std::ifstream input(obj_name);
        Surface_mesh mesh;

        if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
            std::cerr << "Failed to read .obj file!" << std::endl;
            LOG_FUNC("EXIT" << " subdirName = " << subdirName << "outputDir = " << outputDir << " Failed to read .obj file!" );
            return;
        }

        //dertimine Reccomende voxel size 
        double voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, min_n_kernel, band_width, padding, mesh);

        //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);
        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)band_width, std::numeric_limits<float>::max());


        //based on the cropping parameter -> calculate a origin for each cropping segemnent 
        //save origin as binary for reconstruction of labled data a remapping of segmentation resulst 
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, k_size, padding);

        if (crop_list.size() < 300) {
            auto orgin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
            std::string origin_bin = outputDir.generic_string() + "/origins" + ".bin";
            Tools::util::saveFloatMatrix(orgin_list, origin_bin);

            //Create face to type map and save it a binary
            int n_face = my_faces.size();
            auto face_list = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_face);
            std::string face_bin = outputDir.generic_string() + "/FaceTypeMap" + ".bin";
            Tools::util::saveTypeMapToBinary(face_list, face_bin);

            //Create vertex to type map and save it a binary
            int n_vert = my_verts.size();
            auto vert_list = Tools::util::GetVertexToSurfTypeMapYAML(yml_name, n_vert);
            std::string vert_bin = outputDir.generic_string() + "/VertTypeMap" + ".bin";
            Tools::util::saveTypeMapToBinary(vert_list, vert_bin);

            //Create a Vertex to Grid centered index map and save it as binary
            /*
            auto arr = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, my_verts);
            std::string arr_bin = target_dir + "/VertToGridIndex" + ".bin";
            Tools::util::saveFloatMatrix(arr, arr_bin);
            */

            //Create a Face to Grid centered index map and save it as binary
            auto face_centers = Tools::util::CalculateFaceCenters(my_faces, my_verts);
            auto FaceToGridIndex = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, face_centers);
            std::string FaceToGridIndex_bin = outputDir.generic_string() + "/FaceToGridIndex.bin";
            Tools::util::saveFloatMatrix(FaceToGridIndex, FaceToGridIndex_bin);

            //Count Faces per SurfaceType and save counts binary
            auto counts = Tools::util::CountFacesPerSurfaceType(face_list);
            auto count_bin = outputDir.generic_string() + "/TypeCounts.bin";
            Tools::util::saveTypeCountsToBinary(counts, count_bin);

            //Set up linear map for normalization
            Tools::LinearSDFMap lmap;

            double background = grid->tree().background();
            float minVal = Tools::OpenVDBbased::getGridMinActiceValue(grid);

            lmap.create(minVal, background, -1, 1);

            //Create a an array that holds cropping results
            Tools::Float3DArray clipped_array;

            //crop sdf grid and write cropping result into 3D float array
            //save cropped segments into binary file
            for (size_t i = 0; i < crop_list.size(); ++i) {
                clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], k_size);
                Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap); //normalize clipped array 
                std::string f_name = target_dir + "_" + std::to_string(i) + ".bin";
                Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, background);
            }

        }
        else
        {
            std::cout << subdirName << +".bin " << "is odd sized --> skipped";
            LOG_FUNC("EXIT" << " subdirName = " << subdirName << "outputDir = " << outputDir << "is odd sized --> skipped");
           
        }
        LOG_FUNC("EXIT" << " subdirName = " << subdirName << "outputDir = " << outputDir);




    }

    // Stub for your actual processing function
    void processingForDLLDatasetAE(const fs::path& sourceDir, const fs::path& outputDir, const std::string& subdirName, int k_size, int min_n_kernel, int band_width, int padding, double param_1, double param_2, double threshold, int random_seed) {


        LOG_FUNC("ENTER" << " subdirName = " << subdirName << ", outputDir = " << outputDir);

        std::cout << "Processing: " << subdirName << " -> Output: " << outputDir << std::endl;



        //Define source file and traget file location
        std::string yml_name = (sourceDir / subdirName).generic_string() + ".yml";
        std::string obj_name = (sourceDir / subdirName).generic_string() + ".obj";
        std::string obj_out = (outputDir / subdirName).generic_string() + ".obj";
        std::string target_dir = (outputDir / subdirName).generic_string();

        //load obj into cgal surface mesh
        //remember clean obj from "vc" lines 
        std::ifstream input(obj_name);
        Surface_mesh mesh;

        if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
            std::cerr << "Failed to read .obj file!" << std::endl;
            LOG_FUNC("EXIT" << " subdirName = " << subdirName << "outputDir = " << outputDir << " Failed to read .obj file!");
            return;
        }

        //Add noise to mesh 
        int removed = NoiseOnMesh::CGALbased::applySwirlyNoise(&mesh, param_1, param_2, threshold, random_seed);

        std::cout << "Removed " << removed << " Faces from" << outputDir << std::endl;


        //dertimine Reccomende voxel size 
        double voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, min_n_kernel, band_width, padding, mesh);

        //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);
        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)band_width, std::numeric_limits<float>::max());


        //based on the cropping parameter -> calculate a origin for each cropping segemnent 
        //save origin as binary for reconstruction of labled data a remapping of segmentation resulst 
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, k_size, padding);

        if (crop_list.size() < 300) {
            auto orgin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
            std::string origin_bin = outputDir.generic_string() + "/origins" + ".bin";
            Tools::util::saveFloatMatrix(orgin_list, origin_bin);

            //Create face to type map and save it a binary
            /*
            int n_face = my_faces.size();
            auto face_list = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_face);
            std::string face_bin = target_dir + "/FaceTypeMap" + ".bin";
            Tools::util::saveTypeMapToBinary(face_list, face_bin);
            */

            //Create vertex to type map and save it a binary
            int n_vert = my_verts.size();
            auto vert_list = Tools::util::GetVertexToSurfTypeMapYAML(yml_name, n_vert);
            std::string vert_bin = outputDir.generic_string() + "/VertTypeMap" + ".bin";
            Tools::util::saveTypeMapToBinary(vert_list, vert_bin);


            //Create a Vertex to Grid centered index map and save it as binary
            auto arr = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, my_verts);
            std::string arr_bin = outputDir.generic_string() + "/VertToGridIndex" + ".bin";
            Tools::util::saveFloatMatrix(arr, arr_bin);

            //Set up linear map for normalization

            Tools::LinearSDFMap lmap;

            double background = grid->tree().background();
            float minVal = Tools::OpenVDBbased::getGridMinActiceValue(grid);

            lmap.create(minVal, background, -1, 1);

            //Create a an array that holds cropping results
            Tools::Float3DArray clipped_array;

            //crop sdf grid and write cropping result into 3D float array
            //save cropped segments into binary file
            for (size_t i = 0; i < crop_list.size(); ++i) {
                clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], k_size);
                Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap); //normalize clipped array 
                std::string f_name = target_dir + "_" + std::to_string(i) + ".bin";
                Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, background);
            }

        }
        else
        {
            std::cout << subdirName << +".bin " << "is odd sized --> skipped";
            LOG_FUNC("EXIT" << " subdirName = " << subdirName << "outputDir = " << outputDir << "is odd sized --> skipped");

        }
        LOG_FUNC("EXIT" << " subdirName = " << subdirName << "outputDir = " << outputDir);

    }


    void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir,int& k_size, double& voxel_size, int& band_width, int& padding) {

        // Create the target root if it doesn't exist
        fs::create_directories(Output_Dir);

        // Iterate through subdirectories of the main folder
        for (const auto& entry : fs::directory_iterator(Source_Dir)) {
            if (!fs::is_directory(entry.status())) continue;

            fs::path subdirPath = entry.path();
            std::string subdirName = subdirPath.filename().string();

            bool hasObj = false;
            bool hasYml = false;

            // Check for .obj and .yml files inside subdir
            for (const auto& file : fs::directory_iterator(subdirPath)) {
                if (!fs::is_regular_file(file.status())) continue;

                std::string ext = file.path().extension().string();
                if (ext == ".obj") hasObj = true;
                if (ext == ".yml") hasYml = true;
            }

            // If both .obj and .yml are found, process
            if (hasObj && hasYml) {
                fs::path newOutputDir = Output_Dir / subdirName;
                fs::create_directories(newOutputDir);  // creates if not exists

                processingForDLLDataset(subdirPath, newOutputDir, subdirName, k_size, voxel_size, band_width, padding);
            }
        }

    }

    void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding) {

        LOG_FUNC("ENTER");

        // Create the target root if it doesn't exist
        fs::create_directories(Output_Dir);

        // Iterate through subdirectories of the main folder
        for (const auto& entry : fs::directory_iterator(Source_Dir)) {
            if (!fs::is_directory(entry.status())) continue;

            fs::path subdirPath = entry.path();
            std::string subdirName = subdirPath.filename().string();

            bool hasObj = false;
            bool hasYml = false;

            // Check for .obj and .yml files inside subdir
            for (const auto& file : fs::directory_iterator(subdirPath)) {
                if (!fs::is_regular_file(file.status())) continue;

                std::string ext = file.path().extension().string();
                if (ext == ".obj") hasObj = true;
                if (ext == ".yml") hasYml = true;
            }

            // If both .obj and .yml are found, process
            if (hasObj && hasYml) {
                fs::path newOutputDir = Output_Dir / subdirName;
                
                if (!checkIfDirWasProcessed(newOutputDir.generic_string())) {
                    fs::create_directories(newOutputDir);  // creates if not exists
                    processingForDLLDataset(subdirPath, newOutputDir, subdirName, k_size, min_n_kernel, band_width, padding);
                }
                else {
                    std::cout << "Directory " << newOutputDir.filename() << " alreay processed -> skipped" << std::endl;
                   
                }
                
            }

            LOG_FUNC("EXIT");
        }

    }

    void parseABCtoDataset(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding, int max_threads = 0) {

        LOG_FUNC("ENTER" << " Source_Dir = " << Source_Dir.filename() << ", Output_Dir = " << Output_Dir.filename() << ", Threads = " << max_threads);

        fs::create_directories(Output_Dir);

        std::vector<std::thread> threads;
        std::mutex mtx;
        std::condition_variable cv;
        int active_threads = 0;

        if (max_threads == 0) {
            max_threads = std::thread::hardware_concurrency();
        }

        std::cout << "Running on thread count: " << max_threads << std::endl;
       

        for (const auto& entry : fs::directory_iterator(Source_Dir)) {
            if (!fs::is_directory(entry.status())) continue;

            fs::path subdirPath = entry.path();
            std::string subdirName = subdirPath.filename().string();

            bool hasObj = false;
            bool hasYml = false;

            for (const auto& file : fs::directory_iterator(subdirPath)) {
                if (!fs::is_regular_file(file.status())) continue;

                std::string ext = file.path().extension().string();
                if (ext == ".obj") hasObj = true;
                if (ext == ".yml") hasYml = true;
            }

            if (hasObj && hasYml) {
                fs::path newOutputDir = Output_Dir / subdirName;

                if (checkIfDirWasProcessed(newOutputDir.generic_string())) {
                    std::cout << "Directory " << newOutputDir.filename() << " already processed -> skipped" << std::endl;
                    LOG_FUNC("EXIT " << " Directory " << newOutputDir.filename() << " already processed->skipped");
                    continue;
                }

                // Wait until there is a free thread slot
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]() { return active_threads < max_threads; });
                ++active_threads;
                lock.unlock();

                // Copy values to avoid reference issues
                int k_size_cpy = k_size;
                int min_n_kernel_cpy = min_n_kernel;
                int band_width_cpy = band_width;
                int padding_cpy = padding;

                threads.emplace_back([&, subdirPath, newOutputDir, subdirName,
                    k_size_cpy, min_n_kernel_cpy, band_width_cpy, padding_cpy]() {
                        LOG_LEVEL("INFO", "Started processing " + subdirName);

                        fs::create_directories(newOutputDir);

                        try {
                            processingForDLLDataset(subdirPath, newOutputDir, subdirName,
                                k_size_cpy, min_n_kernel_cpy, band_width_cpy, padding_cpy);
                            LOG_LEVEL("INFO", "Finished processing " + subdirName);
                        }
                        catch (const std::exception& e) {
                            LOG_LEVEL("ERROR", "Error processing " + subdirName + ": " + e.what());
                        }

                        {
                            std::lock_guard<std::mutex> guard(mtx);
                            --active_threads;
                        }
                        cv.notify_one();
                    });
            }
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        LOG_FUNC("EXIT" << " Source_Dir = " << Source_Dir.filename() << ", Output_Dir = " << Output_Dir.filename() << ", Threads = " << max_threads);
    }

    void parseABCtoDatasetAE(fs::path& Source_Dir, fs::path Output_Dir, int& k_size, int& min_n_kernel, int& band_width, int& padding, double param_1, double param_2, double threshold, int random_seed, int max_threads = 0) {

        LOG_FUNC("ENTER" << " Source_Dir = " << Source_Dir.filename() << ", Output_Dir = " << Output_Dir.filename() << ", Threads = " << max_threads);

        fs::create_directories(Output_Dir);

        std::vector<std::thread> threads;
        std::mutex mtx;
        std::condition_variable cv;
        int active_threads = 0;

        if (max_threads == 0) {
            max_threads = std::thread::hardware_concurrency();
        }

        std::cout << "Running on thread count: " << max_threads << std::endl;


        for (const auto& entry : fs::directory_iterator(Source_Dir)) {
            if (!fs::is_directory(entry.status())) continue;

            fs::path subdirPath = entry.path();
            std::string subdirName = subdirPath.filename().string();

            bool hasObj = false;
            bool hasYml = false;

            for (const auto& file : fs::directory_iterator(subdirPath)) {
                if (!fs::is_regular_file(file.status())) continue;

                std::string ext = file.path().extension().string();
                if (ext == ".obj") hasObj = true;
                if (ext == ".yml") hasYml = true;
            }

            if (hasObj && hasYml) {
                fs::path newOutputDir = Output_Dir / subdirName;

                if (checkIfDirWasProcessed(newOutputDir.generic_string())) {
                    std::cout << "Directory " << newOutputDir.filename() << " already processed -> skipped" << std::endl;
                    LOG_FUNC("EXIT " << " Directory " << newOutputDir.filename() << " already processed->skipped");
                    continue;
                }

                // Wait until there is a free thread slot
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]() { return active_threads < max_threads; });
                ++active_threads;
                lock.unlock();

                // Copy values to avoid reference issues
                int k_size_cpy = k_size;
                int min_n_kernel_cpy = min_n_kernel;
                int band_width_cpy = band_width;
                int padding_cpy = padding;

                int param_1_cpy = param_1;
                int param_2_cpy = param_2;
                double threshold_cpy = threshold;
                int seed_cpy = random_seed;

                threads.emplace_back([&, subdirPath, newOutputDir, subdirName,
                    k_size_cpy, min_n_kernel_cpy, band_width_cpy, padding_cpy, param_1_cpy, param_2_cpy, threshold_cpy, seed_cpy]() {
                        LOG_LEVEL("INFO", "Started processing " + subdirName);

                        fs::create_directories(newOutputDir);

                        try {
                            processingForDLLDatasetAE(subdirPath, newOutputDir, subdirName,
                                k_size_cpy, min_n_kernel_cpy, band_width_cpy, padding_cpy,
                                param_1_cpy, param_2_cpy, threshold_cpy, seed_cpy);
                            LOG_LEVEL("INFO", "Finished processing " + subdirName);
                        }
                        catch (const std::exception& e) {
                            LOG_LEVEL("ERROR", "Error processing " + subdirName + ": " + e.what());
                        }

                        {
                            std::lock_guard<std::mutex> guard(mtx);
                            --active_threads;
                        }
                        cv.notify_one();
                    });
            }
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        LOG_FUNC("EXIT" << " Source_Dir = " << Source_Dir.filename() << ", Output_Dir = " << Output_Dir.filename() << ", Threads = " << max_threads);
    }


   


}