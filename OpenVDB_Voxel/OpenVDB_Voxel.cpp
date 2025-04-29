#include "Scripts.h"
#include "LOG.h"
#include <iostream>

int main() {

    initLogger("program.log");
   
    Scripts::ABCtoDatasetAE();

    return 0;
}

