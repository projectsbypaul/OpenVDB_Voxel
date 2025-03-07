#include "Tools.h"
#include "MyTypes.h"

namespace Tools {

    namespace util {

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

            auto v_index_opt = mesh.property_map<Surface_mesh::Vertex_index, std::size_t>("v:idx");
            // If it doesn't exist, create it and assign unique indices.
            if (!v_index_opt.has_value()) {
                auto result = mesh.add_property_map<Surface_mesh::Vertex_index, std::size_t>("v:idx", 0);
                auto v_index = result.first;
                std::size_t idx = 0;
                for (auto v : mesh.vertices()) {
                    v_index[v] = idx++;
                }
            }
            // Retrieve the (now ensured) property map.
            auto v_index = *mesh.property_map<Surface_mesh::Vertex_index, std::size_t>("v:idx");

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
    }

    namespace OpenVDBbased {

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
                if (*iter == -background) iter.setActiveState(true);
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

            return {bboxSize[0], bboxSize[1] , bboxSize[2]};
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

                denseArray[Coord.x()-minCord.x()][Coord.y() - minCord.y()][Coord.z() - minCord.z()] = value;

            }

          
            

            return denseArray;
        }

        openvdb::FloatGrid::Ptr FloatGridFromFloat3DArray(Tools::Float3DArray floatArray) {

            int sizeX = floatArray.size();
            int sizeY = (sizeX > 0) ? floatArray[0].size() : 0;
            int sizeZ = (sizeY > 0) ? floatArray[0][0].size() : 0;

            float background = std::max(floatArray[0][0][0], floatArray[sizeX-1][sizeY-1][sizeZ-1]);

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

        void RemapFloatGrid(openvdb::FloatGrid::Ptr grid, LinearSDFMap& linear_map) {

            for (auto iter = grid->beginValueOff(); iter; ++iter) {
                float current = iter.getValue();
                iter.setValue(linear_map.mapping(current));
                
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
                    float waveInput = frequency * projection * 2.0f * M_PI;

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

    }
}


