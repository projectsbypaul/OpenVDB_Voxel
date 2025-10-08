#include "../include/DL_Preprocessing.h"


namespace DLPP {

    namespace label_func {
        Tools::Int3DArray label_by_template(const Tools::Int3DArray& segment, const Tools::MappingTable& face_to_type, LabelTemplates::LabelTemplate& label_template) {
            int dim_x = segment.size();
            int dim_y = segment[0].size();
            int dim_z = segment[0][0].size();

            Tools::Int3DArray labeled_segment(dim_x, std::vector<std::vector<int>>(dim_y, std::vector<int>(dim_z)));

            std::unordered_map<std::string, int> class_to_index = label_template.get_class_to_index();
            std::unordered_map<int, std::string> index_to_class = label_template.get_index_to_class();

            //std::vector<int> class_count(label_template.class_count);

            for (int x = 0; x < dim_x; x++) {
                for (int y = 0; y < dim_y; y++) {
                    for (int z = 0; z < dim_z; z++) {
                        const int& face_index = segment[x][y][z];
                        if (face_index > 0) {//surface
                            const std::vector<std::string>& surf_types = face_to_type[face_index];
                            labeled_segment[x][y][z] = class_to_index[surf_types[0]];
                            //class_count[class_to_index[surf_types[0]]]++;
                        }
                        else if (face_index == -1) {//inside
                            labeled_segment[x][y][z] = class_to_index["Inside"];
                            //class_count[class_to_index["Inside"]]++;
                        }
                        else if (face_index == -2) {//outside
                            labeled_segment[x][y][z] = class_to_index["Outside"];
                            //class_count[class_to_index["Outside"]]++;
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

        std::vector <Tools::FloatMatrix> bin_gridcoord_by_origin(const Tools::FloatMatrix& gridindex,const Tools::FloatMatrix& origins, int kernel_size) {

            const int n_origins = static_cast<int>(origins.size());

            std::vector <Tools::FloatMatrix> bins(n_origins);

            for (const std::vector<float>& grid_coord : gridindex) {

                for (int i = 0; i < n_origins; i++) {

                    const std::vector<float>& origin = origins[i];
                   
                    bool in_bounds =
                        (origin[0] <= grid_coord[0]) && (grid_coord[0] < (origin[0] + kernel_size)) &&
                        (origin[1] <= grid_coord[1]) && (grid_coord[1] < (origin[1] + kernel_size)) &&
                        (origin[2] <= grid_coord[2]) && (grid_coord[2] < (origin[2] + kernel_size));

                    if (in_bounds) { bins[i].push_back(grid_coord); }


                }
            }

            return bins;
        }

        Tools::Int3DArray get_nearest_face_index(Tools::FloatMatrix& face_to_gridindex, Tools::Float3DArray& segment, std::vector<float> origin, float max_r_surface) {
            int dim_x = segment.size();
            int dim_y = segment[0].size();
            int dim_z = segment[0][0].size();

            Tools::Int3DArray nearest_face_index(dim_x, std::vector<std::vector<int>>(dim_y, std::vector<int>(dim_z)));

            for (int x = 0; x < dim_x; x++) {
                for (int y = 0; y < dim_y; y++) {
                    for (int z = 0; z < dim_z; z++) {

                        if (segment[x][y][z] <= -max_r_surface) { //inside
                            nearest_face_index[x][y][z] = -1;
                        }
                        else if (segment[x][y][z] >= max_r_surface) { //outside
                            nearest_face_index[x][y][z] = -2;
                        }
                        else { //surface
                            float min_r_abs = float(dim_x);
                            int min_index = -1;

                            int rolling_index = 0;
                            for (std::vector<float> face_on_grid : face_to_gridindex) {
                                float delta_x = (x + origin[0]) - face_on_grid[0];
                                float delta_y = (y + origin[1]) - face_on_grid[1];
                                float delta_z = (z + origin[2]) - face_on_grid[2];

                                float r_abs = std::sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
                                if (r_abs < min_r_abs) {
                                    min_r_abs = r_abs;
                                    min_index = rolling_index;
                                }

                                rolling_index++;
                            }
                            nearest_face_index[x][y][z] = min_index;
                        }

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

            openvdb::Coord step_coord;

            for (int i = 0; i < x_steps; i++) {
                for (int j = 0; j < y_steps; j++) {
                    for (int k = 0; k < z_steps; k++) {

                        int x_comp = origin.x() + i * kernel_size - (i + 1) * padding;
                        int y_comp = origin.y() + j * kernel_size - (j + 1) * padding;
                        int z_comp = origin.z() + k * kernel_size - (k + 1) * padding;

                        step_coord = openvdb::Coord(x_comp, y_comp, z_comp);

                        crop_origins.push_back(step_coord);
                    }
                }

            }

            if (surface_only) {

                std::vector<openvdb::Coord> surface_only_origins;

                for (openvdb::Coord origin : crop_origins)
                {
                    int c_positive = 0;
                    int c_negative = 0;
                    int c_zero = 0;

                    std::vector<openvdb::Coord> corners;
                    int x_comp = origin.x();
                    int y_comp = origin.y();
                    int z_comp = origin.z();

                    corners.push_back(openvdb::Coord(x_comp, y_comp, z_comp)); //0 0 0 
                    corners.push_back(openvdb::Coord(x_comp + kernel_size, y_comp + kernel_size, z_comp + kernel_size)); //1 1 1 

                    corners.push_back(openvdb::Coord(x_comp + kernel_size, y_comp, z_comp)); //1 0 0
                    corners.push_back(openvdb::Coord(x_comp + kernel_size, y_comp + kernel_size, z_comp)); //1 1 0
                    corners.push_back(openvdb::Coord(x_comp + kernel_size, y_comp, z_comp + kernel_size)); //1 0 1

                    corners.push_back(openvdb::Coord(x_comp, y_comp + kernel_size, z_comp)); //0 1 0
                    corners.push_back(openvdb::Coord(x_comp, y_comp + kernel_size, z_comp + kernel_size)); //0 1 1 


                    corners.push_back(openvdb::Coord(x_comp, y_comp, z_comp + kernel_size)); // 0 0 1 

                    for (openvdb::Coord point : corners) {
                        float corner_value = grid_accessor.getValue(point);
                        if (corner_value > 0) c_positive++;
                        if (corner_value < 0) c_negative++;
                        if (corner_value == 0) c_zero++;
                    }

                    int counters_greater_zero = 0;
                    if (c_positive > 0) counters_greater_zero++;
                    if (c_negative > 0) counters_greater_zero++;
                    if (c_zero > 0) counters_greater_zero++;


                    if (counters_greater_zero > 1) surface_only_origins.push_back(origin);
                }
                return surface_only_origins;

            }
            else
            {
                return crop_origins;
            }

            

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