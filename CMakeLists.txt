cmake_minimum_required(VERSION 3.14)
project(OpenVDB_Voxel)

set(CMAKE_CXX_STANDARD 17)

# === Locate custom OpenVDB install ===
list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)

# === Locate system libzip ===
find_path(LIBZIP_INCLUDE_DIR zip.h REQUIRED)
find_library(LIBZIP_LIBRARY zip REQUIRED)

# === JobController Executable ===
file(GLOB JOBCONTROLLER_SOURCES JobController/*.cpp)
add_executable(JobController ${JOBCONTROLLER_SOURCES})

target_include_directories(JobController PRIVATE ${LIBZIP_INCLUDE_DIR})
target_link_libraries(JobController
    OpenVDB::openvdb
    ${LIBZIP_LIBRARY}
)

# === OpenVDB_Vo Executable ===
file(GLOB OPENVDBVO_SOURCES OpenVDB_Vo/*.cpp)
add_executable(OpenVDB_Vo ${OPENVDBVO_SOURCES})

target_link_libraries(OpenVDB_Vo
    OpenVDB::openvdb
)

# === Optional: print found libs/paths ===
message(STATUS "OpenVDB include dir: ${OpenVDB_INCLUDE_DIRS}")
message(STATUS "OpenVDB library: ${OpenVDB_LIBRARIES}")
message(STATUS "libzip include dir: ${LIBZIP_INCLUDE_DIR}")
message(STATUS "libzip library: ${LIBZIP_LIBRARY}")