@echo off
REM Batch script to launch MultiProcess.exe multiple times in parallel

REM --- Configuration ---
REM Path to your executable (using relative path from script location)
SET "EXECUTABLE_PATH=%~dp0..\x64\Debug\OpenVDB_Voxel.exe"

REM Directory to store output logs from each instance
SET "MAIN_OUTPUT_DIR=%~dp0Parallel_Run_Logs\thread_count_6"

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

REM --- Instance 1 ---
SET "INSTANCE1_ID=Instance001"
SET "INSTANCE1_APP_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE1_ID%_app_internal.log"
SET "INSTANCE1_SOURCE_DIR=C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark"
SET "INSTANCE1_TARGET_DIR=C:\Local_Data\ABC\ABC_statistics\benchmarks\MultiThreadBatch\%INSTANCE1_ID%"
SET "INSTANCE1_STDOUT_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE1_ID%_stdout.txt"
SET "INSTANCE1_STDERR_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE1_ID%_stderr.txt"

echo Launching %INSTANCE1_ID%...
echo   App Log: %INSTANCE1_APP_LOG%
echo   Source:  %INSTANCE1_SOURCE_DIR%
echo   Target:  %INSTANCE1_TARGET_DIR%
echo   Stdout:  %INSTANCE1_STDOUT_LOG%
echo   Stderr:  %INSTANCE1_STDERR_LOG%
START "%INSTANCE1_ID% Process" /B "%EXECUTABLE_PATH%" "%INSTANCE1_APP_LOG%" "%INSTANCE1_SOURCE_DIR%" "%INSTANCE1_TARGET_DIR%" > "%INSTANCE1_STDOUT_LOG%" 2> "%INSTANCE1_STDERR_LOG%"
echo.

REM --- Instance 2 ---
SET "INSTANCE2_ID=Instance002"
SET "INSTANCE2_APP_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE2_ID%_app_internal.log"
SET "INSTANCE2_SOURCE_DIR=C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark"
SET "INSTANCE2_TARGET_DIR=C:\Local_Data\ABC\ABC_statistics\benchmarks\MultiThreadBatch\%INSTANCE2_ID%"
SET "INSTANCE2_STDOUT_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE2_ID%_stdout.txt"
SET "INSTANCE2_STDERR_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE2_ID%_stderr.txt"

echo Launching %INSTANCE2_ID%...
echo   App Log: %INSTANCE2_APP_LOG%
echo   Source:  %INSTANCE2_SOURCE_DIR%
echo   Target:  %INSTANCE2_TARGET_DIR%
echo   Stdout:  %INSTANCE2_STDOUT_LOG%
echo   Stderr:  %INSTANCE2_STDERR_LOG%
START "%INSTANCE2_ID% Process" /B "%EXECUTABLE_PATH%" "%INSTANCE2_APP_LOG%" "%INSTANCE2_SOURCE_DIR%" "%INSTANCE2_TARGET_DIR%" > "%INSTANCE2_STDOUT_LOG%" 2> "%INSTANCE2_STDERR_LOG%"
echo.

REM --- Instance 3 ---
SET "INSTANCE3_ID=Instance003"
SET "INSTANCE3_APP_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE3_ID%_app_internal.log"
SET "INSTANCE3_SOURCE_DIR=C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark"
SET "INSTANCE3_TARGET_DIR=C:\Local_Data\ABC\ABC_statistics\benchmarks\MultiThreadBatch\%INSTANCE3_ID%"
SET "INSTANCE3_STDOUT_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE3_ID%_stdout.txt"
SET "INSTANCE3_STDERR_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE3_ID%_stderr.txt"

echo Launching %INSTANCE3_ID%...
echo   App Log: %INSTANCE3_APP_LOG%
echo   Source:  %INSTANCE3_SOURCE_DIR%
echo   Target:  %INSTANCE3_TARGET_DIR%
echo   Stdout:  %INSTANCE3_STDOUT_LOG%
echo   Stderr:  %INSTANCE3_STDERR_LOG%
START "%INSTANCE3_ID% Process" /B "%EXECUTABLE_PATH%" "%INSTANCE3_APP_LOG%" "%INSTANCE3_SOURCE_DIR%" "%INSTANCE3_TARGET_DIR%" > "%INSTANCE3_STDOUT_LOG%" 2> "%INSTANCE3_STDERR_LOG%"
echo.

REM --- Instance 4 ---
SET "INSTANCE4_ID=Instance004"
SET "INSTANCE4_APP_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE4_ID%_app_internal.log"
SET "INSTANCE4_SOURCE_DIR=C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark"
SET "INSTANCE4_TARGET_DIR=C:\Local_Data\ABC\ABC_statistics\benchmarks\MultiThreadBatch\%INSTANCE4_ID%"
SET "INSTANCE4_STDOUT_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE4_ID%_stdout.txt"
SET "INSTANCE4_STDERR_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE4_ID%_stderr.txt"

echo Launching %INSTANCE4_ID%...
echo   App Log: %INSTANCE4_APP_LOG%
echo   Source:  %INSTANCE4_SOURCE_DIR%
echo   Target:  %INSTANCE4_TARGET_DIR%
echo   Stdout:  %INSTANCE4_STDOUT_LOG%
echo   Stderr:  %INSTANCE4_STDERR_LOG%
START "%INSTANCE4_ID% Process" /B "%EXECUTABLE_PATH%" "%INSTANCE4_APP_LOG%" "%INSTANCE4_SOURCE_DIR%" "%INSTANCE4_TARGET_DIR%" > "%INSTANCE4_STDOUT_LOG%" 2> "%INSTANCE4_STDERR_LOG%"
echo.

REM --- Instance 5 ---
SET "INSTANCE5_ID=Instance005"
SET "INSTANCE5_APP_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE5_ID%_app_internal.log"
SET "INSTANCE5_SOURCE_DIR=C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark"
SET "INSTANCE5_TARGET_DIR=C:\Local_Data\ABC\ABC_statistics\benchmarks\MultiThreadBatch\%INSTANCE5_ID%"
SET "INSTANCE5_STDOUT_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE5_ID%_stdout.txt"
SET "INSTANCE5_STDERR_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE5_ID%_stderr.txt"

echo Launching %INSTANCE5_ID%...
echo   App Log: %INSTANCE5_APP_LOG%
echo   Source:  %INSTANCE5_SOURCE_DIR%
echo   Target:  %INSTANCE5_TARGET_DIR%
echo   Stdout:  %INSTANCE5_STDOUT_LOG%
echo   Stderr:  %INSTANCE5_STDERR_LOG%
START "%INSTANCE5_ID% Process" /B "%EXECUTABLE_PATH%" "%INSTANCE5_APP_LOG%" "%INSTANCE5_SOURCE_DIR%" "%INSTANCE5_TARGET_DIR%" > "%INSTANCE5_STDOUT_LOG%" 2> "%INSTANCE5_STDERR_LOG%"
echo.

REM --- Instance 6 ---
SET "INSTANCE6_ID=Instance006"
SET "INSTANCE6_APP_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE6_ID%_app_internal.log"
SET "INSTANCE6_SOURCE_DIR=C:\Local_Data\ABC\ABC_parsed_files\ABC_chunk_benchmark"
SET "INSTANCE6_TARGET_DIR=C:\Local_Data\ABC\ABC_statistics\benchmarks\MultiThreadBatch\%INSTANCE6_ID%"
SET "INSTANCE6_STDOUT_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE6_ID%_stdout.txt"
SET "INSTANCE6_STDERR_LOG=%MAIN_OUTPUT_DIR%\%INSTANCE6_ID%_stderr.txt"

echo Launching %INSTANCE6_ID%...
echo   App Log: %INSTANCE6_APP_LOG%
echo   Source:  %INSTANCE6_SOURCE_DIR%
echo   Target:  %INSTANCE6_TARGET_DIR%
echo   Stdout:  %INSTANCE6_STDOUT_LOG%
echo   Stderr:  %INSTANCE6_STDERR_LOG%
START "%INSTANCE6_ID% Process" /B "%EXECUTABLE_PATH%" "%INSTANCE6_APP_LOG%" "%INSTANCE6_SOURCE_DIR%" "%INSTANCE6_TARGET_DIR%" > "%INSTANCE6_STDOUT_LOG%" 2> "%INSTANCE6_STDERR_LOG%"
echo.

echo All instances have been launched in the background.
echo Check the "%MAIN_OUTPUT_DIR%" directory for individual logs.
echo.
echo Your C++ application instances are now running. This script does not wait for them to complete.
pause
