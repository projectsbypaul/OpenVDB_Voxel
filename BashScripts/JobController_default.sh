#!/bin/bash
# Bash script to launch JobController multiple times in parallel

# --- Configuration ---
GLOB_THREAD_COUNT=16
CHUNK="chunk_benchmark"
EXECUTABLE_PATH="/mnt/c/Users/pschuster/source/repos/OpenVDB_Voxel/build/bin/JobController"
EXECUTABLE_CHILD_PROCESS="/mnt/c/Users/pschuster/source/repos/OpenVDB_Voxel/build/bin/OpenVDB_Voxel"
MAIN_OUTPUT_DIR="/mnt/h/libzip_test/logs/benchmark_zip_wsl_debug"
GLOB_SOURCE_DIR="/mnt/h/ABC/ABC_parsed_files/ABC_${CHUNK}.zip"
GLOB_TARGET_DIR="/mnt/h/libzip_test/benchmark_zip"
GLOB_JOB_DIR="/mnt/h/libzip_test/job"
JOB_TYPE="zip_mode"

# Create necessary directories
if [ ! -d "$MAIN_OUTPUT_DIR" ]; then
    echo "Creating directory for output logs: $MAIN_OUTPUT_DIR"
    mkdir -p "$MAIN_OUTPUT_DIR"
fi
if [ ! -d "$GLOB_TARGET_DIR" ]; then
    echo "Creating glob target directory: $GLOB_TARGET_DIR"
    mkdir -p "$GLOB_TARGET_DIR"
fi

# Check if executable exists
if [ ! -f "$EXECUTABLE_PATH" ]; then
    echo "ERROR: Executable not found at $EXECUTABLE_PATH"
    read -n1 -r -p "Press any key to exit..."
    exit 1
fi
echo "Executable found: $EXECUTABLE_PATH"
echo

# --- Launch instances ---
for (( i=1; i<=GLOB_THREAD_COUNT; i++ )); do
    ID=$(printf "%03d" $i)
    INSTANCE_ID="Instance${ID}"
    APP_LOG="${MAIN_OUTPUT_DIR}/${INSTANCE_ID}_app_internal.log"
    JOB_LOC="${GLOB_JOB_DIR}/${INSTANCE_ID}.job"
    STDOUT_LOG="${MAIN_OUTPUT_DIR}/${INSTANCE_ID}_stdout.txt"
    STDERR_LOG="${MAIN_OUTPUT_DIR}/${INSTANCE_ID}_stderr.txt"

    echo "Launching $INSTANCE_ID..."
    echo "  App Log: $APP_LOG"
    echo "  JobLoc:  $JOB_LOC"
    echo "  Stdout:  $STDOUT_LOG"
    echo "  Stderr:  $STDERR_LOG"

    "$EXECUTABLE_PATH" "$JOB_TYPE" "$GLOB_SOURCE_DIR" "$GLOB_TARGET_DIR" "$JOB_LOC" "$APP_LOG" "$EXECUTABLE_CHILD_PROCESS" \
        > "$STDOUT_LOG" 2> "$STDERR_LOG" &
    echo
done

echo "All instances have been launched in the background."
echo "Check the \"$MAIN_OUTPUT_DIR\" directory for individual logs."
read -n1 -r -p "Press any key to exit..."