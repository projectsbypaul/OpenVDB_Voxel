#include "Tools.h"
#include <memory>
#include <iostream>
#include <fstream>
#include "MeshDataHandling.h"
#include "Scripts.h"
#include "DatasetToVoxel.h"
#include <filesystem>
#include <fstream>
#include <string>

#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/LevelSetUtil.h>
#include <openvdb/tools/Composite.h>
#include <openvdb/tools/GridTransformer.h>
#include "GetABCStatistics.h"

#include <CGAL/IO/OBJ.h>


int main() {

   
    std::string traget_dir = "C:/Local_Data/ABC/obj/abc_meta_files/abc_0000_obj_v00";
    std::string log_file = traget_dir + "/ABC_log.csv";
    int kernel_size = 32;
    int n_min_k = 2;
    int padding = 4;
    int bandwidth = 5;
    
    Scripts::ProcessABCDir(traget_dir, log_file, kernel_size, n_min_k, padding, bandwidth, 8);

    return 0;
}

