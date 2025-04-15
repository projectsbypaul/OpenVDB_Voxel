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
#include "ABCProcessing.h"


namespace fs = std::filesystem;

int main() {
    //Scripts::stripLinesFormOBJ();
    //Scripts::CopyAndRenameYMLandOBJ();
    Scripts::ABCtoDataset();
    return 0;
}

