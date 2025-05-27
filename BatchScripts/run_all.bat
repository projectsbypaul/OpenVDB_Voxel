@echo OFF
echo Starting all scripts...
echo.

REM Call script and wait for it to complete
echo Calling BatchProcess_4...
CALL BatchProcess_4.bat
echo.

echo Calling BatchProcess_8...
CALL BatchProcess_8.bat
echo.

echo Calling BatchProcess_12...
CALL BatchProcess_12.bat
echo.

echo Calling BatchProcess_16...
CALL BatchProcess_16.bat
echo.

echo All scripts have finished.