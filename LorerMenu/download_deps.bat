@echo off
chcp 65001 >nul
echo.
echo ╔════════════════════════════════════════════════════════╗
echo ║           Lorer - Автозагрузка зависимостей            ║
echo ╚════════════════════════════════════════════════════════╝
echo.

:: Создаем структуру папок
if not exist "external\imgui\backends" mkdir external\imgui\backends
if not exist "external\minhook\include" mkdir external\minhook\include
if not exist "external\minhook\lib" mkdir external\minhook\lib

:: Проверяем ImGui
if not exist "external\imgui\imgui.h" (
    echo [*] Загрузка ImGui...
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imgui.h' -OutFile 'external\imgui\imgui.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imgui.cpp' -OutFile 'external\imgui\imgui.cpp'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imgui_draw.cpp' -OutFile 'external\imgui\imgui_draw.cpp'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imgui_tables.cpp' -OutFile 'external\imgui\imgui_tables.cpp'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imgui_widgets.cpp' -OutFile 'external\imgui\imgui_widgets.cpp'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imconfig.h' -OutFile 'external\imgui\imconfig.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imgui_internal.h' -OutFile 'external\imgui\imgui_internal.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imstb_rectpack.h' -OutFile 'external\imgui\imstb_rectpack.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imstb_textedit.h' -OutFile 'external\imgui\imstb_textedit.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/imstb_truetype.h' -OutFile 'external\imgui\imstb_truetype.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/backends/imgui_impl_dx11.h' -OutFile 'external\imgui\backends\imgui_impl_dx11.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/backends/imgui_impl_dx11.cpp' -OutFile 'external\imgui\backends\imgui_impl_dx11.cpp'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/backends/imgui_impl_win32.h' -OutFile 'external\imgui\backends\imgui_impl_win32.h'"
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/ocornut/imgui/docking/backends/imgui_impl_win32.cpp' -OutFile 'external\imgui\backends\imgui_impl_win32.cpp'"
    echo [+] ImGui загружен
) else (
    echo [*] ImGui уже существует
)

:: Проверяем MinHook
if not exist "external\minhook\include\MinHook.h" (
    echo [*] Загрузка MinHook...
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/TsudaKageyu/minhook/master/include/MinHook.h' -OutFile 'external\minhook\include\MinHook.h'"
    echo [!] MinHook.lib нужно скачать вручную с GitHub releases
    echo     https://github.com/TsudaKageyu/minhook/releases
    echo     Положите libMinHook.x64.lib в external\minhook\lib\
) else (
    echo [*] MinHook уже существует
)

echo.
echo ══════════════════════════════════════════════════════════
echo  Для сборки откройте Lorer.sln в Visual Studio 2019/2022
echo  и соберите в Release x64
echo ══════════════════════════════════════════════════════════
echo.
pause
