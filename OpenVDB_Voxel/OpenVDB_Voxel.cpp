#include "Scripts.h"
#include "LOG.h"
#include <iostream>

int main() {

    initLogger("program.log");

    LOG_FUNC("test");

    //Scripts::stripLinesFormOBJ();
    //Scripts::CopyAndRenameYMLandOBJ();
    Scripts::ABCtoDataset();

    

    return 0;
}

