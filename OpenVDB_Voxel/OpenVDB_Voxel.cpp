#include "Tools.h"
#include <memory>
#include "MeshDataHandling.h"
#include "Scripts.h"


int main() {
    //TO DO Add binary rep
    std::string filename = "C:/Local_Data/CGAL-6.0.1_examples/data/meshes/bunny00.off";

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

    Tools::LinearSDFMap map; 

    map.create(-1, 1, background);

    Tools::OpenVDBbased::RemapFloatGrid(sdfGrid, map);

    Tools::Float3DArray floatArray = Tools::OpenVDBbased::Float3DArrayFromFloatGrid(sdfGrid, voxel_count);

    //auto NewGrid = Tools::OpenVDBbased::FloatGridFromFloat3DArray(floatArray);

    std::string fname = "floatArray.bin";

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