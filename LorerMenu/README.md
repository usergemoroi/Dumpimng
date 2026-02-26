# Lorer - Standoff 2 Cheat Framework

A comprehensive game modification framework for Standoff 2 (v0.37.1) running on BlueStacks 5 emulator.

## Features

### Aimbot
- **Enable/Disable** - Toggle aimbot functionality
- **FOV Control** - Adjustable field of view (0-360°)
- **Smoothness** - Human-like aim movement
- **Target Bone** - Head, Neck, Chest, Stomach
- **Visible Check** - Only target visible enemies
- **Silent Aim** - Stealthy targeting
- **FOV Circle** - Visual indicator for aimbot range

### Visuals (ESP)
- **Box ESP** - Bounding boxes around players
- **Health Bar** - Visual health indicator
- **Name ESP** - Player names
- **Distance ESP** - Distance in meters
- **Skeleton ESP** - Player skeleton outline
- **Snaplines** - Lines from screen to players
- **Head Dot** - Indicator on head position
- **Weapon ESP** - Current ammo display
- **Color Customization** - Enemy, Team, Visible colors

### Misc Features
- **Bunny Hop** - Automatic jumping
- **No Flash** - Remove flashbang effect
- **FOV Changer** - Custom field of view
- **Fake Lag** - Network manipulation
- **Auto Strafe** - Automatic strafing

### Settings
- **Menu Hotkey** - INSERT/HOME/END/F1
- **Stream Proof** - Hide from screen capture
- **Auto-Update Offsets** - Automatic offset updates
- **Config System** - Save/Load configurations

## Memory Offsets (Standoff 2 v0.37.1)

```cpp
// GameManager
constexpr uintptr_t GameManager_StaticInstance = 0x18;

// PlayerManager
constexpr uintptr_t PlayerManager_StaticInstance = 0x18;
constexpr uintptr_t PlayerManager_LocalPlayer = 0x68;
constexpr uintptr_t PlayerManager_PlayerList = 0x38;

// PlayerController
constexpr uintptr_t PlayerController_Health = 0x150;
constexpr uintptr_t PlayerController_Team = 0x154;
constexpr uintptr_t PlayerController_MovementController = 0x98;
constexpr uintptr_t PlayerController_IsVisible = 0x1A0;

// MovementController
constexpr uintptr_t MovementController_TranslationData = 0xB0;

// TranslationData
constexpr uintptr_t TranslationData_Position = 0x1C;
```

## Project Structure

```
LorerMenu/
├── DllMain.cpp           # DLL entry point with DirectX hooks
├── MemoryManager.h/cpp   # Memory operations and game data access
├── CheatFeatures.h/cpp   # Aimbot, ESP, and other cheat features
├── LorerMenu.h/cpp       # ImGui menu implementation
├── LorerOverlay.h        # Overlay management
├── LorerIntegration.h    # Integration utilities
├── Offsets.h             # Game memory offsets
├── CMakeLists.txt        # Build configuration
└── README.md             # This file
```

## Building

### Prerequisites

- CMake 3.16+
- Visual Studio 2022 (MSVC)
- Windows 10/11 SDK
- DirectX 11 SDK

### Dependencies

1. **ImGui** - Download from https://github.com/ocornut/imgui
   - Extract to `external/imgui/`
   - Use docking branch for best results

2. **MinHook** - Download from https://github.com/TsudaKageyu/minhook
   - Extract to `external/minhook/`
   - Required for DirectX hooking

### Build Commands

```bash
mkdir build
cd build
cmake .. -DBUILD_DLL=ON -DBUILD_DEMO=OFF
cmake --build . --config Release
```

Output: `Lorer.dll` for injection into HD-Player.exe

## Usage

### Injection

1. Start BlueStacks 5
2. Launch Standoff 2
3. Use a DLL injector to inject `Lorer.dll` into `HD-Player.exe`
4. Press **INSERT** to toggle the menu

### Menu Navigation

- **INSERT** - Toggle menu visibility
- **Mouse** - Navigate and interact with menu
- **Left Click** - Toggle options, drag sliders

### Configuration

Configurations are stored in the same directory as the DLL. Use the Settings tab to save/load configurations.

## Technical Details

### DirectX 11 Hooking

The framework hooks `IDXGISwapChain::Present` to render the ImGui overlay. This is done by:

1. Creating a temporary swap chain
2. Getting the Present function address from the vtable
3. Using MinHook to create a detour
4. Rendering ImGui before calling the original Present

### Memory Access

Memory is accessed through standard Windows APIs:
- `ReadProcessMemory` for reading game data
- `WriteProcessMemory` for modifying game data
- Pattern scanning for dynamic address resolution

### ESP Rendering

ESP is rendered using ImGui's background draw list, which allows drawing on top of the game without interfering with the rendering pipeline.

## Anti-Detection

The framework includes several anti-detection measures:

1. **String Obfuscation** - Encrypted strings
2. **Random Delays** - Human-like timing
3. **Memory Protection** - VirtualProtect usage
4. **Stream Proof** - Optional screen capture avoidance

## Disclaimer

This software is provided for **educational and research purposes only**. Using game modifications in online games may violate the terms of service and result in account bans. The authors are not responsible for any misuse of this software.

## Credits

- ImGui by Omar Cornut
- MinHook by Tsuda Kageyu
- Game reverse engineering community

## License

This project is for educational purposes. Use at your own risk.

---

**Version:** 1.0.0  
**Target:** Standoff 2 v0.37.1  
**Platform:** BlueStacks 5 (Windows)
