#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>
#include <thread>
#include <sstream>  

extern std::ofstream logFile;
extern std::mutex logMutex;

void initLogger(const std::string& path);

inline std::string currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
#if defined(_MSC_VER)
    localtime_s(&tm_buf, &now_time);
#else
    localtime_r(&now_time, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

#define LOG_LEVEL(level, msg) do { \
    std::lock_guard<std::mutex> lock(logMutex); \
    logFile << "[" << currentTimestamp() << "] "; \
    logFile << "[" << level << "] "; \
    logFile << "[Thread " << std::this_thread::get_id() << "] "; \
    logFile << msg << std::endl; \
} while(0)

#define LOG(msg) LOG_LEVEL("LOG", msg)

#define LOG_FUNC(msg) LOG_LEVEL("INFO", std::string(__FUNCTION__) + "(): " + msg)

#if defined(_MSC_VER)
#define FUNC_SIG __FUNCSIG__
#else
#define FUNC_SIG __PRETTY_FUNCTION__
#endif

#define LOG_FUNC_SIG(msg) LOG_LEVEL("INFO", std::string(FUNC_SIG) + ": " + msg)

#endif