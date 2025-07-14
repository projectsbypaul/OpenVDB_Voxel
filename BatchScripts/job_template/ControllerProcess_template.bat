@echo off
REM Batch script to launch JobController.exe multiple times in parallel

REM --- Configuration ---
SET "GLOB_THREAD_COUNT=16"
SET "CHUNK=chunk_00"
SET "EXECUTABLE_PATH=%~dp0..\x64\Debug\JobController.exe"
SET "EXECUTABLE_CHILD_PROCESS=%~dp0..\x64\Debug\OpenVDB_Voxel.exe"
SET "MAIN_OUTPUT_DIR=%~dp0Parallel_Run_Logs\JobController\%CHUNK%"
SET "GLOB_SOURCE_DIR=H:\ABC\ABC_parsed_files\ABC_%CHUNK%"
SET "GLOB_TARGET_DIR=H:\ABC\ABC_Datasets\Segmentation\val_ks_16_pad_4_bw_5_vs_adaptive_n3\ABC_%CHUNK%\ABC_Data_ks_16_pad_4_bw_5_vs_adaptive_n3"
SET "GLOB_JOB_DIR=H:\ABC\ABC_jobs\job_%CHUNK%"
SET "JOB_TYPE=default"

REM Create necessary directories
IF NOT EXIST "%MAIN_OUTPUT_DIR%" (
    echo Creating directory for output logs: %MAIN_OUTPUT_DIR%
    MKDIR "%MAIN_OUTPUT_DIR%"
)
IF NOT EXIST "%GLOB_TARGET_DIR%" (
    echo Creating glob target directory: %GLOB_TARGET_DIR%
    MKDIR "%GLOB_TARGET_DIR%"
)

REM Check if executable exists
IF NOT EXIST "%EXECUTABLE_PATH%" (
    echo ERROR: Executable not found at "%EXECUTABLE_PATH%"
    pause
    GOTO :EOF
)
echo Executable found: %EXECUTABLE_PATH%
echo.

REM --- Launch instances ---
FOR /L %%I IN (1,1,%GLOB_THREAD_COUNT%) DO (
    SETLOCAL ENABLEDELAYEDEXPANSION
    SET "ID=00%%I"
    SET "ID=!ID:~-3!"
    SET "INSTANCE_ID=Instance!ID!"
    SET "APP_LOG=%MAIN_OUTPUT_DIR%\!INSTANCE_ID!_app_internal.log"
    SET "JOB_LOC=%GLOB_JOB_DIR%\!INSTANCE_ID!.job"
    SET "STDOUT_LOG=%MAIN_OUTPUT_DIR%\!INSTANCE_ID!_stdout.txt"
    SET "STDERR_LOG=%MAIN_OUTPUT_DIR%\!INSTANCE_ID!_stderr.txt"

    echo Launching !INSTANCE_ID!...
    echo   App Log: !APP_LOG!
    echo   JobLoc:  !JOB_LOC!
    echo   Stdout:  !STDOUT_LOG!
    echo   Stderr:  !STDERR_LOG!

    START "!INSTANCE_ID! Process" /B "%EXECUTABLE_PATH%" "%JOB_TYPE%" "%GLOB_SOURCE_DIR%" "%GLOB_TARGET_DIR%" "!JOB_LOC!" "!APP_LOG!" "%EXECUTABLE_CHILD_PROCESS%" > "!STDOUT_LOG!" 2> "!STDERR_LOG!"
    ENDLOCAL
    echo.
)

echo All instances have been launched in the background.
echo Check the "%MAIN_OUTPUT_DIR%" directory for individual logs.
pause