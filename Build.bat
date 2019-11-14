@echo Building GLFW Application...
x86_64-w64-mingw32-g++ -g -c -I. src/*.cpp src/*.c
if %errorlevel% neq 0 exit /b %errorlevel%
@echo Compilation complete, proceeding to linking...
x86_64-w64-mingw32-g++ -g -o app.exe *.o -L. -lglfw3 -lopengl32 -lgdi32 -static
if %errorlevel% neq 0 exit /b %errorlevel%
@echo Linking complete
