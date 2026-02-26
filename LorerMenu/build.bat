@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo.
echo  ╔══════════════════════════════════════════╗
echo  ║         Lorer Build Script               ║
echo  ║   Standoff 2 Cheat - v1.0                ║
echo  ╚══════════════════════════════════════════╝
echo.

:: Check for Visual Studio
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo [*] Looking for Visual Studio...
    
    :: Try VS 2022
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo [!] Visual Studio not found!
        echo [!] Please install Visual Studio 2019 or 2022
        pause
        exit /b 1
    )
)

echo [+] Compiler found

:: Create build directory
if not exist "build" mkdir build
cd build

:: Check dependencies
echo.
echo [*] Checking dependencies...

:: ImGui
if not exist "..\external\imgui\imgui.cpp" (
    echo [!] ImGui not found at external/imgui/
    echo [!] Download from: https://github.com/ocornut/imgui
    echo [!] Extract to external/imgui/
)

:: MinHook
if not exist "..\external\minhook\include\MinHook.h" (
    echo [!] MinHook not found at external/minhook/
    echo [!] Download from: https://github.com/TsudaKageyu/minhook
    echo [!] Extract to external/minhook/
)

:: Configure with CMake
echo.
echo [*] Configuring with CMake...
cmake .. -DBUILD_LOADER=ON -DBUILD_DLL=ON -DBUILD_DEMO=OFF -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo [!] CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

:: Build
echo.
echo [*] Building Release...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [!] Build failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ╔══════════════════════════════════════════╗
echo ║          Build Successful!               ║
echo ╚══════════════════════════════════════════╝
echo.

:: Copy output files
echo [*] Copying output files...
if exist "Release\Loader.exe" (
    copy /Y "Release\Loader.exe" "..\Loader.exe" >nul
    echo [+] Loader.exe
)
if exist "Release\Lorer.dll" (
    copy /Y "Release\Lorer.dll" "..\Lorer.dll" >nul
    echo [+] Lorer.dll
)

cd ..

echo.
echo Output files:
echo   - Loader.exe  (Inject DLL into HD-Player.exe)
echo   - Lorer.dll   (Main cheat module)
echo.
echo Usage:
echo   1. Start BlueStacks 5
echo   2. Launch Standoff 2
echo   3. Run Loader.exe as Administrator
echo   4. Press INSERT to toggle menu
echo.

pause
