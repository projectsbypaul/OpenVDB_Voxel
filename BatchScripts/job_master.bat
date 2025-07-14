@echo off

SET "WORK_DIR=%~dp0"
SET "LOG_DIR=%WORK_DIR%logs"

REM Create log directory if it doesn't exist
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

REM --- Job names ---
SET "JOB_01=run_benchmark_zip"
SET "JOB_02=run_benchmark_default"

REM --- Start Jobs ---

echo Starting %JOB_01%...
call "%WORK_DIR%jobs\%JOB_01%.bat" > "%LOG_DIR%\%JOB_01%_out.log" 2> "%LOG_DIR%\%JOB_01%_err.log"

echo Waiting for job...
timeout /t 2700 /nobreak

echo Starting %JOB_02%...
call "%WORK_DIR%jobs\%JOB_02%.bat" > "%LOG_DIR%\%JOB_02%_out.log" 2> "%LOG_DIR%\%JOB_02%_err.log"

echo All jobs completed. See log files in: %LOG_DIR%
pause

