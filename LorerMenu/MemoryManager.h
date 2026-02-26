#pragma once
#include "Offsets.h"
#include <windows.h>
#include <vector>
#include <string>
#include <Psapi.h>
#include <TlHelp32.h>

#pragma comment(lib, "psapi.lib")

class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    bool Initialize();
    bool IsInitialized() const { return m_initialized; }
    
    // Memory operations
    bool Read(uintptr_t address, void* buffer, size_t size);
    bool Write(uintptr_t address, const void* buffer, size_t size);
    
    template<typename T>
    T Read(uintptr_t address) {
        T value = {};
        Read(address, &value, sizeof(T));
        return value;
    }
    
    template<typename T>
    bool Write(uintptr_t address, const T& value) {
        return Write(address, &value, sizeof(T));
    }
    
    // Address resolution
    uintptr_t GetUnityBase() const { return m_unityBase; }
    uintptr_t ResolvePointer(uintptr_t base, const std::vector<uintptr_t>& offsets);
    uintptr_t FindPattern(const char* moduleName, const char* pattern, const char* mask);
    
    // Game data access
    uintptr_t GetGameManager();
    uintptr_t GetPlayerManager();
    uintptr_t GetLocalPlayer();
    std::vector<uintptr_t> GetAllPlayers();
    
    // Player information
    GameStructs::Vector3 GetPlayerPosition(uintptr_t player);
    GameStructs::Vector3 GetPlayerHead(uintptr_t player);
    int GetPlayerHealth(uintptr_t player);
    int GetPlayerTeam(uintptr_t player);
    bool IsPlayerVisible(uintptr_t player);
    bool IsPlayerAlive(uintptr_t player);
    std::string GetPlayerName(uintptr_t player);
    
    // Local player info
    GameStructs::Vector3 GetLocalPosition();
    GameStructs::Vector3 GetLocalHead();
    int GetLocalHealth();
    int GetLocalTeam();
    
    // View matrix
    GameStructs::ViewMatrix GetViewMatrix();
    bool WorldToScreen(const GameStructs::Vector3& world, GameStructs::Vector2& screen);
    
    // Camera
    float GetCameraFOV();
    
    // Weapon info
    int GetCurrentAmmo(uintptr_t player);
    bool IsReloading(uintptr_t player);
    
    // Complete player info structure
    GameStructs::PlayerInfo GetPlayerInfo(uintptr_t player);
    
private:
    bool m_initialized = false;
    HANDLE m_hProcess = nullptr;
    uintptr_t m_unityBase = 0;
    uintptr_t m_moduleSize = 0;
    
    bool FindUnityModule();
    bool GetModuleInfo(const wchar_t* moduleName, uintptr_t& base, uintptr_t& size);
    
    // Cached addresses
    uintptr_t m_gameManager = 0;
    uintptr_t m_playerManager = 0;
    uintptr_t m_localPlayer = 0;
    
    // Screen dimensions
    int m_screenWidth = 1920;
    int m_screenHeight = 1080;
};
