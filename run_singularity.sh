#!/bin/bash

# Set container paths
SCRIPT_NAME="SingularityController.sh"      # Name of the script inside /scripts
SCRIPT_DIR="/scripts"
MAIN_OUTPUT_DIR="/logs"
GLOB_SOURCE_DIR="/source"
GLOB_TARGET_DIR="/target"
GLOB_JOB_DIR="/jobs"

# Set host paths (edit these!)
HOST_ROOT="/mnt/h/singularity_benchmark"

HOST_SCRIPTS="/mnt/c/Users/pschuster/source/repos/OpenVDB_Voxel/BashScripts"
HOST_LOGS="$HOST_ROOT/logs"
HOST_SOURCE_DIR="$HOST_ROOT/source/ABC_chunk_benchmark"
HOST_TARGET_DIR="$HOST_ROOT/target"
HOST_JOBS="$HOST_ROOT/jobs"

# Container image and script to run
SIF_IMAGE="OpenVDB_Voxel.sif"
EXECUTABLE="$SCRIPT_DIR/$SCRIPT_NAME"

singularity exec \
  --bind "$HOST_SCRIPTS:$SCRIPT_DIR" \
  --bind "$HOST_LOGS:$MAIN_OUTPUT_DIR" \
  --bind "$HOST_SOURCE_DIR:$GLOB_SOURCE_DIR" \
  --bind "$HOST_TARGET_DIR:$GLOB_TARGET_DIR" \
  --bind "$HOST_JOBS:$GLOB_JOB_DIR" \
  "$SIF_IMAGE" \
  "$EXECUTABLE"