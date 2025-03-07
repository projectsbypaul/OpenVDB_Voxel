#include "Tools.h"
#include <memory>
#include <openvdb/tools/Composite.h>
#include "MeshDataHandling.h"
#include "Scripts.h"

#include <CGAL/Bbox_3.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

namespace Scripts {

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



