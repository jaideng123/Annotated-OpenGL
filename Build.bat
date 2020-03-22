@echo Building GLFW Application...
g++ -g -c -I ./include . src/*.cpp src/*.c
if %errorlevel% neq 0 exit /b %errorlevel%
@echo Compilation complete, proceeding to linking...
g++ -g -o app.exe *.o -L . -lglfw3 -lopengl32 -lgdi32 -lassimp.dll -static
if %errorlevel% neq 0 exit /b %errorlevel%
@echo Linking complete
