// LOG.cpp
#include "LOG.h"
#include <iostream>

std::ofstream logFile;

void initLogger(const std::string& path) {
    logFile.open(path, std::ios::out | std::ios::app);
    if (!logFile) {
        std::cerr << "Failed to open log file: " << path << std::endl;
        std::exit(1);
    }
}