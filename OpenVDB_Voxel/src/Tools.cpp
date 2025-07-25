﻿#include "../include/Tools.h"
#include "../include/MyTypes.h"
#include "../include/binArrayReader.h"

#include <filesystem>

namespace fs = std::filesystem;


namespace Tools {

    namespace util {

        std::unordered_map<std::string, int> CountFacesPerSurfaceType(const std::vector<std::vector<std::string>>& FaceToTypeMap) {
            std::unordered_map<std::string, int> type_counts;

            for (const auto& face_types : FaceToTypeMap) {
                for (const auto& surf_type : face_types) {
                    type_counts[surf_type]++;
                }
            }

            return type_counts;
        }


        std::vector<Tools::MyVertex> CalculateFaceCenters(const std::vector<Tools::MyFace>& faces, const std::vector<Tools::MyVertex>& vertices) {

            std::vector<Tools::MyVertex> face_centers(faces.size());

            for (size_t i = 0; i < faces.size(); ++i) {
                const Tools::MyFace& f = faces[i];

                Tools::MyVertex center{};

                center.x = (vertices[f.v0].x + vertices[f.v1].x + vertices[f.v2].x) / 3.0f;
                center.y = (vertices[f.v0].y + vertices[f.v1].y + vertices[f.v2].y) / 3.0f;
                center.z = (vertices[f.v0].z + vertices[f.v1].z + vertices[f.v2].z) / 3.0f;

                face_centers[i] = center;
            }

            return face_centers;
        }

        void saveTypeMapToBinary(const std::vector<std::vector<std::string>>& data, const std::string& filename) {
            std::ofstream out(filename, std::ios::binary);
            for (const auto& row : data) {
                uint32_t rowSize = static_cast<uint32_t>(row.size());
                out.write(reinterpret_cast<char*>(&rowSize), sizeof(rowSize));
                for (const auto& str : row) {
                    uint32_t strSize = static_cast<uint32_t>(str.size());
                    out.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
                    out.write(str.data(), strSize);
                }
            }
            out.close();
        }

        void saveFloatMatrix(const std::vector<std::vector<float>>& matrix, const std::string& filename) {
            std::ofstream out(filename, std::ios::binary);
            for (const auto& row : matrix) {
                uint32_t rowSize = static_cast<uint32_t>(row.size());
                out.write(reinterpret_cast<const char*>(&rowSize), sizeof(rowSize));
                out.write(reinterpret_cast<const char*>(row.data()), rowSize * sizeof(float));
            }
            out.close();
        }


        char mapValueToChar(float value, float minVal, float maxVal) {
            // Normalize the value to a [0, 1] range.
            float normalized = (value - minVal) / (maxVal - minVal);

            // Multiply by 5 (the number of steps) to determine the bucket.
            int index = static_cast<int>(normalized * 5);

            // Clamp index to 0-4 (handles the edge case when value == maxVal)
            if (index >= 5) index = 4;

            // Define a gradient of characters from low to high intensity.
            const char gradient[5] = { ' ', '.', '*', 'O', '#' };

            return gradient[index];
        }

        void saveFloat3DGridPythonic(std::string& filename, Float3DArray& array, double& voxelSize, double& background) {

            std::ofstream file(filename, std::ios::binary);
            if (!file) {
                std::cerr << "Error opening file for writing!" << std::endl;
                return;
            }

            size_t dim1 = array.size();
            size_t dim2 = array[0].size();
            size_t dim3 = array[0][0].size();

            // Write metadata: dimension (as double) and sizes
            file.write(reinterpret_cast<const char*>(&voxelSize), sizeof(double));
            file.write(reinterpret_cast<const char*>(&background), sizeof(double));

            file.write(reinterpret_cast<const char*>(&dim1), sizeof(size_t));
            file.write(reinterpret_cast<const char*>(&dim2), sizeof(size_t));
            file.write(reinterpret_cast<const char*>(&dim3), sizeof(size_t));

            // Write array data
            for (const auto& mat : array) {
                for (const auto& row : mat) {
                    file.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(float));
                }
            }

            file.close();
            // std::cout << "Data saved to " << filename << std::endl;

        }

        void saveFloat3DGridPythonic(std::string& targetdir, std::string& filename, Float3DArray& array, double& voxelSize, double& background) {

            std::ofstream file(targetdir + "/" + filename, std::ios::binary);
            if (!file) {
                std::cerr << "Error opening file for writing!" << std::endl;
                return;
            }

            size_t dim1 = array.size();
            size_t dim2 = array[0].size();
            size_t dim3 = array[0][0].size();

            // Write metadata: dimension (as double) and sizes
            file.write(reinterpret_cast<const char*>(&voxelSize), sizeof(double));
            file.write(reinterpret_cast<const char*>(&background), sizeof(double));

            file.write(reinterpret_cast<const char*>(&dim1), sizeof(size_t));
            file.write(reinterpret_cast<const char*>(&dim2), sizeof(size_t));
            file.write(reinterpret_cast<const char*>(&dim3), sizeof(size_t));

            // Write array data
            for (const auto& mat : array) {
                for (const auto& row : mat) {
                    file.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(float));
                }
            }

            file.close();
            std::cout << "Data saved to " << filename << std::endl;

        }

        void clean_obj_file(const std::string& in_path, const std::string& out_path) {
            std::ifstream in(in_path);
            std::ofstream out(out_path);

            std::string line;
            while (std::getline(in, line)) {
                if (line.size() < 2 || line.substr(0, 2) != "vc") {
                    out << line << '\n';
                }
            }
        }
        // Helper function to check if line starts with any of the given prefixes
        bool startsWithAny(const std::string& line, const std::vector<std::string>& prefixes) {
            for (const auto& prefix : prefixes) {
                if (line.rfind(prefix, 0) == 0) {  // Starts with prefix
                    return true;
                }
            }
            return false;
        }

        void filterObjFile(const std::string& filePath,const std::string& filter_string) {
            std::ifstream inFile(filePath);
            if (!inFile.is_open()) {
                std::cerr << "Error: Cannot open " << filePath << " for reading.\n";
                return;
            }

            std::vector<std::string> lines;
            lines.reserve(10000);  // Reserve some space (optional optimization)

            std::string line;
            while (std::getline(inFile, line)) {
                // Check if the line starts with "vc"
                if (line.rfind(filter_string, 0) != 0) {
                    // rfind returns 0 if "vc" is found at position 0 (start of string).
                    // So != 0 means either not found at start or string shorter than 2 chars.
                    lines.push_back(line);
                }
                // If line starts with "vc", do nothing (skip it).
            }

            inFile.close();  // Done reading the file

            // Open the same file for writing, truncating the existing content
            std::ofstream outFile(filePath, std::ios::trunc);
            if (!outFile.is_open()) {
                std::cerr << "Error: Cannot open " << filePath << " for writing.\n";
                return;
            }

            // Write back all the lines except those starting with "vc"
            for (const std::string& outLine : lines) {
                outFile << outLine << '\n';
            }

            outFile.close();
        }

        void filterObjFile(const std::string& filePath, const std::vector<std::string>& filter_strings) {
            std::ifstream inFile(filePath);
            if (!inFile.is_open()) {
                std::cerr << "Error: Cannot open " << filePath << " for reading.\n";
                return;
            }

            std::vector<std::string> lines;
            lines.reserve(10000);  // Optional optimization

            std::string line;
            while (std::getline(inFile, line)) {
                if (!startsWithAny(line, filter_strings)) {
                    lines.push_back(line);  // Keep the line if it doesn't match any filter
                }
            }

            inFile.close();

            std::ofstream outFile(filePath, std::ios::trunc);
            if (!outFile.is_open()) {
                std::cerr << "Error: Cannot open " << filePath << " for writing.\n";
                return;
            }

            for (const auto& outLine : lines) {
                outFile << outLine << '\n';
            }

            outFile.close();
        }

        std::vector<ABC_Surface> ParseABCyml(std::string& file_name) {

            std::vector<ABC_Surface> surfaces;

            YAML::Node root = YAML::LoadFile(file_name);

            for (const auto& node : root["surfaces"]) {
                ABC_Surface s;

                // Scalars
                s.type = node["type"].as<std::string>();

                // Vectors
                s.face_indices = node["face_indices"].as<std::vector<int>>();
                s.vert_indices = node["vert_indices"].as<std::vector<int>>();

                surfaces.push_back(s);
            }

            return surfaces;
        }

        std::vector<std::vector<std::string>> GetVertexToSurfTypeMapYAML(std::string f_name, int n_vertices) {

            std::vector<std::vector<std::string>> VertexToTypeMap(n_vertices);

            std::vector<ABC_Surface> surfaces = util::ParseABCyml(f_name);

            int write_counter = 0;

            for (const auto& surf : surfaces) {
                std::string surf_type = surf.type;

                for (auto vert_id : surf.vert_indices) {

                    if (vert_id >= 0) { //catch ABC file specific -1 indices 
                        if (vert_id >= VertexToTypeMap.size()) {

                            VertexToTypeMap.resize(vert_id + 1);  // Expand the outer vector to include vert_id
                            std::cout << "Resized to accommodate vertex: " << vert_id << std::endl;
                        }
                        if (VertexToTypeMap[vert_id].size() == 0) {
                            write_counter++;
                        }

                        VertexToTypeMap[vert_id].push_back(surf_type);
                    }
                    else {
                        std::cout << "current " << surf_type << " contains vert_index = -1, vertex ignored" << std::endl;
                    }

                    
                }

            }
            std::cout << "Wrote " << write_counter << " vertices to array" << std::endl;

            return VertexToTypeMap;

        }

        std::vector<std::vector<std::string>> GetFaceToSurfTypeMapYAML(std::string f_name, int n_faces) {

            std::vector<std::vector<std::string>> FaceToTypeMap(n_faces);

            std::vector<ABC_Surface> surfaces = util::ParseABCyml(f_name);

            int write_counter = 0;

            for (const auto& surf : surfaces) {

                std::string surf_type = surf.type;

                for (auto face_id : surf.face_indices) {

                    if (face_id >= 0) { //catch ABC file specific -1 indices 
                        if (face_id >= FaceToTypeMap.size()) {

                            FaceToTypeMap.resize(face_id + 1);  // Expand the outer vector to include face_id
                            std::cout << "Resized to accommodate face: " << face_id << std::endl;
                        }


                        if (FaceToTypeMap[face_id].size() == 0) {
                            write_counter++;
                        }

                        FaceToTypeMap[face_id].push_back(surf_type);
                    }
                    else {
                        std::cout << "current " << surf_type << "contains face_index = -1" << std::endl;
                    }
                    
                }
            }
            std::cout << "Wrote " << write_counter << " faces to array" << std::endl;

            return FaceToTypeMap;

        }

        void saveTypeCountsToBinary(
            const std::unordered_map<std::string, int>& surface_type_counts,
            const std::string& filename)
        {
            std::ofstream ofs(filename, std::ios::binary);
            if (!ofs) {
                throw std::runtime_error("Could not open file for writing: " + filename);
            }

            size_t map_size = surface_type_counts.size();
            ofs.write(reinterpret_cast<const char*>(&map_size), sizeof(map_size));

            for (const auto& [key, value] : surface_type_counts) {
                size_t key_size = key.size();
                ofs.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
                ofs.write(key.data(), key_size);
                ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
            }
        }

    }

    namespace CGALbased {

        std::pair <std::vector<MyVertex>, std::vector<MyFace>> GetVerticesAndFaces(Surface_mesh mesh)
        {
            // --- 1. Collect all vertices into a vector ---
           // 
           // Here we simply loop over each vertex in the mesh and save its 3D point.
            std::vector<Point> vertices;
            for (auto v : mesh.vertices()) {
                vertices.push_back(mesh.point(v));  // Save the 3D coordinate of vertex 'v'
            }

            // --- 2.1 Get the vertex index property map using Boost---
            // 
            // A property map in CGAL is like a lookup table that maps each vertex to extra info—in this case, a unique index.
            // We use the Boost Graph interface to get a built-in vertex index.
            //auto v_index = get(boost::vertex_index, mesh);
            // Now, v_index[v] returns the unique index for vertex 'v'.
            //Pros & Cons
            //Convenience: It’s automatically available and managed.
            //Standardization: It uses the standard Boost Graph property map interface, which is well integrated with CGAL’s mesh data structures.


            // --- 2.2 Get the vertex index property map using manual created Property Map---
            // 
             // --- Create the vertex index property map manually ---
            // Try to get the property map with key "v:idx"

            auto v_index_result = mesh.add_property_map<Surface_mesh::Vertex_index, std::size_t>("v:idx", 0);
            auto& v_index = v_index_result.first;
            if (v_index_result.second) {
                // Only assign if we just created it!
                std::size_t idx = 0;
                for (auto v : mesh.vertices()) {
                    v_index[v] = idx++;
                }
            }

            //Pros & Cons
            //Control: You manually create and assign indices, which can be customized
            //Responsibility: You must initialize it and update it if the mesh is modified.


            // --- 3. Extract face indices ---
            // 
            // For each face, we want a list of indices that refer to the vertices of that face.
            std::vector<std::vector<std::size_t>> faces;
            for (auto f : mesh.faces()) {
                std::vector<std::size_t> face_indices;
                // 'vertices_around_face' iterates over all vertices that make up the face 'f'
                for (auto v : CGAL::vertices_around_face(mesh.halfedge(f), mesh)) {
                    // Use the property map to get the index of the current vertex

                    face_indices.push_back(v_index[v]);
                }
                faces.push_back(face_indices);
            }

            // Now:
            // - 'vertices' is a list of all vertex coordinates.
            // - 'faces' is a list where each element is a vector of vertex indices that form a face.

            //casting vertices and faces from 

            std::vector<MyVertex> meshVertices;

            for (auto p : vertices) {
                MyVertex v;
                v.x = p.x();
                v.y = p.y();
                v.z = p.z();
                meshVertices.push_back(v);
            }

            std::vector<MyFace> meshFaces;

            for (auto f : faces) {
                MyFace mf;
                mf.v0 = f[0];
                mf.v1 = f[1];
                mf.v2 = f[2];
                meshFaces.push_back(mf);
            }

            return{ meshVertices, meshFaces };
        }

        std::vector<double> GetBBoxDimensions(Surface_mesh& mesh) {

            std::vector<double> dimensions;

            CGAL::Bbox_3 bbox = CGAL::Polygon_mesh_processing::bbox(mesh);

            dimensions.push_back(bbox.x_span());
            dimensions.push_back(bbox.y_span());
            dimensions.push_back(bbox.z_span());

            return dimensions;
        }

        std::vector<int> GetBBoxMinMaxIndex(Surface_mesh& mesh) {

            std::vector<int> minmax_index;

            CGAL::Bbox_3 bbox = CGAL::Polygon_mesh_processing::bbox(mesh);

            std::initializer_list<double> values = { bbox.x_span(), bbox.y_span(), bbox.z_span() };

            double min_val = *std::min_element(values.begin(), values.end());
            double max_val = *std::max_element(values.begin(), values.end());

            int min_index = -1;
            int max_index = -1;

            if (min_val == bbox.x_span()) min_index = 0;
            if (min_val == bbox.y_span()) min_index = 1;
            if (min_val == bbox.z_span()) min_index = 2;

            if (max_val == bbox.x_span()) max_index = 0;
            if (max_val == bbox.y_span()) max_index = 1;
            if (max_val == bbox.z_span()) max_index = 2;

            minmax_index.push_back(min_index);
            minmax_index.push_back(max_index);

            return minmax_index;
        }

    }

    namespace OpenVDBbased {

        bool loadSingleFloatGridFromVDB(openvdb::FloatGrid::Ptr& outGrid, const std::string& filename)
        {
            openvdb::io::File file(filename);

            try {
                file.open();
            }
            catch (const openvdb::IoError& e) {
                std::cerr << "Failed to open file: " << e.what() << std::endl;
                return false;
            }

            // Count grids and get name
            int gridCount = 0;
            std::string gridName;
            for (auto nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
                gridName = nameIter.gridName();
                ++gridCount;
            }

            if (gridCount != 1) {
                std::cerr << "File does not contain exactly one grid." << std::endl;
                file.close();
                return false;
            }

            // Read grid as GridBase
            openvdb::GridBase::Ptr baseGrid = file.readGrid(gridName);

            // Try to cast to FloatGrid
            outGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);

            if (!outGrid) {
                std::cerr << "The only grid in the file is not a FloatGrid." << std::endl;
                file.close();
                return false;
            }

            file.close();
            return true;
        }


        float getGridMinActiceValue(openvdb::FloatGrid::Ptr grid) {
            // Get the minimum value among active voxels
            float minValue = std::numeric_limits<float>::max();

            for (auto iter = grid->cbeginValueOn(); iter.test(); ++iter) {
                float val = *iter;
                if (val < minValue) {
                    minValue = val;
                }
            }

            return minValue;
        }

        std::vector<std::vector<float>> CoordListToFloatMatrix(std::vector<openvdb::Coord>& coord_list) {

            std::vector<std::vector<float>> float_matrix;


            for (const auto& coord : coord_list) {
                std::vector<float> temp_float_entry;

                temp_float_entry.push_back(coord.x());
                temp_float_entry.push_back(coord.y());
                temp_float_entry.push_back(coord.z());

                float_matrix.push_back(temp_float_entry);
            }

            return float_matrix;
        }

        openvdb::FloatGrid::Ptr MeshToFloatGrid(
            const std::vector<MyVertex>& meshVertices,
            const std::vector<MyFace>& meshFaces,
            float voxelSize,
            float exteriorBandWidth,
            float interiorBandWidth
        )  // Custom grid dimensions
        {
            // Initialize OpenVDB
            openvdb::initialize();

            // Convert vertices to OpenVDB format
            std::vector<openvdb::Vec3s> points;
            for (const auto& vertex : meshVertices) {
                points.emplace_back(vertex.x, vertex.y, vertex.z);
            }

            // Convert faces to OpenVDB format
            std::vector<openvdb::Vec3I> triangles;
            for (const auto& face : meshFaces) {
                triangles.emplace_back(face.v0, face.v1, face.v2);
            }

            std::vector<openvdb::Vec4I> quads;  // Empty for triangle-only meshes


            // Define transformation with voxel size
            openvdb::math::Transform::Ptr transform = openvdb::math::Transform::createLinearTransform(voxelSize);

            openvdb::FloatGrid::Ptr sdfGrid;

            // Convert mesh to signed distance field
            sdfGrid = openvdb::tools::meshToSignedDistanceField<openvdb::FloatGrid>(
                *transform, points, triangles, quads, exteriorBandWidth, interiorBandWidth);

            return sdfGrid;
        }

        openvdb::FloatGrid::Ptr MeshToFloatGrid(
            const std::vector<MyVertex>& meshVertices,
            const std::vector<MyFace>& meshFaces,
            int voxelDim,
            float exteriorBandWidth,
            float interiorBandWidth
        )  // Custom grid dimensions
        {
            // Initialize OpenVDB
            openvdb::initialize();

            // Convert vertices to OpenVDB format
            std::vector<openvdb::Vec3s> points;
            for (const auto& vertex : meshVertices) {
                points.emplace_back(vertex.x, vertex.y, vertex.z);
            }

            // Convert faces to OpenVDB format
            std::vector<openvdb::Vec3I> triangles;
            for (const auto& face : meshFaces) {
                triangles.emplace_back(face.v0, face.v1, face.v2);
            }

            std::vector<openvdb::Vec4I> quads;  // Empty for triangle-only meshes


            auto MeshDim = Tools::OpenVDBbased::DetermineBoundingBox(points);

            double x_dim = MeshDim[0];
            double y_dim = MeshDim[1];
            double z_dim = MeshDim[2];

            // Compute max dimension of the bounding box
            double maxMeshDim = std::max({ x_dim, y_dim, z_dim });

            // Compute voxel size considering narrow-band expansion
            float totalExpansion = 2.0f * std::max(exteriorBandWidth, interiorBandWidth);
            float effectiveTargetDim = voxelDim - totalExpansion;
            if (effectiveTargetDim <= 0) {
                std::cerr << "Error: targetMaxDim is too small for the chosen narrow-band widths!" << std::endl;
                return nullptr;
            }

            // Compute adjusted voxel size
            float voxelSize = static_cast<float>(maxMeshDim / effectiveTargetDim);


            // Define transformation with voxel size
            openvdb::math::Transform::Ptr transform = openvdb::math::Transform::createLinearTransform(voxelSize);

            // Create an empty grid with a background value
            openvdb::FloatGrid::Ptr sdfGrid;

            // Convert mesh to signed distance field
            sdfGrid = openvdb::tools::meshToSignedDistanceField<openvdb::FloatGrid>(
                *transform, points, triangles, quads, exteriorBandWidth, interiorBandWidth);

            return sdfGrid;
        }

        int ActivateInsideValues(openvdb::FloatGrid::Ptr grid) {

            auto background = grid->tree().background();

            int count = 0;

            for (auto iter = grid->beginValueOff(); iter; ++iter) {
                if (*iter < 0) iter.setActiveState(true);
            }
            return count;
        }

        bool CheckIfGridHasValidInsideVoxel(openvdb::FloatGrid::Ptr grid) {
            int count = 0;

            for (auto iter = grid->beginValueOn(); iter; ++iter) {
                if (*iter < 0) count++;
            }
            return (count > 0) ? true : false;

        }

        int CountActiveValue(openvdb::FloatGrid::Ptr grid) {
            int count = 0;

            for (auto iter = grid->beginValueOn(); iter; ++iter) {
                count++;
            }
            return count;

        }

        std::vector<double> DetermineBoundingBox(std::vector<openvdb::Vec3s> points) {

            openvdb::BBoxd meshBBox;
            for (const auto& p : points) {
                meshBBox.expand(p);
            }

            openvdb::Vec3d bboxMin = meshBBox.min();
            openvdb::Vec3d bboxMax = meshBBox.max();

            openvdb::Vec3d bboxSize = bboxMax - bboxMin;

            return { bboxSize[0], bboxSize[1] , bboxSize[2] };
        }

        Tools::Float3DArray Float3DArrayFromFloatGrid(openvdb::FloatGrid::Ptr FloatGrid) {
            // Get full bounding box of the allocated grid
            openvdb::CoordBBox ActiveBBox;
            FloatGrid->tree().evalActiveVoxelBoundingBox(ActiveBBox);

            int dimX = ActiveBBox.dim().x();
            int dimY = ActiveBBox.dim().y();
            int dimZ = ActiveBBox.dim().z();


            // Create a 3D array initialized with the grid's background value
            std::vector<std::vector<std::vector<float>>> denseArray(
                dimX, std::vector<std::vector<float>>(
                    dimY, std::vector<float>(dimZ, FloatGrid->background())
                )
            );

            // Fill the array with voxel values
            for (int i = 0; i < dimX; ++i) {
                for (int j = 0; j < dimY; ++j) {
                    for (int k = 0; k < dimZ; ++k) {
                        openvdb::Coord coord(ActiveBBox.min().x() + i,
                            ActiveBBox.min().y() + j,
                            ActiveBBox.min().z() + k);
                        denseArray[i][j][k] = FloatGrid->tree().getValue(coord);
                    }
                }
            }

            return denseArray;
        }

        Tools::Float3DArray Float3DArrayFromFloatGrid(openvdb::FloatGrid::Ptr FloatGrid, int exspected_dim) {
            // Get full bounding box of the allocated grid
            openvdb::CoordBBox ActiveBBox;
            FloatGrid->tree().evalActiveVoxelBoundingBox(ActiveBBox);

            auto minCord = ActiveBBox.min();

            //auto dim = std::max({ActiveBBox.dim().x(), ActiveBBox.dim().y(), ActiveBBox.dim().z() });
            auto dim = exspected_dim;

            // Create a 3D array initialized with the grid's background value
            std::vector<std::vector<std::vector<float>>> denseArray(
                dim, std::vector<std::vector<float>>(
                    dim, std::vector<float>(dim, FloatGrid->background())
                )
            );

            // Iterate over grid values and write into Float3DGrid
            for (openvdb::FloatGrid::ValueOnCIter iter = FloatGrid->cbeginValueOn(); iter; ++iter) {

                openvdb::Coord Coord = iter.getCoord();
                float value = *iter;

                int x = Coord.x() - minCord.x();
                int y = Coord.y() - minCord.y();
                int z = Coord.z() - minCord.z();

                if (x < dim && y < dim && z < dim) {
                    denseArray[x][y][z] = value;
                }
                else {
                    std::cout << "unable to write Activevoxel to Array -> out off bounds" << std::endl;
                }


            }
            return denseArray;
        }

        openvdb::FloatGrid::Ptr FloatGridFromFloat3DArray(Tools::Float3DArray floatArray) {

            int sizeX = floatArray.size();
            int sizeY = (sizeX > 0) ? floatArray[0].size() : 0;
            int sizeZ = (sizeY > 0) ? floatArray[0][0].size() : 0;

            float background = std::max(floatArray[0][0][0], floatArray[sizeX - 1][sizeY - 1][sizeZ - 1]);

            openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create(background);

            openvdb::FloatGrid::Accessor accessor = grid->getAccessor();

            for (int i = 0; i < sizeX; i++) {
                for (int j = 0; j < sizeX; j++) {
                    for (int k = 0; k < sizeX; k++) {
                        accessor.setValue(openvdb::Coord(i, j, k), floatArray[i][j][k]);
                    }
                }
            }

            return grid;
        }

        void RemapFloat3DArray(Float3DArray& array, LinearSDFMap& linear_map) {

            int sizeX = array.size();
            int sizeY = (sizeX > 0) ? array[0].size() : 0;
            int sizeZ = (sizeY > 0) ? array[0][0].size() : 0;

            for (int i = 0; i < sizeX; i++) {
                for (int j = 0; j < sizeX; j++) {
                    for (int k = 0; k < sizeX; k++) {
                        array[i][j][k] = linear_map.mapping(array[i][j][k]);
                    }
                }
            }

        }

        void RemapFloat3DArray(Float3DArray& array, LinearSDFMap& linear_map, float background) {
            int sizeX = array.size();
            int sizeY = (sizeX > 0) ? array[0].size() : 0;
            int sizeZ = (sizeY > 0) ? array[0][0].size() : 0;

            float minMappedValue = linear_map.mapping(-background);
            float maxMappedValue = linear_map.mapping(background);

            for (int i = 0; i < sizeX; i++) {
                for (int j = 0; j < sizeY; j++) {
                    for (int k = 0; k < sizeZ; k++) {
                        float val = array[i][j][k];

                        if (val <= -background) {
                            array[i][j][k] = minMappedValue;

                        }
                        else if (val >= background) {
                            array[i][j][k] = maxMappedValue;

                        }
                        else {
                            array[i][j][k] = linear_map.mapping(val);
                        }
                    }
                }
            }
        }

        void GridAddWaveFunction(openvdb::FloatGrid::Ptr Floatgrid, float amp, float n_period, float disp, float direction[3]) {
            float bckgrd = Floatgrid->background();

            // Get the bounding box for active voxels.
            openvdb::CoordBBox bbox = Floatgrid->evalActiveVoxelBoundingBox();
            openvdb::Coord minCoord = bbox.min();
            openvdb::Coord maxCoord = bbox.max();

            int dimX = maxCoord.x() - minCoord.x() + 1;
            int dimY = maxCoord.y() - minCoord.y() + 1;
            int dimZ = maxCoord.z() - minCoord.z() + 1;

            float T = dimZ / n_period;

            float omega = TWO_PI / T;

            auto wavefunc = [amp, omega, disp, bckgrd](float x) {return  amp * std::sin(omega * x) + disp; };

            auto scale = Floatgrid->transform().voxelSize()[0];

            for (int i = 0; i < dimX; ++i) {

                for (int j = 0; j < dimY; ++j) {
                    for (int k = 0; k < dimZ; ++k) {
                        openvdb::Coord coord(minCoord.x() + i, minCoord.y() + j, minCoord.z() + k);
                        float val = Floatgrid->tree().getValue(coord);
                        //Floatgrid->tree().setValue(coord, val + scale*(direction[0]*wavefunc(i) + direction[1]*wavefunc(j) + direction[2]*wavefunc(k)));
                        //Floatgrid->tree().setValue(coord, val + scale * std::max({wavefunc(i), wavefunc(j), wavefunc(k)}));
                        Floatgrid->tree().setValue(coord, val + scale * std::max({ direction[0] * wavefunc(i), direction[1] * wavefunc(j), direction[2] * wavefunc(k) }));

                    }
                }
            }


        }

        float triangularWave(float x) {
            return 2.0f * fabsf(x - floorf(x + 0.5f));
        }

        void applyWaveDeformation(openvdb::FloatGrid::Ptr sdfGrid, float amplitude, float frequency, WaveType waveType) {
            using namespace openvdb;

            // Compute the gradient (surface normal)
            auto gradientGrid = tools::gradient(*sdfGrid);

            auto scale = sdfGrid->transform().voxelSize()[0];

            // Iterate over active voxels
            for (auto iter = sdfGrid->beginValueOn(); iter; ++iter) {
                Coord voxelCoord = iter.getCoord();

                // Convert voxel coordinates to floating-point position
                Vec3f position(static_cast<float>(voxelCoord.x()),
                    static_cast<float>(voxelCoord.y()),
                    static_cast<float>(voxelCoord.z()));

                // Get normal at voxel
                Vec3f normal = gradientGrid->tree().getValue(voxelCoord);

                // Normalize the normal
                if (normal.lengthSqr() > 0.0f) {
                    normal.normalize();

                    // Project position onto normal
                    float projection = position.dot(normal);

                    // Compute displacement based on wave type
                    float waveOffset;
                    float waveInput = frequency * projection * 2.0f * MY_PI;

                    if (waveType == SINE_WAVE) {
                        waveOffset = amplitude * scale * std::sin(waveInput);
                    }
                    else if (waveType == TRIANGLE_WAVE) {
                        waveOffset = amplitude * scale * triangularWave(waveInput);
                    }

                    // Apply perturbation
                    iter.setValue(iter.getValue() + waveOffset);
                }
            }
        }

        openvdb::FloatGrid::Ptr resizeToEvenGrid(openvdb::FloatGrid::Ptr unevenGrid,
            int targetDimX, int targetDimY, int targetDimZ) {

            // Get the bounding box of the existing uneven grid
            openvdb::CoordBBox bbox = unevenGrid->evalActiveVoxelBoundingBox();
            openvdb::Coord minCoord = bbox.min();
            openvdb::Coord maxCoord = bbox.max();

            // Compute the current dimensions of the uneven grid
            int dimX = maxCoord.x() - minCoord.x() + 1;
            int dimY = maxCoord.y() - minCoord.y() + 1;
            int dimZ = maxCoord.z() - minCoord.z() + 1;

            // Validate if the target dimensions are large enough
            if (targetDimX < dimX || targetDimY < dimY || targetDimZ < dimZ) {
                std::cerr << "Error: Target dimensions are too small to fit the uneven grid!" << std::endl;
                return nullptr;
            }

            // Create a new even-sized FloatGrid with the same background value
            openvdb::FloatGrid::Ptr evenGrid = openvdb::FloatGrid::create(unevenGrid->background());
            evenGrid->setTransform(unevenGrid->transform().copy()); // Maintain the same transform

            // Compute the offsets to center the uneven grid inside the new grid
            int offsetX = (targetDimX - dimX) / 2;
            int offsetY = (targetDimY - dimY) / 2;
            int offsetZ = (targetDimZ - dimZ) / 2;

            // Iterate over the old grid and copy values into the new grid
            for (openvdb::FloatGrid::ValueOnCIter iter = unevenGrid->cbeginValueOn(); iter; ++iter) {
                openvdb::Coord oldCoord = iter.getCoord();
                float value = *iter;

                // Compute the new coordinates in the even grid
                openvdb::Coord newCoord = oldCoord + openvdb::Coord(offsetX, offsetY, offsetZ);

                // Set the value in the new grid
                evenGrid->tree().setValue(newCoord, value);
            }

            bbox = evenGrid->evalActiveVoxelBoundingBox();
            minCoord = bbox.min();
            maxCoord = bbox.max();

            return evenGrid;
        }

        std::vector<std::vector<float>> TransformWorldPointsToIndexFloatArray(openvdb::FloatGrid::Ptr& grid, std::vector<MyVertex>& vertex_list) {

            std::vector<std::vector<float>> transformed_point_array;

            for (auto& vert : vertex_list) {
                std::vector<float> transformed_point;

                openvdb::Vec3d word_point = openvdb::Vec3d(vert.x, vert.y, vert.z);
                openvdb::Coord index_coord = grid->transform().worldToIndexCellCentered(word_point);

                transformed_point.push_back(index_coord.x());
                transformed_point.push_back(index_coord.y());
                transformed_point.push_back(index_coord.z());

                transformed_point_array.push_back(transformed_point);
            }

            return transformed_point_array;

        }

    }
    namespace Functions {
        float read_voxel_size(const std::string& dat_file) {
            std::ifstream infile(dat_file);
            std::string line;
            while (std::getline(infile, line)) {
                if (line.find("voxel_size:") != std::string::npos) {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos) {
                        return std::stof(line.substr(pos + 1));
                    }
                }
            }
            // Default value or error handling
            return 1.0f;
        }

        openvdb::Vec3f read_bottom_coord(const std::string& dat_file) {
            std::ifstream infile(dat_file);
            std::string line;
            bool in_origin_container = false;
            float min_x = std::numeric_limits<float>::max();
            float min_y = std::numeric_limits<float>::max();
            float min_z = std::numeric_limits<float>::max();

            while (std::getline(infile, line)) {
                if (line.find("[ORIGIN_CONTAINER]") != std::string::npos) {
                    in_origin_container = true;
                    continue;
                }
                if (in_origin_container) {
                    if (line.find("[END_ORIGIN_CONTAINER]") != std::string::npos) break;
                    std::istringstream iss(line);
                    float x, y, z;
                    if (iss >> x >> y >> z) {
                        if (x < min_x) min_x = x;
                        if (y < min_y) min_y = y;
                        if (z < min_z) min_z = z;
                    }
                }
            }
            return openvdb::Vec3f(min_x, min_y, min_z);
        }


        std::vector<float> read_grid_bin(const std::string& filename, int dimX, int dimY, int dimZ) {
            std::vector<float> data(dimX * dimY * dimZ);
            std::ifstream bin(filename, std::ios::binary);
            bin.read(reinterpret_cast<char*>(data.data()), data.size() * sizeof(float));
            return data;
        }

        void save_grid_to_vdb(const std::vector<float>& grid_data,
            const int dims[3],
            const openvdb::Vec3f& bottom_coord,
            float voxel_size,
            const std::string& filename)
        {
            int dimX = dims[0], dimY = dims[1], dimZ = dims[2];

            // Create VDB grid
            openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create(0.0f);
            grid->setName("prediction");

            // Set the grid's transform: linear transform with offset for bottom_coord
            openvdb::math::Transform::Ptr xform = openvdb::math::Transform::createLinearTransform(voxel_size);
            xform->postTranslate(bottom_coord * voxel_size);
            grid->setTransform(xform);
           

            // Fill the grid
            openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
            for (int z = 0; z < dimZ; ++z) {
                for (int y = 0; y < dimY; ++y) {
                    for (int x = 0; x < dimX; ++x) {
                        size_t idx = x + y * dimX + z * dimX * dimY;
                        float value = grid_data[idx];
                        if (value != 0.0f) {
                            accessor.setValue(openvdb::Coord(x, y, z), value);
                        }
                    }
                }
            }
            // Write to file
            openvdb::io::File vdbfile(filename);
            openvdb::GridPtrVec grids;
            grids.push_back(grid);
            vdbfile.write(grids);
            vdbfile.close();
        }

        openvdb::FloatGrid::Ptr readFirstFloatGrid(const std::string& filename)
        {
            openvdb::initialize();

            openvdb::io::File file(filename);
            try {
                file.open();

                // Find the first grid name
                for (auto nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
                    std::string gridName = nameIter.gridName();
                    // Read the grid by name
                    openvdb::GridBase::Ptr baseGrid = file.readGrid(gridName);
                    // Try casting to FloatGrid
                    openvdb::FloatGrid::Ptr floatGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);
                    if (floatGrid) {
                        file.close();
                        return floatGrid;
                    }
                    // If not FloatGrid, skip or try next
                }

                file.close();
                std::cerr << "No FloatGrid found in file: " << filename << std::endl;
                return nullptr;
            }
            catch (const openvdb::IoError& e) {
                std::cerr << "OpenVDB IO error: " << e.what() << std::endl;
                return nullptr;
            }
        }

        void read_grid_shape(const std::string& shape_file, int dims[3]) {
            std::ifstream infile(shape_file);
            if (!infile) throw std::runtime_error("Could not open shape file!");
            infile >> dims[0] >> dims[1] >> dims[2];
        }


    }//namespace Functions

    namespace Macros {

        void export_bin_to_vdb(const std::string& dat_file,
            const std::string& bin_file,
            const std::string& shape_file,
            const std::string& out_vdb_file)
        {
           

            // 1. Read meta info
            openvdb::Vec3f bottom_coord = Functions::read_bottom_coord(dat_file);
            float voxel_size = Functions::read_voxel_size(dat_file);

            // 2. Read shape
            int dims[3];
            Functions::read_grid_shape(shape_file, dims);

            // 3. Read grid data
            std::vector<float> grid_data = Functions::read_grid_bin(bin_file, dims[0], dims[1], dims[2]);

            // 4. Write to VDB
            openvdb::initialize();
            Functions::save_grid_to_vdb(grid_data, dims, bottom_coord, voxel_size, out_vdb_file);

            std::cout << "Done! Saved to " << out_vdb_file << std::endl;
        }

        void test_grid_vdb(fs::path filename)
        {
            const std::string target = filename.generic_string();
            auto segments = cppIO::Functions::read_segments_from_binary<float>(target);
  

        }
    
    }//namespace Macros

   
}


