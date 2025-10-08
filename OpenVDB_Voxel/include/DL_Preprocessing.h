#pragma once

#pragma region common 
#include "Tools.h"
#include "LabelTemplates.h"
#pragma endregion 

#pragma region OpenVDB 

#pragma endregion 

#pragma region CGAL 

#pragma endregion 

namespace DLPP {

	namespace label_func {
		Tools::Int3DArray label_by_template(const Tools::Int3DArray& segment, const Tools::MappingTable& face_to_type, LabelTemplates::LabelTemplate& label_template);
	}

	namespace util {
		/// <summary>
		/// Calcutlates the required ammount of cropping steps based on input cropping parameters 
		/// during the cropping procedure 
		/// </summary>
		/// <param name="kernel_size">: Dim of the cropping kernel</param>
		/// <param name="n_voxel_dim">: voxelcount of actice values in dimension in which want to calculate the minal ammount of cropping steps</param>
		/// <param name="padding">: overlapp of cropping kernels</param>
		/// <returns>minimal ammount of cropping stept in trages dim - return value is always round up</returns>
		int calculateMinCroppingStep(int n_voxel_dim, int kernel_size, int padding);

		Tools::Int3DArray get_nearest_face_index(Tools::FloatMatrix& face_to_gridindex, Tools::Float3DArray& segment, std::vector<float> origin, float max_r_surface);

		std::vector <Tools::FloatMatrix> bin_gridcoord_by_origin(const Tools::FloatMatrix& gridindex, const Tools::FloatMatrix& origins, int kernel_size);

	}

	namespace OpenVDBbased {
		Tools::Float3DArray KernelCropFloatGridFromCoord(openvdb::FloatGrid::Ptr& grid, openvdb::Coord& origin, int& kernel_size);

		std::vector<openvdb::Coord> calculateCroppingOrigins(openvdb::FloatGrid::Ptr& grid, int& kernel_size, int& padding, bool surface_only = false);
	}

	namespace CGALbased{
		/// <summary>
		/// Calculates a recommended voxelsize which ensures that even the min dimension of the mesh can fit the minmal ammount of kernels 
		/// during the cropping procedure 
		/// </summary>
		/// <param name="kernel_size">: Dim of the cropping kernel</param>
		/// <param name="min_n_kernel">: mininmal ammount of cropping kernels in the smallest dimension</param>
		/// <param name="exterior_bandwidth">: exterior bandwidth for SDF calculation -> ensures cropping includes narrowband</param>
		/// <param name="padding">: overlapp of cropping kernels</param>
		/// <param name="mesh">: The CGAL Polygomesh which is supposed to be processed</param>
		/// <returns>Recommended voxelsize to fit the cropping kernel parameters</returns>
		float calculateRecommendeVoxelsize(int& kernel_size, int& min_n_kernel, int& exterior_bandwidth, int& padding, Surface_mesh& mesh);

	}
}