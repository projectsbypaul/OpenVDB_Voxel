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
	int ProcessABCDir(std::string target_dir, std::string log_file, int& k_size, int& n_min_k, int& padding, int& bandwidth, unsigned max_threads = 0);
}