#pragma once

#pragma region Tools
#include "Tools.h"
#include "MyTypes.h" // Include the struct definitions
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <unordered_map>
#pragma endregion

#pragma region util
#include <yaml-cpp/yaml.h>
#pragma endregion


#pragma region CGALbased
#include <CGAL/Surface_mesh.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/boost/graph/iterator.h> // For vertices_around_face()
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <CGAL/Polygon_mesh_processing/bbox.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Surface_mesh;

#pragma endregion

#pragma region OpenVDBbased
#include <openvdb/openvdb.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/LevelSetRebuild.h>
#include <openvdb/tools/LevelSetUtil.h>
#include <openvdb/tools/GridOperators.h>

#pragma endregion

namespace Tools {
    

    namespace util {
        std::unordered_map<std::string, int> CountFacesPerSurfaceType(const std::vector<std::vector<std::string>>& FaceToTypeMap);
        std::vector<Tools::MyVertex> CalculateFaceCenters(const std::vector<Tools::MyFace>& faces, const std::vector<Tools::MyVertex>& vertices);
        char mapValueToChar(float value, float minVal, float maxVal);
        void saveTypeMapToBinary(const std::vector<std::vector<std::string>>& data, const std::string& filename);
        void saveFloatMatrix(const std::vector<std::vector<float>>& matrix, const std::string& filename);
        void saveFloat3DGridPythonic(std::string& filename, Float3DArray& array, double& voxelSize, double& background);
        void saveFloat3DGridPythonic(std::string& targetdir, std::string& filename, Float3DArray& array, double& voxelSize, double& background);
        void filterObjFile(const std::string& filePath, const std::string& filter_string);
        void filterObjFile(const std::string& filePath, const std::vector<std::string>& filter_strings);
        std::vector<ABC_Surface> ParseABCyml(std::string& file_name);
        std::vector<std::vector<std::string>> GetVertexToSurfTypeMapYAML(std::string f_name, int n_vertices);
        std::vector<std::vector<std::string>> GetFaceToSurfTypeMapYAML(std::string f_name, int n_faces);
        void saveTypeMapToBinary(const std::vector<std::vector<std::string>>& data, const std::string& filename);
        void saveTypeCountsToBinary(
            const std::unordered_map<std::string, int>& surface_type_counts,
            const std::string& filename);
    }

    namespace CGALbased {
        using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
        using Point = Kernel::Point_3;
        using Surface_mesh = CGAL::Surface_mesh<Point>;

        std::pair<std::vector<MyVertex>, std::vector<MyFace>> GetVerticesAndFaces(Surface_mesh mesh);

        std::vector<int> GetBBoxMinMaxIndex(Surface_mesh& mesh);

        std::vector<double> GetBBoxDimensions(Surface_mesh& mesh);
    }

    namespace OpenVDBbased {
        float getGridMinActiceValue(openvdb::FloatGrid::Ptr grid);

        openvdb::FloatGrid::Ptr MeshToFloatGrid(
            const std::vector<MyVertex>& meshVertices,
            const std::vector<MyFace>& meshFaces,
            float voxelSize,
            float exteriorBandWidth,
            float interiorBandWidth
        );


        openvdb::FloatGrid::Ptr MeshToFloatGrid(
            const std::vector<MyVertex>& meshVertices,
            const std::vector<MyFace>& meshFaces,
            int voxelDim,
            float exteriorBandWidth,
            float interiorBandWidth
        );

        std::vector<std::vector<float>> CoordListToFloatMatrix(std::vector<openvdb::Coord>& coord_list);

        void RemapFloat3DArray(Float3DArray& array, LinearSDFMap& linear_map);

        void NormalizeFloat3DArray(Float3DArray& array, double& voxel_size);

        int ActivateInsideValues(openvdb::FloatGrid::Ptr grid);

        bool CheckIfGridHasValidInsideVoxel(openvdb::FloatGrid::Ptr grid);

        int CountActiveValue(openvdb::FloatGrid::Ptr grid);

        Float3DArray Float3DArrayFromFloatGrid(
            openvdb::FloatGrid::Ptr Floatgrid
        );

        Tools::Float3DArray Float3DArrayFromFloatGrid(
            openvdb::FloatGrid::Ptr FloatGrid,
            int exspected_dim
        );

        openvdb::FloatGrid::Ptr FloatGridFromFloat3DArray(
            Tools::Float3DArray floatArray
        );

        void GridAddWaveFunction(
            openvdb::FloatGrid::Ptr Floatgrid, 
            float amp,
            float n_period, 
            float disp,
            float direction[3]
        );

        void applyWaveDeformation(
            openvdb::FloatGrid::Ptr sdfGrid,
            float amplitude,
            float frequency,
            WaveType waveType
        );

        openvdb::FloatGrid::Ptr resizeToEvenGrid(
            openvdb::FloatGrid::Ptr unevenGrid,
            int targetDimX,
            int targetDimY,
            int targetDimZ
        );

        std::vector<double> DetermineBoundingBox(std::vector<openvdb::Vec3s> points);

        std::vector<std::vector<float>> TransformWorldPointsToIndexFloatArray(openvdb::FloatGrid::Ptr& grid, std::vector<MyVertex>& vertex_list);
    }
}