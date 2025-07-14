#include "../include/IO_Test.h"

namespace Test {
	int TestFaceMapping() {

        std::string yml_name = R"(C:\Local_Data\ABC\ABC_parsed_files\00000066\00000066.yml)";
        std::string obj_name = R"(C:\Local_Data\ABC\ABC_parsed_files\00000066\00000066.obj)";

        std::string target_dir = R"(C:\Local_Data\ABC\ABC_Testing)";

        Surface_mesh mesh;


        int k_size = 16;
        int n_min_k = 2;
        int padding = 4;
        int bandwidth = 5;

        std::ifstream input(obj_name);

        if (!input || !CGAL::IO::read_OBJ(input, mesh)) {
            std::cerr << "Failed to read .obj file!" << std::endl;
            return 1;
        }

        auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

        auto rec_voxelsize = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, n_min_k, bandwidth, padding, mesh);

        openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, rec_voxelsize, (float)bandwidth, std::numeric_limits<float>::max());

        int n_faces = my_faces.size();
         
        auto face_type_map = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_faces);
        auto face_type_map_bin = target_dir + "/FaceTypeMap.bin";
        Tools::util::saveTypeMapToBinary(face_type_map, face_type_map_bin);

        auto counts = Tools::util::CountFacesPerSurfaceType(face_type_map);
        for (const auto& [type, count] : counts) {
            std::cout << "Surface type '" << type << "' has " << count << " faces." << std::endl;
        }

        auto count_bin = target_dir + "/TypeCounts.bin";
        
        Tools::util::saveTypeCountsToBinary(counts, count_bin);

        auto face_centers = Tools::util::CalculateFaceCenters(my_faces, my_verts);

        auto FaceToGridIndex = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, face_centers);
        std::string FaceToGridIndex_bin = target_dir + "/FaceToGridIndex.bin";
        Tools::util::saveFloatMatrix(FaceToGridIndex, FaceToGridIndex_bin);

		return 0;
	}
}