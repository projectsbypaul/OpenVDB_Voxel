#include "DL_Preprocessing.h"

namespace DLPP {

	namespace util {
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

            openvdb::Coord kernel_max_point = openvdb::Coord({ origin.x() + kernel_size -1, origin.y() + kernel_size -1, origin.z() + kernel_size-1 });

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

        std::vector<openvdb::Coord> calculateCroppingOrigins(openvdb::FloatGrid::Ptr& grid, int& kernel_size, int& padding) {
            
            std::vector<openvdb::Coord> crop_origins;
            
            openvdb::CoordBBox ActiveBBox;
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

                        step_coord = openvdb::Coord({ x_comp, y_comp, z_comp });

                        crop_origins.push_back(step_coord);
                    }
                }

            }  

            return crop_origins;

        }
	}

	namespace CGALbased {

		float calculateRecommendeVoxelsize(int& kernel_size, int& min_n_kernel, int& exterior_bandwidth, int& padding, Surface_mesh& mesh) {

			std::vector<double> bbox_dims = Tools::CGALbased::GetBBoxDimensions(mesh);

			std::vector<int> min_max_index = Tools::CGALbased::GetBBoxMinMaxIndex(mesh);

			float recommended_voxelsize = bbox_dims[min_max_index[0]] / (kernel_size * min_n_kernel - 2 * exterior_bandwidth - (min_n_kernel - 1) * padding - 2 * padding);

			return recommended_voxelsize;
		}


	}
}