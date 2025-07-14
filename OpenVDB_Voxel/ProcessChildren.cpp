#include "ProcessChildren.h"
#include <iostream> // For demonstration purposes

#include "Tools.h"
#include "DatasetStats.h"
#include "DL_Preprocessing.h"
#include "NoiseOnMesh.h"
#include "DataContainer.h"
#include "LOG.h"
#include "MeshDataHandling.h"


namespace ProcessingUtility {
    /// <summary>
    /// Implementation of ProcessForDLLDataset
    /// </summmary>
    /// 
    /**
     * @brief Constructs a ProcessForDLLDataset object.
     * @brief Determine resolution by defining a voxel_size or n_min_kernel
     *
     * @param sourceDir The path to the main source directory.
     * @param targetDir The path to the main target directory.
     * @param kernel_size Size if the cropping kernel
     * @param padding Overlap of cropping kernel
     * @param bandwidth Bandwidth for sdf creation inside openvdd
     * @param voxel_size Voxel Size used to calculate SDF grid
     * @param n_min_kernel Minimal ammount of cropping kernel required to fit in the smallest dimension if the mesh
     */
    ProcessForDLLDataset::ProcessForDLLDataset(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, int n_min_kernel)
        : GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), padding_(padding), bandwidth_(bandwidth), n_min_kernel_(n_min_kernel), voxel_size_(0) {
        std::cout << "Process will run in Mode - " << "adaptive voxel size" << std::endl;
    }

    ProcessForDLLDataset::ProcessForDLLDataset(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, double voxel_size)
        : GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), padding_(padding), bandwidth_(bandwidth), n_min_kernel_(0), voxel_size_(voxel_size) {
        std::cout << "Process will run in Mode - " << "fixed voxel size" << std::endl;
    }
    /**s
    * @brief Executes the DLL dataset processing logic for the specified subdirectory.
    *
    * @param subDirPath The specific subdirectory path within the source directory to process.
    * @override
    */
    void ProcessForDLLDataset::run(const std::string& subDirName)
    {
        LOG_FUNC("ENTER" << " subdirName = " << subDirName << ", outputDir = " << targetDir_);

        std::cout << "Processing: " << subDirName << " -> Output: " << targetDir_ << std::endl;

        //Define source file and traget file location
        std::string yml_name = (sourceDir_ / subDirName / subDirName).generic_string() + ".yml";
        std::string obj_name = (sourceDir_ / subDirName / subDirName).generic_string() + ".obj";
        std::string target_dir = (targetDir_ / subDirName).generic_string();

        fs::create_directories(target_dir);

        //load obj into cgal surface mesh
        //remember clean obj from "vc" lines 
        std::ifstream input(obj_name);
        Surface_mesh mesh;



        if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
            std::cerr << "Failed to read . file!" << std::endl;
            LOG_FUNC("EXIT" << " subdirName = " << subDirName << "outputDir = " << targetDir_ << " Failed to read .obj file!");
            return;
        }

        //determine definition of voxel_size based on class initialization
        double voxel_size;

        if (voxel_size_ == 0) {
            //dertimine Reccomende voxel size 
            voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(kernel_size_, n_min_kernel_, bandwidth_, padding_, mesh);
        }
        else {
            voxel_size = voxel_size_;
        }


        //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);
        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)bandwidth_, std::numeric_limits<float>::max());


        //based on the cropping parameter -> calculate a origin for each cropping segemnent 
        //save origin as binary for reconstruction of labled data a remapping of segmentation resulst 
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, kernel_size_, padding_);


        if (crop_list.size() < 300) {
            auto orgin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
            std::string origin_bin = target_dir + "/origins" + ".bin";
            Tools::util::saveFloatMatrix(orgin_list, origin_bin);

            //Create face to type map and save it a binary
            int n_face = my_faces.size();
            auto face_list = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_face);
            std::string face_bin = target_dir + "/FaceTypeMap" + ".bin";
            Tools::util::saveTypeMapToBinary(face_list, face_bin);

            //Create vertex to type map and save it a binary
            int n_vert = my_verts.size();
            auto vert_list = Tools::util::GetVertexToSurfTypeMapYAML(yml_name, n_vert);
            std::string vert_bin = target_dir + "/VertTypeMap" + ".bin";
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
            std::string FaceToGridIndex_bin = target_dir + "/FaceToGridIndex.bin";
            Tools::util::saveFloatMatrix(FaceToGridIndex, FaceToGridIndex_bin);

            //Count Faces per SurfaceType and save counts binary
            auto counts = Tools::util::CountFacesPerSurfaceType(face_list);
            auto count_bin = target_dir + "/TypeCounts.bin";
            Tools::util::saveTypeCountsToBinary(counts, count_bin);

            //Set up linear map for normalization
            Tools::LinearSDFMap lmap;

            double background = grid->tree().background();
            float minVal = Tools::OpenVDBbased::getGridMinActiceValue(grid);

            lmap.create(minVal, background, 0, 1);

            //Create a an array that holds cropping results
            Tools::Float3DArray clipped_array;

            //crop sdf grid and write cropping result into 3D float array
            //save cropped segments into binary file
            for (size_t i = 0; i < crop_list.size(); ++i) {
                clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], kernel_size_);
                Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap); //normalize clipped array 
                std::string f_name = (targetDir_ / subDirName / subDirName).generic_string() + "_" + std::to_string(i) + ".bin";
                Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, background);
            }

        }
        else
        {
            std::cout << subDirName << +".bin " << "is odd sized --> skipped";
            LOG_FUNC("EXIT" << " subdirName = " << subDirName << " outputDir = " << targetDir_ << "is odd sized --> skipped");

        }
        LOG_FUNC("EXIT" << " subdirName = " << subDirName << " outputDir = " << targetDir_);

    }

    /// <summary>
    /// Implementation of ProcessForDLLDataset
    /// </summmary>
    /// 
    /**
     * @brief Constructs a ProcessForDLLDataset object.
     * @brief Determine resolution by defining a voxel_size or n_min_kernel
     *
     * @param sourceDir The path to the main source directory.
     * @param targetDir The path to the main target directory.
     * @param kernel_size Size if the cropping kernel
     * @param padding Overlap of cropping kernel
     * @param bandwidth Bandwidth for sdf creation inside openvdd
     * @param voxel_size Voxel Size used to calculate SDF grid
     * @param n_min_kernel Minimal ammount of cropping kernel required to fit in the smallest dimension if the mesh
     */
    ProcessWithDumpTruck::ProcessWithDumpTruck(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, int n_min_kernel)
        : GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), padding_(padding), bandwidth_(bandwidth), n_min_kernel_(n_min_kernel), voxel_size_(0) {
        std::cout << "Process will run in Mode - " << "adaptive voxel size" << std::endl;
    }

    ProcessWithDumpTruck::ProcessWithDumpTruck(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, double voxel_size)
        : GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), padding_(padding), bandwidth_(bandwidth), n_min_kernel_(0), voxel_size_(voxel_size) {
        std::cout << "Process will run in Mode - " << "fixed voxel size" << std::endl;
    }
    /**s
    * @brief Executes the DLL dataset processing logic for the specified subdirectory.
    *
    * @param subDirPath The specific subdirectory path within the source directory to process.
    * @override
    */
    void ProcessWithDumpTruck::run(const std::string& subDirName)
    {
        LOG_FUNC("ENTER" << " subdirName = " << subDirName << ", outputDir = " << targetDir_);

        std::cout << "Processing: " << subDirName << " -> Output: " << targetDir_ << std::endl;

        //Define source file and traget file location
        std::string yml_name = (sourceDir_ / subDirName / subDirName).generic_string() + ".yml";
        std::string obj_name = (sourceDir_ / subDirName / subDirName).generic_string() + ".obj";
        std::string target_dir = (targetDir_ / subDirName).generic_string();

        //load obj into cgal surface mesh
        //remember clean obj from "vc" lines 
        std::ifstream input(obj_name);

        std::vector<Tools::MyVertex> my_verts;
        std::vector<Tools::MyFace> my_faces;

        double voxel_size;

        {
            Surface_mesh mesh;

            if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
                std::cerr << "Failed to read .obj file!" << std::endl;
                LOG_FUNC("EXIT" << " subdirName = " << subDirName << "outputDir = " << targetDir_ << " Failed to read .obj file!");
                return;
            }

            //determine definition of voxel_size based on class initialization
           

            if (voxel_size_ == 0) {
                //dertimine Reccomende voxel size 
                voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(kernel_size_, n_min_kernel_, bandwidth_, padding_, mesh);
            }
            else {
                voxel_size = voxel_size_;
            }

            //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
            std::tie(my_verts, my_faces) = Tools::CGALbased::GetVerticesAndFaces(mesh);

        }

        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)bandwidth_, std::numeric_limits<float>::max());

        //based on the cropping parameter -> calculate a origin for each cropping segemnent 
        //save origin as binary for reconstruction of labled data a remapping of segmentation resulst 
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, kernel_size_, padding_);


        //setup data container
        cppIOUtility::SegmentationDataContainer DumpTruck;



        if (crop_list.size() < 500) {

         
            {
                std::vector<std::vector<float>> origin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
                DumpTruck.setOriginContainer(origin_list);
            }

            //Create face to type map and save it a binary
            {
                int n_face = my_faces.size();
                std::vector<std::vector<std::string>> face_list = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_face);
                DumpTruck.setFaceTypeMap(face_list);

                //Count Faces per SurfaceType and save counts binary
                auto counts = Tools::util::CountFacesPerSurfaceType(face_list);
                DumpTruck.setTypeCount(counts);

            }
            //Create vert to type map
            {
                int n_verts = my_verts.size();
                std::vector<std::vector<std::string>> vert_list = Tools::util::GetVertexToSurfTypeMapYAML(yml_name, n_verts);
                DumpTruck.setVertTypeMap(vert_list);
            }

            
            //Create a Face to Grid centered index map and save it as binary
            {
                std::vector<Tools::MyVertex> face_centers = Tools::util::CalculateFaceCenters(my_faces, my_verts);
                Tools::FloatMatrix FaceToGridIndex = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, face_centers);
                DumpTruck.setFaceToGridIndex_container(FaceToGridIndex);
            }
            
            //Set up linear map for normalization
            Tools::LinearSDFMap lmap;

            double background = grid->tree().background();
            float minVal = Tools::OpenVDBbased::getGridMinActiceValue(grid);
            
            DumpTruck.setBackground(background);
            DumpTruck.setVoxelSize(voxel_size);
            DumpTruck.setMinVal(minVal);

            //map for normalization 
            lmap.create(-background, background, -1, 1);

            //Create a an array that holds cropping results
            Tools::Float3DArray clipped_array;

            //crop sdf grid and write cropping result into 3D float array
            //save cropped segments into binary file
            for (size_t i = 0; i < crop_list.size(); ++i) {
                clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], kernel_size_);

                //remapping with backgroud clamp values smaller than -background to min_map
                // compensation for infinite inside bandwidth
                Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap, background); 

                DumpTruck.addSegment(clipped_array);

            }

            DumpTruck.dump(target_dir);

        }
        else
        {
            std::cout << subDirName << +".bin " << "is odd sized --> skipped";
            LOG_FUNC("EXIT" << " subdirName = " << subDirName << " outputDir = " << targetDir_ << "is odd sized --> skipped");

        }
        LOG_FUNC("EXIT" << " subdirName = " << subDirName << " outputDir = " << targetDir_);

    }

    /// <summary>
    /// Implementation of ProcessForDLLDatasetae
    /// </summmary>
    /// 
    /**
    * @brief Constructs a ProcessForDLLDataset object.
    * @brief Determine resolution by defining a voxel_size or n_min_kernel
    *
    * @param sourceDir The path to the main source directory.
    * @param targetDir The path to the main target directory.
    * @param kernel_size Size if the cropping kernel
    * @param padding Overlap of cropping kernel
    * @param bandwidth Bandwidth for sdf creation inside openvdd
    * @param voxel_size Voxel Size used to calculate SDF grid
    * @param n_min_kernel Minimal ammount of cropping kernel required to fit in the smallest dimension if the mesh
    * @param param_1 Parmeter for noise amplitude an frequency
    * @param param_2 Parmeter for noise amplitude an frequency
    * @param threshold Threshold to determine which faces are removed
    * @param randomseed Randomseed for noise generation
    */
    ProcessForDLLDatasetAE::ProcessForDLLDatasetAE(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int n_min_kernel, int bandwidth, int padding, double param_1, double param_2, double threshold, int random_seed) :
        GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), n_min_kernel_(n_min_kernel), bandwidth_(bandwidth), padding_(padding), param_1_(param_1), param_2_(param_2_), threshold_(threshold), random_seed_(random_seed)
    {
        voxel_size_ = 0;
        std::cout << "Process will run in Mode - " << "adaptive voxel size" << std::endl;
    }

    ProcessForDLLDatasetAE::ProcessForDLLDatasetAE(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, double voxel_size, int bandwidth, int padding, double param_1, double param_2, double threshold, int random_seed) :
        GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), voxel_size_(voxel_size), bandwidth_(bandwidth), padding_(padding), param_1_(param_1), param_2_(param_2_), threshold_(threshold), random_seed_(random_seed)
    {
        std::cout << "Process will run in Mode - " << "fixed voxel size" << std::endl;
    }

    void ProcessForDLLDatasetAE::run(const std::string& subDirName)
    {
        LOG_FUNC("ENTER" << " subdirName = " << subDirName << ", outputDir = " << targetDir_);

        std::cout << "Processing: " << subDirName << " -> Output: " << targetDir_ << std::endl;



        //Define source file and traget file location
        std::string yml_name = (sourceDir_ / subDirName / subDirName).generic_string() + ".yml";
        std::string obj_name = (sourceDir_ / subDirName / subDirName).generic_string() + ".obj";
        std::string target_dir = (targetDir_ / subDirName).generic_string();

        //load obj into cgal surface mesh
        //remember clean obj from "vc" lines 
        std::ifstream input(obj_name);
        Surface_mesh mesh;

        if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
            std::cerr << "Failed to read .obj file!" << std::endl;
            LOG_FUNC("EXIT" << " subdirName = " << subDirName << "outputDir = " << targetDir_ << " Failed to read .obj file!");
            return;
        }

        //Add noise to mesh 
        int removed = NoiseOnMesh::CGALbased::applySwirlyNoise(&mesh, param_1_, param_2_, threshold_, random_seed_);

        std::cout << "Removed " << removed << " Faces from" << subDirName << std::endl;


        //dertimine Reccomende voxel size 
        double voxel_size;
        if (voxel_size_ == 0) {
            voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(kernel_size_, n_min_kernel_, bandwidth_, padding_, mesh);
        }
        else
        {
            voxel_size = voxel_size_;
        }


        //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);
        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)bandwidth_, std::numeric_limits<float>::max());


        //based on the cropping parameter -> calculate a origin for each cropping segemnent 
        //save origin as binary for reconstruction of labled data a remapping of segmentation resulst 
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, kernel_size_, padding_);

        if (crop_list.size() < 1000) {
            auto orgin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
            std::string origin_bin = target_dir + "/origins" + ".bin";
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
                clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], kernel_size_);
                Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap); //normalize clipped array 
                std::string f_name = (targetDir_ / subDirName / subDirName).generic_string() + "_" + std::to_string(i) + ".bin";
                Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, background);
            }

        }
        else
        {
            std::cout << subDirName << +".bin " << "is odd sized --> skipped";
            LOG_FUNC("EXIT" << " subdirName = " << subDirName << "outputDir = " << targetDir_ << "is odd sized --> skipped");

        }
        LOG_FUNC("EXIT" << " subdirName = " << subDirName << "outputDir = " << targetDir_);
    }


    ProcessObjStrip::ProcessObjStrip(const fs::path& sourceDir, const fs::path& targetDir) : GenericDirectoryProcess(sourceDir, targetDir)
    {
    }

    void ProcessObjStrip::run(const std::string& subDirName)
    {
        fs::path file = (sourceDir_ / subDirName) / (subDirName + extension_);
        Tools::util::filterObjFile(file.generic_string(), filter_);
    }

    ProcessSimpleSegmentation::ProcessSimpleSegmentation(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, int n_min_kernel)
        : GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), padding_(padding), bandwidth_(bandwidth), n_min_kernel_(n_min_kernel), voxel_size_(0) {
        std::cout << "Process will run in Mode - " << "adaptive voxel size" << std::endl;
    }

    ProcessSimpleSegmentation::ProcessSimpleSegmentation(const fs::path& sourceDir, const fs::path& targetDir, int kernel_size, int padding, int bandwidth, double voxel_size)
        : GenericDirectoryProcess(sourceDir, targetDir), kernel_size_(kernel_size), padding_(padding), bandwidth_(bandwidth), n_min_kernel_(0), voxel_size_(voxel_size) {
        std::cout << "Process will run in Mode - " << "fixed voxel size" << std::endl;
    }

    void ProcessSimpleSegmentation::run(const std::string& subDirName)
    {
        LOG_FUNC("ENTER" << " subdirName = " << subDirName << ", outputDir = " << targetDir_);

        std::cout << "Processing: " << subDirName << " -> Output: " << targetDir_ << std::endl;

        //Define source file and traget file location
  
        std::string mesh_file_name = sourceDir_.generic_string();
        std::string target_dir = (targetDir_ / subDirName).generic_string();

        //load obj into cgal surface mesh
        //remember clean obj from "vc" lines 

        std::vector<Tools::MyVertex> my_verts;
        std::vector<Tools::MyFace> my_faces;

        double voxel_size;

        {
            Surface_mesh mesh;

            

            if (!MDH::readMesh(&mesh_file_name, &mesh)) {
                std::cerr << "Failed to read mesh file!" << std::endl;
                LOG_FUNC("EXIT" << " subdirName = " << subDirName << "outputDir = " << targetDir_ << " Failed to read mesh file!");
                return;
            }

            //determine definition of voxel_size based on class initialization

            if (voxel_size_ == 0) {
                //dertimine Reccomende voxel size 
                voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(kernel_size_, n_min_kernel_, bandwidth_, padding_, mesh);
            }
            else {
                voxel_size = voxel_size_;
            }

            //extract verts and faces from CGAL mesh and create SDF Grid in OpenVDB 
            std::tie(my_verts, my_faces) = Tools::CGALbased::GetVerticesAndFaces(mesh);
        }

        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, (float)voxel_size, (float)bandwidth_, std::numeric_limits<float>::max());

        //based on the cropping parameter -> calculate a origin for each cropping segemnent 
        //save origin as binary for reconstruction of labled data a remapping of segmentation resulst 
        auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, kernel_size_, padding_);


        //setup data container
        cppIOUtility::SegmentationDataContainer DumpTruck;

        {
            std::vector<std::vector<float>> origin_list = Tools::OpenVDBbased::CoordListToFloatMatrix(crop_list);
            DumpTruck.setOriginContainer(origin_list);
        }

        //Create a Face to Grid centered index map and save it as binary
        {
            std::vector<Tools::MyVertex> face_centers = Tools::util::CalculateFaceCenters(my_faces, my_verts);
            Tools::FloatMatrix FaceToGridIndex = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, face_centers);
            DumpTruck.setFaceToGridIndex_container(FaceToGridIndex);
        }

        //Set up linear map for normalization
        Tools::LinearSDFMap lmap;

        double background = grid->tree().background();
        float minVal = Tools::OpenVDBbased::getGridMinActiceValue(grid);

        DumpTruck.setBackground(background);
        DumpTruck.setVoxelSize(voxel_size);
        DumpTruck.setMinVal(minVal);

        //map for normalization
        lmap.create(-background, background, -1, 1);

        //Create a an array that holds cropping results
        Tools::Float3DArray clipped_array;

        //crop sdf grid and write cropping result into 3D float array
        //save cropped segments into binary file
        for (size_t i = 0; i < crop_list.size(); ++i) {
            clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i], kernel_size_);

            //Remapping with background to clamp "values < -background" to min_map 
            Tools::OpenVDBbased::RemapFloat3DArray(clipped_array, lmap, background); //normalize clipped array 
            DumpTruck.addSegment(clipped_array);

        }

        DumpTruck.dump(target_dir);

        LOG_FUNC("EXIT" << " subdirName = " << subDirName << " outputDir = " << targetDir_);
    }

    ProcessGetStats::ProcessGetStats(const fs::path& sourceDir, const fs::path& targetDir, std::string temp_file_name)
        : GenericDirectoryProcess(sourceDir, targetDir), temp_file_name_(temp_file_name)
    {
    }

    void ProcessGetStats::run(const std::string& subDirName) {
        fs::path temp_file = targetDir_ / "temp_stats" / temp_file_name_;
     

        if (!fs::exists(temp_file.parent_path())) {
            fs::create_directories(temp_file.parent_path());
        }

        fs::path file = (targetDir_ / subDirName) / target_file_name_;
        fs::path bin_file = (targetDir_ / subDirName) / target_bin_file_name_;

        if (fs::exists(file) && fs::exists(bin_file)) {
           
            // Accumulated stats
            size_t total_segments = 0;
            size_t file_count = 0;
            size_t min_segments = std::numeric_limits<size_t>::max();
            size_t max_segments = 0;
            std::map<std::string, size_t> type_counts;

            // === Load from temp file if it exists ===
            if (fs::exists(temp_file)) {
                std::ifstream in(temp_file);
                if (in.is_open()) {
                    std::string line;
                    bool in_type_counts = false;

                    while (std::getline(in, line)) {
                        line.erase(0, line.find_first_not_of(" \t"));
                        line.erase(line.find_last_not_of(" \t") + 1);

                        if (line.empty() || line[0] == '#') continue;

                        if (line == "[TYPE_COUNTS]") {
                            in_type_counts = true;
                            continue;
                        }
                        if (line == "[END_TYPE_COUNTS]") {
                            in_type_counts = false;
                            continue;
                        }

                        if (line.find("total_segments:") == 0) {
                            std::istringstream iss(line.substr(line.find(":") + 1));
                            iss >> total_segments;
                        }
                        else if (line.find("file_count:") == 0) {
                            std::istringstream iss(line.substr(line.find(":") + 1));
                            iss >> file_count;
                        }
                        else if (line.find("min_segments:") == 0) {
                            std::istringstream iss(line.substr(line.find(":") + 1));
                            iss >> min_segments;
                        }
                        else if (line.find("max_segments:") == 0) {
                            std::istringstream iss(line.substr(line.find(":") + 1));
                            iss >> max_segments;
                        }
                        else if (in_type_counts) {
                            auto colon = line.find(":");
                            if (colon != std::string::npos) {
                                std::string type = line.substr(0, colon);
                                type.erase(type.find_last_not_of(" \t") + 1);
                                size_t count = 0;
                                std::istringstream iss(line.substr(colon + 1));
                                iss >> count;
                                type_counts[type] += count;
                            }
                        }
                    }

                    in.close();
                }
                else {
                    std::cerr << "Failed to open temp file for reading: " << temp_file << std::endl;
                }
            }

            // === Process current subdir file ===
            size_t seg_count = Tools::DatasetStats::Functions::read_segment_count(file.string());
            total_segments += seg_count;
            file_count += 1;

            min_segments = std::min(min_segments, seg_count);
            max_segments = std::max(max_segments, seg_count);

            auto face_type_map = Tools::DatasetStats::Functions::read_face_type_map(file.string());
            auto type_to_faces = Tools::DatasetStats::Functions::build_type_to_faces_map(face_type_map);

            for (const auto& [type, faces] : type_to_faces) {
                type_counts[type] += faces.size();
            }

            // === Write merged data back to temp file ===
            std::ofstream out(temp_file);
            if (!out.is_open()) {
                std::cerr << "Failed to open temp file for writing: " << temp_file << std::endl;
                return;
            }

            out << "# Accumulated statistics\n";
            out << "file_count: " << file_count << "\n";
            out << "total_segments: " << total_segments << "\n";
            out << "min_segments: " << min_segments << "\n";
            out << "max_segments: " << max_segments << "\n";

            out << "[TYPE_COUNTS]\n";
            for (const auto& [type, total] : type_counts) {
                out << type << ": " << total << "\n";
            }
            out << "[END_TYPE_COUNTS]\n";

            out.close();

            std::cout << "Updated statistics written to: " << temp_file << std::endl;
        }
        else
        {
            std::cerr << "File does not exist: " << file << std::endl;
        }
    }


}
