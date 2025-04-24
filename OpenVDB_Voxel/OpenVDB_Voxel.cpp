#include "Scripts.h"
#include "LOG.h"
#include <iostream>

int main() {

    initLogger("program.log");

    //Scripts::stripLinesFormOBJ();
    //Scripts::CopyAndRenameYMLandOBJ();
    //Scripts::ABCtoDataset();
    //Scripts::MeshToSdfSegments();
    Scripts::ApplySwirlOnMesh();
    return 0;
}

