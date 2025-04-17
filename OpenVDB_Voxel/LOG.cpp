// LOG.cpp
#include "LOG.h"


void initLogger(const std::string& path) {
    logFile.open(path, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Unable to open log file: " + path);
    }
}

inline std::string currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm timeInfo;
    localtime_s(&timeInfo, &in_time);

    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}