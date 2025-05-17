#include "Scripts.h"
#include "LOG.h"
#include <iostream>
#include "IO_Test.h"

int main() {

    //To Does 
    // move processing functions to ProessChildren
    // remove legacy parsing functions in all source files 
    // add children process to genertae TypeCount only 
    // *config reader -> instead of hard coding parama

    initLogger("program.log");
    Scripts::ABCtoDataset();
    return 0;
}

