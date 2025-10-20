#include "../include/DL_Preprocessing.h"


namespace DLPP {

    namespace label_func {
        std::pair<Tools::Int3DArray, std::vector<int>>
            label_by_template_count(
                const Tools::Int3DArray& segment,
                const Tools::MappingTable& face_to_type,
                LabelTemplates::LabelTemplate& label_template
            ) {
            int dim_x = segment.size();
            int dim_y = segment[0].size();
            int dim_z = segment[0][0].size();

            Tools::Int3DArray labeled_segment(dim_x, std::vector<std::vector<int>>(dim_y, std::vector<int>(dim_z)));

            std::unordered_map<std::string, int> class_to_index = label_template.get_class_to_index();

            std::vector<int> class_count(label_template.class_count, 0);

            for (int x = 0; x < dim_x; x++) {
                for (int y = 0; y < dim_y; y++) {
                    for (int z = 0; z < dim_z; z++) {
                        const int& face_index = segment[x][y][z];
                        if (face_index > 0) { // surface
                            const std::vector<std::string>& surf_types = face_to_type.at(face_index);
                            int idx = class_to_index[surf_types[0]];
                            labeled_segment[x][y][z] = idx;
                            class_count[idx]++;
                        }
                        else if (face_index == -1) { // inside
                            int idx = class_to_index["Inside"];
                            labeled_segment[x][y][z] = idx;
                            class_count[idx]++;
                        }
                        else if (face_index == -2) { // outside
                            int idx = class_to_index["Outside"];
                            labeled_segment[x][y][z] = idx;
                            class_count[idx]++;
                        }
                    }
                }
            }

            return { labeled_segment, class_count };
        }

        Tools::Int3DArray label_by_template(const Tools::Int3DArray& segment, const Tools::MappingTable& face_to_type, LabelTemplates::LabelTemplate& label_template) {
            int dim_x = segment.size();
            int dim_y = segment[0].size();
            int dim_z = segment[0][0].size();

            Tools::Int3DArray labeled_segment(dim_x, std::vector<std::vector<int>>(dim_y, std::vector<int>(dim_z)));

            std::unordered_map<std::string, int> class_to_index = label_template.get_class_to_index();
            std::unordered_map<int, std::string> index_to_class = label_template.get_index_to_class();

            std::vector<int> class_count(label_template.class_count);

            for (int x = 0; x < dim_x; x++) {
                for (int y = 0; y < dim_y; y++) {
                    for (int z = 0; z < dim_z; z++) {
                        const int& face_index = segment[x][y][z];
                        if (face_index > 0) {//surface
                            const std::vector<std::string>& surf_types = face_to_type[face_index];
                            labeled_segment[x][y][z] = class_to_index[surf_types[0]];
                            class_count[class_to_index[surf_types[0]]]++;
                        }
                        else if (face_index == -1) {//inside
                            labeled_segment[x][y][z] = class_to_index["Inside"];
                            class_count[class_to_index["Inside"]]++;
                        }
                        else if (face_index == -2) {//outside
                            labeled_segment[x][y][z] = class_to_index["Outside"];
                            class_count[class_to_index["Outside"]]++;
                        }
                    }

                }
            }

            return labeled_segment;
        }
    }


	namespace util {

        bool nearly_equal(float a, float b, float eps = 1e-6) {
            float delta = std::fabs(a - b);
            return delta < eps;
        }

        std::vector <Tools::FaceBin> bin_gridcoord_by_origin(
            const Tools::FloatMatrix& gridindex,  // all faces, coords in grid-index space
            const Tools::FloatMatrix& origins,    // per-segment origins in same space
            int kernel_size,                      // segment side length in indices
            int pad_indices                       // extra padding on each side (in indices){
        ){
            const int n_origins = static_cast<int>(origins.size());
            std::vector<Tools::FaceBin> bins(n_origins);

            for (int g = 0; g < static_cast<int>(gridindex.size()); ++g) {
                const auto& p = gridindex[g]; // p[0], p[1], p[2] in grid-index coords

                for (int i = 0; i < n_origins; ++i) {
                    const auto& o = origins[i];

                    const float x0 = o[0] - pad_indices;
                    const float y0 = o[1] - pad_indices;
                    const float z0 = o[2] - pad_indices;

                    const float x1 = o[0] + kernel_size + pad_indices;
                    const float y1 = o[1] + kernel_size + pad_indices;
                    const float z1 = o[2] + kernel_size + pad_indices;

                    const bool in_bounds =
                        (x0 <= p[0]) && (p[0] < x1) &&
                        (y0 <= p[1]) && (p[1] < y1) &&
                        (z0 <= p[2]) && (p[2] < z1);

                    if (in_bounds) {
                        bins[i].coords.push_back(p);
                        bins[i].to_global.push_back(g); // keep the global id!
                    }
                }
            }
            return bins;
        }

        Tools::Int3DArray get_nearest_face_index_binned(
            const Tools::FloatMatrix& face_coords_bin,   // local
            const std::vector<int>& to_global,           // local->global
            const Tools::Float3DArray& segment,          // normalized SDF [-1,+1]
            const std::array<float, 3>& origin,           // grid-index offset for this segment
            float voxel_size, float background, float max_r_surface)
        {
            const int X = segment.size(), Y = segment[0].size(), Z = segment[0][0].size();
            Tools::Int3DArray out(X, std::vector<std::vector<int>>(Y, std::vector<int>(Z, -3)));

            for (int x = 0; x < X; ++x) for (int y = 0; y < Y; ++y) for (int z = 0; z < Z; ++z) {
                const float sdf_phys = segment[x][y][z] * background;
                if (sdf_phys <= -max_r_surface) { out[x][y][z] = -1; continue; }
                if (sdf_phys >= max_r_surface) { out[x][y][z] = -2; continue; }

                const float px = x + origin[0], py = y + origin[1], pz = z + origin[2];

                float best_d2 = std::numeric_limits<float>::infinity();
                int best_local = -3;

                for (size_t i = 0; i < face_coords_bin.size(); ++i) {
                    const auto& f = face_coords_bin[i]; // grid-index coords
                    const float dx = (px - f[0]) * voxel_size;
                    const float dy = (py - f[1]) * voxel_size;
                    const float dz = (pz - f[2]) * voxel_size;
                    const float d2 = dx * dx + dy * dy + dz * dz;
                    if (d2 < best_d2) { best_d2 = d2; best_local = static_cast<int>(i); }
                }

                out[x][y][z] = (best_local >= 0) ? to_global[best_local] : -3; // **GLOBAL ID**
            }
            return out;
        }

        Tools::Int3DArray get_nearest_face_index(
            const Tools::FloatMatrix& face_to_gridindex,   // Nx3, in grid-index coords
            const Tools::Float3DArray& segment,            // normalized SDF in [-1,+1]
            const std::array<float, 3>& origin,             // grid-index offset
            float voxel_size,                               // isotropic spacing (world units per index)
            float back_ground,                              // the +/- value used for normalization
            float max_r_surface)                            // band half-width in *physical* SDF units
        {
            const int dim_x = segment.size();
            const int dim_y = segment[0].size();
            const int dim_z = segment[0][0].size();

            // -1 = inside, -2 = outside, -3 = surface but no nearest face found
            Tools::Int3DArray nearest_face_index(
                dim_x, std::vector<std::vector<int>>(dim_y, std::vector<int>(dim_z, -3)));

            for (int x = 0; x < dim_x; ++x) {
                for (int y = 0; y < dim_y; ++y) {
                    for (int z = 0; z < dim_z; ++z) {

                        // Convert normalized SDF back to physical units to compare with max_r_surface
                        const float sdf_phys = segment[x][y][z] * back_ground;

                        if (sdf_phys <= -max_r_surface) {
                            nearest_face_index[x][y][z] = -1;   // inside
                            continue;
                        }
                        if (sdf_phys >= max_r_surface) {
                            nearest_face_index[x][y][z] = -2;   // outside
                            continue;
                        }

                        // Surface voxel: find nearest face
                        const float px = x + origin[0];
                        const float py = y + origin[1];
                        const float pz = z + origin[2];

                        float best_d2 = std::numeric_limits<float>::infinity();
                        int best_idx = -3;

                        for (size_t i = 0; i < face_to_gridindex.size(); ++i) {
                            const auto& f = face_to_gridindex[i]; // f[0], f[1], f[2] in grid-index coords
                            const float dx = (px - f[0]) * voxel_size;
                            const float dy = (py - f[1]) * voxel_size;
                            const float dz = (pz - f[2]) * voxel_size;
                            const float d2 = dx * dx + dy * dy + dz * dz;
                            if (d2 < best_d2) {
                                best_d2 = d2;
                                best_idx = static_cast<int>(i);
                            }
                        }

                        nearest_face_index[x][y][z] = best_idx;
                    }
                }
            }

            return nearest_face_index;
        }

        int calculateMinCroppingStep(int n_voxel_dim, int kernel_size, int padding) {

            double step_double = static_cast<double>(n_voxel_dim + padding) / (kernel_size - padding);

            int min_cropping_steps = static_cast<int>(std::ceil(step_double));

            return min_cropping_steps;
        }


	}

	namespace OpenVDBbased {

        Tools::Float3DArray KernelCropFloatGridFromCoord(openvdb::FloatGrid::Ptr& grid, openvdb::Coord& origin, int& kernel_size) {


            // Create a 3D array initialized with the grid's background value
            std::vector<std::vector<std::vector<float>>> denseArray(
                kernel_size, std::vector<std::vector<float>>(
                    kernel_size, std::vector<float>(kernel_size, grid->background())
                )
            );

            openvdb::Coord kernel_max_point(origin.x() + kernel_size - 1, origin.y() + kernel_size - 1, origin.z() + kernel_size - 1);

            openvdb::CoordBBox clip_box = openvdb::CoordBBox(origin, kernel_max_point);

            openvdb::FloatGrid::Ptr clipped = grid->deepCopy();

            clipped->clip(clip_box);

            // Iterate over grid values and write into Float3DGrid
            for (openvdb::FloatGrid::ValueOnCIter iter = clipped->cbeginValueOn(); iter; ++iter) {

                openvdb::Coord Coord = iter.getCoord();
                float value = *iter;

                int x = Coord.x() - origin.x();
                int y = Coord.y() - origin.y();
                int z = Coord.z() - origin.z();

                denseArray[x][y][z] = clipped->getAccessor().getValue(Coord);
            }

            return denseArray;

        }

        std::vector<openvdb::Coord> calculateCroppingOrigins(openvdb::FloatGrid::Ptr& grid, int& kernel_size, int& padding, bool surface_only) {
            
            std::vector<openvdb::Coord> crop_origins;

            openvdb::CoordBBox ActiveBBox;
            auto grid_accessor = grid->getAccessor();
            grid->tree().evalActiveVoxelBoundingBox(ActiveBBox);

            openvdb::Coord origin = ActiveBBox.min();

            int x_steps = util::calculateMinCroppingStep((int)ActiveBBox.dim().x(), kernel_size, padding);
            int y_steps = util::calculateMinCroppingStep((int)ActiveBBox.dim().y(), kernel_size, padding);
            int z_steps = util::calculateMinCroppingStep((int)ActiveBBox.dim().z(), kernel_size, padding);

            for (int i = 0; i < x_steps; i++) {
                for (int j = 0; j < y_steps; j++) {
                    for (int k = 0; k < z_steps; k++) {
                        int x_comp = origin.x() + i * kernel_size - (i + 1) * padding;
                        int y_comp = origin.y() + j * kernel_size - (j + 1) * padding;
                        int z_comp = origin.z() + k * kernel_size - (k + 1) * padding;
                        crop_origins.emplace_back(x_comp, y_comp, z_comp);
                    }
                }
            }

            if (!surface_only) return crop_origins;

            //Enhanced surface-only filter: corners + center + inner cube (0.25..0.75)
            std::vector<openvdb::Coord> surface_only_origins;
            // Trilinear sampler in index space
            openvdb::tools::GridSampler<openvdb::FloatGrid, openvdb::tools::BoxSampler> sampler(*grid);
            constexpr double EPS = 1e-6; // treat very small magnitudes as zero

            for (const openvdb::Coord& org : crop_origins) {
                const int x0 = org.x();
                const int y0 = org.y();
                const int z0 = org.z();

                // Build sample positions (INDEX space)
                std::vector<openvdb::Vec3d> samples;
                samples.reserve(8 + 1 + 8); //outer cube + center + inner cube

                //outer cube corners spanning [0, 1]^3
                for (int dx = 0; dx < 2; ++dx)
                    for (int dy = 0; dy < 2; ++dy)
                        for (int dz = 0; dz < 2; ++dz) {
                            samples.emplace_back(
                                x0 + dx * kernel_size,
                                y0 + dy * kernel_size,
                                z0 + dz * kernel_size
                            );
                        }

                // center (0.5, 0.5, 0.5)
                samples.emplace_back(x0 + 0.5 * kernel_size, y0 + 0.5 * kernel_size, z0 + 0.5 * kernel_size);

                // inner cube corners spanning [0.25, 0.75]^3
                for (int dx = 0; dx < 2; ++dx)
                    for (int dy = 0; dy < 2; ++dy)
                        for (int dz = 0; dz < 2; ++dz) {
                            samples.emplace_back(
                                x0 + (0.25 + 0.5 * dx) * kernel_size,
                                y0 + (0.25 + 0.5 * dy) * kernel_size,
                                z0 + (0.25 + 0.5 * dz) * kernel_size
                            );
                        }

                // Count sign categories
                int c_pos = 0, c_neg = 0, c_zero = 0;
                for (const auto& p : samples) {
                    const double v = sampler.isSample(p); // INDEX space sampling
                    if (v > EPS)      ++c_pos;
                    else if (v < -EPS)++c_neg;
                    else              ++c_zero;
                }

                int kinds = 0;
                if (c_pos > 0) ++kinds;
                if (c_neg > 0) ++kinds;
                if (c_zero > 0) ++kinds;

                // If more than one category is present, the crop intersects the surface
                if (kinds > 1) surface_only_origins.push_back(org);
            }
            //

            return surface_only_origins;
        }

	}

	namespace CGALbased {

		float calculateRecommendeVoxelsize(int& kernel_size, int& min_n_kernel, int& exterior_bandwidth, int& padding, Surface_mesh& mesh) {

			std::vector<double> bbox_dims = Tools::CGALbased::GetBBoxDimensions(mesh);

			std::vector<int> min_max_index = Tools::CGALbased::GetBBoxMinMaxIndex(mesh);

			//float recommended_voxelsize = bbox_dims[min_max_index[0]] / (kernel_size * min_n_kernel - 2 * exterior_bandwidth - (min_n_kernel - 1) * padding - 2 * padding);

            float recommended_voxelsize = bbox_dims[min_max_index[0]] / (kernel_size * min_n_kernel) ;

			return recommended_voxelsize;
		}


	}
}