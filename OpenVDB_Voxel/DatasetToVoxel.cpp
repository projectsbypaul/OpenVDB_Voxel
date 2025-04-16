#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include <CGAL/Bbox_3.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

#include "MeshDataHandling.h"
#include "Tools.h"
#include "DatasetToVoxel.h"

namespace fs = std::filesystem;

// Function to process and save the modified file with a new extension
void processFile(const fs::path& srcFilePath, const fs::path& destDir, const std::string& newExtension) {

  
    // Create a destination file path with the new extension
    fs::path destFilePath = destDir / srcFilePath.filename();
    destFilePath.replace_extension(newExtension);

    std::ifstream srcFile(srcFilePath);
    //std::ofstream destFile(destFilePath);

    //modify File

    Surface_mesh mesh;

    std::string in_file = srcFilePath.string();

    if (!MDH::readMesh(&in_file, &mesh)) {
        std::cerr << "Cannot open file " << srcFilePath.string() << std::endl;
        return;
    }

    auto [meshVertices, meshFaces] = Tools::CGALbased::GetVerticesAndFaces(mesh);

    int voxel_count = 32;

    // Define narrow-band widths
    float exBandWidth = 5.0f;
    float inBandWidth = 5.0f;

    auto sdfGrid = Tools::OpenVDBbased::MeshToFloatGrid(meshVertices, meshFaces, voxel_count, exBandWidth, inBandWidth);

    std::cout << "MeshToFloatGrid "<< "done" << std::endl;

    int test = Tools::OpenVDBbased::ActivateInsideValues(sdfGrid);

    std::cout << "ActivateInsideValues " << "done" << std::endl;

    double voxelSize = sdfGrid->transform().voxelSize()[0];

    double background = sdfGrid->tree().background();

    //if (Tools::OpenVDBbased::CheckIfGridHasValidInsideVoxel(sdfGrid)) {

    Tools::Float3DArray floatArray = Tools::OpenVDBbased::Float3DArrayFromFloatGrid(sdfGrid, voxel_count);

    std::cout << "Float3DArrayFromFloatGrid " << "done" << std::endl;

    Tools::LinearSDFMap map;

    float min = Tools::OpenVDBbased::getGridMinActiceValue(sdfGrid);

    map.create(min, background, -1, 1);

    Tools::OpenVDBbased::RemapFloat3DArray(floatArray, map);

    std::cout << "RemapFloat3DArray " << "done" << std::endl;

    std::string out_dir = destDir.string();

    std::string fname = destFilePath.filename().string();

    Tools::util::saveFloat3DGridPythonic(out_dir, fname, floatArray, voxelSize, background);

    std::cout << "saveFloat3DGridPythonic " << "done" << std::endl;

    //Done 
    std::cout << srcFilePath.filename() << "// Modified and Copied to target" << std::endl;

    srcFile.close();
    //destFile.close();

    std::cout << "Processed: " << srcFilePath << " -> " << destFilePath << std::endl;
    //}
    //else
    //{
    //    std::cout << srcFilePath.filename() << " has no valid inside voxels -> processing skipped" << std::endl;
    //    std::cout << "Reasons: mesh is not watertight or 1 dimension to small for selected Bandwidth" << std::endl;
    //}

    
    
}

// Function to copy and process files while maintaining structure
void processDirectory(const fs::path& srcDir, const fs::path& destDir, const std::string& newExtension) {
    if (!fs::exists(srcDir) || !fs::is_directory(srcDir)) {
        std::cerr << "Source directory does not exist or is not a directory!" << std::endl;
        return;
    }

    // Create the destination directory if it doesn't exist
    fs::create_directories(destDir);

    for (const auto& entry : fs::recursive_directory_iterator(srcDir)) {
        const auto& srcPath = entry.path();
        auto relativePath = fs::relative(srcPath, srcDir);
        auto destPath = destDir / relativePath;

        if (fs::is_directory(srcPath)) {
            fs::create_directories(destPath);
        }
        else if (fs::is_regular_file(srcPath) && srcPath.extension() == ".off") {
           
           processFile(srcPath, destDir / relativePath.parent_path(), newExtension);
         
        }
        else {
            // Copy non-.off files directly
            //fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);
        }
    }
}

void ParseDataset(fs::path& srcDir, fs::path& destDir, std::string& newExtension) {

    processDirectory(srcDir, destDir, newExtension);

    std::cout << "Processing complete. Edited files saved in: " << destDir << std::endl;

    return;
}
