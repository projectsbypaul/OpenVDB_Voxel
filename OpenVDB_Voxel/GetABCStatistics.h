#pragma once
#include "Tools.h"
#include <memory>
#include <openvdb/tools/Composite.h>
#include "MeshDataHandling.h"
#include "Scripts.h"

#include <CGAL/Bbox_3.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

#include <filesystem>
#include "DatasetToVoxel.h"

#include "DL_Preprocessing.h"
#include <string>
#include <vector>
#include <iostream>

#include <fstream>
#include <filesystem>

namespace Scripts {
	int AnalyzeABCDir(fs::path rootDir, std::string extension, int& k_size, int& n_min_kernel, int& band_width, int& padding, unsigned int max_threads=10);
	std::vector<fs::path> GetFilesOfType(const fs::path& rootDir, const std::string& extension);
}