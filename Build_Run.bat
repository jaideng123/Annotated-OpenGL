call Build.bat
if %errorlevel% neq 0 exit /b %errorlevel%
@echo Running application...
.\app
