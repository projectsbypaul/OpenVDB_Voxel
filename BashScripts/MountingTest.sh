#!/bin/bash
# Bash script to launch JobController multiple times in parallel

# --- Configuration ---

EXECUTABLE_PATH="/opt/OpenVDB_Voxel/build/bin/JobController"
EXECUTABLE_CHILD_PROCESS="/opt/OpenVDB_Voxel/build/bin/OpenVDB_Voxel"

echo Run mounting test...
echo "${LINE:0:50}"

echo Testing JobController  
"$EXECUTABLE_PATH"
echo "${LINE:0:50}"

echo Testing OpenVDB_Voxel
"$EXECUTABLE_CHILD_PROCESS"
echo "${LINE:0:50}"

echo test done.
