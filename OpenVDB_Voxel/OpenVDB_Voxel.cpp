#include "Scripts.h"
#include "LOG.h"
#include <iostream>
#include "IO_Test.h"

int main() {

    //To Does 
    // * add children process to genertae TypeCount only 
    // * config reader -> instead of hard coding parama

    initLogger("processABCbenchmark.log");
    Scripts::ABCtoDataset();
    return 0;
}

