# Lorer Menu

A beautiful, modern ImGui-based menu for game modification tools. Features smooth animations, custom styling, and an intuitive user interface.

## Features

- **Modern UI Design**: Dark theme with purple accent colors
- **Animated Toggle Switches**: Smooth on/off animations
- **Tabbed Interface**: Organized categories (Aimbot, Visuals, Misc, Settings)
- **Color Pickers**: For ESP and overlay customization
- **Configuration System**: Save and load configurations
- **Watermark Display**: Optional watermark when menu is hidden

## Screenshots

```
┌─────────────────────────────────────────────────┐
│  LORER                              [Close]     │
├─────────────────────────────────────────────────┤
│ [Aimbot] [Visuals] [Misc] [Settings]            │
├─────────────────────────────────────────────────┤
│                                                 │
│  Main Settings                                  │
│  ────────────────────────────────────────────   │
│                                                 │
│  ⬤─● Enable Aimbot                              │
│                                                 │
│  FOV: ═════════════●═══════════════  90.0°      │
│  Smoothness: ═══════●════════════════  5.0      │
│  Target Bone: [Head ▼]                          │
│                                                 │
│  Additional Options                             │
│  ────────────────────────────────────────────   │
│                                                 │
│  ⬤─● Visible Check                              │
│  ⬤──○ Silent Aim                                │
│  ⬤─● Draw FOV Circle                            │
│                                                 │
├─────────────────────────────────────────────────┤
│  Lorer v1.0 | Made with <3 | Standoff 2 v0.37.1 │
└─────────────────────────────────────────────────┘
```

## Building

### Prerequisites

- CMake 3.16+
- Visual Studio 2022 or compatible C++ compiler
- DirectX 11 SDK
- ImGui (docking branch recommended)
- MinHook (optional, for hooking)

### Setup

1. Clone the repository
2. Download ImGui and extract to `external/imgui/`
3. (Optional) Download MinHook and extract to `external/minhook/`
4. Build with CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Project Structure

```
LorerMenu/
├── LorerMenu.h          # Menu configuration structures and class
├── LorerMenu.cpp        # Menu implementation
├── LorerOverlay.h       # Overlay management class
├── LorerIntegration.h   # Integration utilities
├── LorerDemo.cpp        # Standalone demo application
├── CMakeLists.txt       # CMake build configuration
└── README.md            # This file
```

## Usage

### Standalone Demo

Run `LorerDemo.exe` to see the menu in action without a game.

### Integration into Your Project

```cpp
#include "LorerIntegration.h"

// In your DirectX Present hook:
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    LorerIntegration::OnPresent(pSwapChain);
    return oPresent(pSwapChain, SyncInterval, Flags);
}

// In DllMain:
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        // Initialize and install hooks...
        break;
    case DLL_PROCESS_DETACH:
        LorerIntegration::Shutdown();
        break;
    }
    return TRUE;
}
```

### Accessing Menu Settings

```cpp
#include "LorerOverlay.h"

// Check if aimbot is enabled
bool aimbotEnabled = Lorer::LorerOverlay::GetInstance()
    .GetMenu().GetAimbotConfig().enabled;

// Get ESP settings
const auto& espConfig = Lorer::LorerOverlay::GetInstance()
    .GetMenu().GetVisualsConfig();

if (espConfig.espEnabled) {
    // Draw ESP with colors
    auto enemyColor = espConfig.enemyColor.ToImU32();
    // ...
}
```

## Configuration

### Aimbot Settings

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| enabled | bool | false | Enable/disable aimbot |
| fov | float | 90.0 | Field of view for targeting |
| smoothness | float | 5.0 | Aim smoothness (1-30) |
| targetBone | int | 0 | Target bone index |
| visibleCheck | bool | true | Check if target is visible |
| silentAim | bool | false | Silent aim mode |
| drawFOV | bool | true | Draw FOV circle |
| fovColor | Color | Purple | FOV circle color |

### Visuals Settings

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| espEnabled | bool | false | Enable ESP |
| boxESP | bool | true | Draw bounding boxes |
| healthESP | bool | true | Show health bars |
| nameESP | bool | true | Show player names |
| distanceESP | bool | true | Show distance |
| skeletonESP | bool | false | Draw skeleton |
| snaplines | bool | false | Draw snaplines |
| enemyColor | Color | Red | Enemy color |
| teamColor | Color | Green | Teammate color |

### Misc Settings

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| bunnyHop | bool | false | Auto bunny hop |
| noFlash | bool | false | Remove flash effect |
| radar | bool | false | Show enemies on radar |
| fovChanger | bool | false | Enable FOV changer |
| fovValue | int | 90 | Custom FOV value |

## Customization

### Changing Colors

Edit `LorerMenu.cpp` in the `ApplyStyle()` function to customize the theme:

```cpp
// Main accent color (purple)
colors[ImGuiCol_Button] = ImVec4(0.56f, 0.19f, 0.96f, 0.50f);

// Background color
colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
```

### Adding New Features

1. Add configuration struct in `LorerMenu.h`
2. Add UI rendering in the appropriate tab function
3. Access settings via `GetMenu().GetYourConfig()`

## License

This project is provided for educational purposes only.

## Credits

- ImGui by Omar Cornut
- MinHook by Tsuda Kageyu

## Disclaimer

This software is intended for educational and research purposes only. Use at your own risk. The authors are not responsible for any misuse of this software.
