#include "Tools.h"
#include <memory>
#include <iostream>
#include <fstream>
#include "MeshDataHandling.h"
#include "Scripts.h"
#include "DatasetToVoxel.h"
#include <filesystem>
#include <fstream>
#include <string>

#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/LevelSetUtil.h>
#include <openvdb/tools/Composite.h>
#include <openvdb/tools/GridTransformer.h>

#include <CGAL/IO/OBJ.h>
#include "DL_Preprocessing.h"


int main() {

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

    auto rec_voxelsize = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, n_min_k, bandwidth, padding, mesh);

    auto [my_verts, my_faces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

    openvdb::FloatGrid::Ptr grid = Tools::OpenVDBbased::MeshToFloatGrid(my_verts, my_faces, rec_voxelsize, (float)bandwidth, std::numeric_limits<float>::max());

    int n_face = my_faces.size();

    int n_vert = my_verts.size();

    auto vert_list = Tools::util::GetVertexToSurfTypeMapYAML(yml_name, n_vert);

    auto face_list = Tools::util::GetFaceToSurfTypeMapYAML(yml_name, n_face);

    auto arr = Tools::OpenVDBbased::TransformWorldPointsToIndexFloatArray(grid, my_verts);

   
    auto crop_list = DLPP::OpenVDBbased::calculateCroppingOrigins(grid, k_size, padding);

    Tools::Float3DArray clipped_array;

    std::string target_dir = "C:/Local_Data/cropping_test";

    double bckgrd = grid->tree().background();

    double voxel_size = (double)rec_voxelsize;

    for (size_t i = 0; i < crop_list.size(); ++i) {
        clipped_array = DLPP::OpenVDBbased::KernelCropFloatGridFromCoord(grid, crop_list[i] , k_size);
        std::string f_name = target_dir + "/cropped_" + std::to_string(i) + ".bin";
        Tools::util::saveFloat3DGridPythonic(f_name, clipped_array, voxel_size, bckgrd);
    }
    
    


    return 0;
}

