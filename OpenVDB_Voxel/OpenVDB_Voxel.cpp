#include "Scripts.h"
#include "LOG.h"
#include <iostream>
#include "IO_Test.h"

int main() {

    initLogger("program.log");
    Scripts::ABCtoDataset();

    return 0;
}

