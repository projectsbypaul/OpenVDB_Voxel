@echo off

SET "WORK_DIR=%~dp0"
SET "LOG_DIR=%WORK_DIR%logs"

REM Create log directory if it doesn't exist
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

REM --- Job names ---
SET "JOB_01=run_purge_by_yaml_chunk_01"
SET "JOB_02=run_purge_by_yaml_chunk_02"
SET "JOB_03=run_purge_by_yaml_chunk_03"

REM --- Start Jobs ---

echo Starting %JOB_01%...
call "%WORK_DIR%jobs\%JOB_01%.bat" > "%LOG_DIR%\%JOB_01%_out.log" 2> "%LOG_DIR%\%JOB_01%_err.log"

echo Waiting for job...
timeout /t 10800 /nobreak

echo Starting %JOB_02%...
call "%WORK_DIR%jobs\%JOB_02%.bat" > "%LOG_DIR%\%JOB_02%_out.log" 2> "%LOG_DIR%\%JOB_02%_err.log"

echo Waiting for job...
timeout /t 10800 /nobreak

echo Starting %JOB_03%...
call "%WORK_DIR%jobs\%JOB_03%.bat" > "%LOG_DIR%\%JOB_03%_out.log" 2> "%LOG_DIR%\%JOB_03%_err.log"

echo All jobs completed. See log files in: %LOG_DIR%
pause

