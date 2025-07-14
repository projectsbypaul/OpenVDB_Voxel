cmake_minimum_required(VERSION 3.14)
project(OpenVDB_Voxel)

set(CMAKE_CXX_STANDARD 17)

# === Locate custom installed dependencies===
# === Locate OpenVDB ===
list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB") 
#/usr/local/lib/cmake/<package>/<package>-config.cmake --> if this file doesnt exist list(APPEND ...) 
find_package(OpenVDB REQUIRED)

# === Locate system libzip ===
find_path(LIBZIP_INCLUDE_DIR zip.h REQUIRED)
find_library(LIBZIP_LIBRARY zip REQUIRED)

# === Locate yaml-cpp ===
find_package(yaml-cpp REQUIRED)

# === JobController Executable ===
file(GLOB JOBCONTROLLER_SOURCES JobController/JobController.cpp)
add_executable(JobController ${JOBCONTROLLER_SOURCES})

target_include_directories(JobController PRIVATE ${LIBZIP_INCLUDE_DIR})
target_link_libraries(JobController
    OpenVDB::openvdb
    ${LIBZIP_LIBRARY}
    yaml-cpp
)

# === OpenVDB_Voxel Executable ===
file(GLOB OPENVDBVOXEL_SOURCES OpenVDB_Voxel/OpenVDB_Voxel.cpp)
add_executable(OpenVDB_Voxel ${OPENVDBVOXEL_SOURCES})

target_link_libraries(OpenVDB_Voxel
    OpenVDB::openvdb
    yaml-cpp
)

# === Optional: print found libs/paths ===
message(STATUS "OpenVDB include dir: ${OpenVDB_INCLUDE_DIRS}")
message(STATUS "OpenVDB library: ${OpenVDB_LIBRARIES}")
message(STATUS "libzip include dir: ${LIBZIP_INCLUDE_DIR}")
message(STATUS "libzip library: ${LIBZIP_LIBRARY}")
message(STATUS "yaml-cpp found and linked.")