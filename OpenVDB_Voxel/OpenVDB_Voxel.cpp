#include "Scripts.h"
#include "LOG.h"
#include <iostream>
#include "IO_Test.h"

int main() {

    //To Does 
    // * add children process to genertae TypeCount only 
    // * config reader -> instead of hard coding parama

    initLogger("FaceTypeBenchmark_TBB_1_cpu_const.log");
    Scripts::ABCgetFaceTypeMaps();
    return 0;
}

