// LOG.cpp
#include "../include/LOG.h"

std::ofstream logFile;
std::mutex logMutex;

void initLogger(const std::string& path) {
    logFile.open(path, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Unable to open log file: " + path);
    }
}