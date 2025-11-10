#!/bin/bash
# Bash script to launch JobController multiple times in parallel

# --- Default configuration ---
GLOB_THREAD_COUNT=14
EXECUTABLE_PATH="/mnt/c/Users/pschuster/source/repos/OpenVDB_Voxel/build/bin/JobController"
EXECUTABLE_CHILD_PROCESS="/mnt/c/Users/pschuster/source/repos/OpenVDB_Voxel/build/bin/OpenVDB_Voxel"
MAIN_OUTPUT_DIR="/mnt/h/ws_label_test/logs"
JOB_TYPE="subdirLabel"
CLASS_TEMPLATE="edge"
SURFACE_THRESHOLD=8.0
GLOB_SOURCE_DIR="/mnt/h/ws_label_test/target"
GLOB_TARGET_DIR="/mnt/h/ws_label_test/label"
GLOB_JOB_DIR="/mnt/h/ws_label_test/jobs"

# --- Parse arguments ---
usage() {
    echo "Usage: $0 -m <mode> -p <instances> -s <source_file> -t <target_dir> -j <job_dir>"
    exit 1
}

while getopts ":m:p:t:s:j:o:" opt; do
  case ${opt} in
    m ) JOB_TYPE=$OPTARG ;;
    p ) GLOB_THREAD_COUNT=$OPTARG ;;
    t ) GLOB_TARGET_DIR=$OPTARG ;;
    s ) GLOB_SOURCE_DIR=$OPTARG ;;
    j ) GLOB_JOB_DIR=$OPTARG ;;
    o ) MAIN_OUTPUT_DIR=$OPTARG ;; # optional override for log dir
    \? ) echo "Invalid option: -$OPTARG" >&2; usage ;;
    : ) echo "Option -$OPTARG requires an argument." >&2; usage ;;
  esac
done

# --- Validate required args ---
if [[ -z "$JOB_TYPE" || -z "$GLOB_SOURCE_DIR" || -z "$GLOB_TARGET_DIR" || -z "$GLOB_JOB_DIR" ]]; then
    echo "Error: Missing required arguments."
    usage
fi

# Create necessary directories
mkdir -p "$MAIN_OUTPUT_DIR" "$GLOB_TARGET_DIR" "$GLOB_JOB_DIR"

# Check if executable exists
if [ ! -f "$EXECUTABLE_PATH" ]; then
    echo "ERROR: Executable not found at $EXECUTABLE_PATH"
    exit 1
fi
echo "Executable found: $EXECUTABLE_PATH"
echo "Launching $GLOB_THREAD_COUNT instances with mode '$JOB_TYPE'..."
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
    "$EXECUTABLE_PATH" "$JOB_TYPE" "$GLOB_SOURCE_DIR" "$GLOB_TARGET_DIR" "$JOB_LOC" "$APP_LOG" "$CLASS_TEMPLATE" "$SURFACE_THRESHOLD" "$EXECUTABLE_CHILD_PROCESS"\
        > "$STDOUT_LOG" 2> "$STDERR_LOG" &
done

echo "All instances have been launched in the background."
echo "Logs available in: $MAIN_OUTPUT_DIR"
