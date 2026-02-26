# External Dependencies

This directory should contain external libraries required for building Lorer.

## Required Libraries

### 1. ImGui
- **Download:** https://github.com/ocornut/imgui
- **Branch:** docking (recommended)
- **Extract to:** `external/imgui/`

```
external/imgui/
├── imgui.cpp
├── imgui.h
├── imgui_draw.cpp
├── imgui_tables.cpp
├── imgui_widgets.cpp
├── imconfig.h
├── imgui_internal.h
├── imstb_rectpack.h
├── imstb_textedit.h
├── imstb_truetype.h
└── backends/
    ├── imgui_impl_dx11.cpp
    ├── imgui_impl_dx11.h
    ├── imgui_impl_win32.cpp
    └── imgui_impl_win32.h
```

### 2. MinHook
- **Download:** https://github.com/TsudaKageyu/minhook
- **Extract to:** `external/minhook/`

```
external/minhook/
├── include/
│   └── MinHook.h
└── lib/
    ├── libMinHook.x64.lib
    └── libMinHook.x86.lib
```

## Quick Download (PowerShell)

```powershell
# Create directories
New-Item -ItemType Directory -Force -Path external\imgui
New-Item -ItemType Directory -Force -Path external\minhook\include
New-Item -ItemType Directory -Force -Path external\minhook\lib

# Download ImGui (simplified - manual download recommended)
# Download MinHook release from GitHub releases page
```

## Build Order

1. Download and extract ImGui to `external/imgui/`
2. Download and extract MinHook to `external/minhook/`
3. Run `build.bat` or use CMake directly
