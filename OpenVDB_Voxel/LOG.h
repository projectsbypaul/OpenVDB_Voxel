#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <iostream>

extern std::ofstream logFile;
void initLogger(const std::string& path);

#define LOG(msg) do { logFile << msg << std::endl; } while(0)

#define LOG_FUNC(msg) do { \
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); \
    std::tm timeInfo; \
    localtime_s(&timeInfo, &now); \
    logFile << "[" << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S") << "] "; \
    logFile << "[" << __FUNCTION__ << "] " << msg << std::endl; \
} while(0)


#if defined(_MSC_VER)
#define FUNC_SIG __FUNCSIG__
#else
#define FUNC_SIG __PRETTY_FUNCTION__
#endif

#define LOG_FUNC_SIG(msg) do { \
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); \
    std::tm timeInfo; \
    localtime_s(&timeInfo, &now); \
    logFile << "[" << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S") << "] "; \
    logFile << "[" << FUNC_SIG << "] " << msg << std::endl; \
} while(0)

#endif