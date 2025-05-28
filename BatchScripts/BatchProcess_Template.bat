@echo off
REM Batch script to launch MultiProcess.exe multiple times in parallel

REM --- Configuration ---
SET "GLOB_THREAD_COUNT=1"
REM Path to your executable (using relative path from script location)
SET "EXECUTABLE_PATH=%~dp0..\x64\Debug\OpenVDB_Voxel.exe"

REM Directory to store output logs from each instance
SET "MAIN_OUTPUT_DIR=%~dp0Parallel_Run_Logs\thread_count_%GLOB_THREAD_COUNT%"

REM Create the main output directory if it doesn't exist
IF NOT EXIST "%MAIN_OUTPUT_DIR%" (
    echo Creating directory for output logs: %MAIN_OUTPUT_DIR%
    MKDIR "%MAIN_OUTPUT_DIR%"
)
REM --- End Configuration ---

REM --- Check if Executable Exists ---
IF NOT EXIST "%EXECUTABLE_PATH%" (
    echo ERROR: Executable not found at "%EXECUTABLE_PATH%"
    echo Please check the EXECUTABLE_PATH in the script.
    pause
    GOTO :EOF
)
echo Executable found: %EXECUTABLE_PATH%
echo.

REM ---Shared Parameters
SET "GLOB_SOURCE_DIR=C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark
SET "GLOB_TARGET_DIR=C:\Local_Data\ABC\ABC_statistics\benchmarks\Outputs_Benchmark"
SET "GLOB_JOB_DIR=C:\Local_Data\ABC\ABC_jobs\job_benchmark\job_threads_%GLOB_THREAD_COUNT%"

REM --- !!!rember  to change GLOB_THREAD_COUNT!!! --- 
REM --- Instance Blocks Begin --- 




REM --- Instance Blocks END ---

echo All instances have been launched in the background.
echo Check the "%MAIN_OUTPUT_DIR%" directory for individual logs.
echo.
echo Your C++ application instances are now running. This script does not wait for them to complete.
pause