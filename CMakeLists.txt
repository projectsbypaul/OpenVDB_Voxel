cmake_minimum_required(VERSION 3.14)
project(OpenVDB_Voxel)

set(CMAKE_CXX_STANDARD 17)

# === Locate dependencies ===
list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB") 
find_package(OpenVDB REQUIRED)
find_package(yaml-cpp REQUIRED)

find_path(LIBZIP_INCLUDE_DIR zip.h REQUIRED)
find_library(LIBZIP_LIBRARY zip REQUIRED)

# === JobController Executable ===
file(GLOB_RECURSE JOBCONTROLLER_SOURCES
    JobController/src/*.cpp
    JobController/JobController.cpp
)

add_executable(JobController ${JOBCONTROLLER_SOURCES})

target_include_directories(JobController PRIVATE
    JobController/include
    ${LIBZIP_INCLUDE_DIR}
)

target_link_libraries(JobController
    ${LIBZIP_LIBRARY}
)

# === OpenVDB_Voxel Executable ===
file(GLOB_RECURSE OPENVDBVOXEL_SOURCES
    OpenVDB_Voxel/src/*.cpp
    OpenVDB_Voxel/OpenVDB_Voxel.cpp
)

add_executable(OpenVDB_Voxel ${OPENVDBVOXEL_SOURCES})

target_include_directories(OpenVDB_Voxel PRIVATE
    OpenVDB_Voxel/include
    ${OpenVDB_INCLUDE_DIRS}
)

target_link_libraries(OpenVDB_Voxel
    OpenVDB::openvdb
    yaml-cpp
)

# === Logging ===
message(STATUS "OpenVDB include dir: ${OpenVDB_INCLUDE_DIRS}")
message(STATUS "OpenVDB library: ${OpenVDB_LIBRARIES}")
message(STATUS "libzip include dir: ${LIBZIP_INCLUDE_DIR}")
message(STATUS "libzip library: ${LIBZIP_LIBRARY}")
message(STATUS "yaml-cpp found and linked.")
