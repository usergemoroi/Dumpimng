#include "MemoryManager.h"
#include <algorithm>
#include <cctype>

MemoryManager::MemoryManager() {}

MemoryManager::~MemoryManager() {
    if (m_hProcess) {
        CloseHandle(m_hProcess);
        m_hProcess = nullptr;
    }
}

bool MemoryManager::Initialize() {
    if (m_initialized) return true;

    // Get current process (we're injected into HD-Player.exe)
    m_hProcess = GetCurrentProcess();
    
    // Find Unity module base
    if (!FindUnityModule()) {
        return false;
    }

    m_initialized = true;
    return true;
}

bool MemoryManager::FindUnityModule() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    
    if (!EnumProcessModules(m_hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        return false;
    }

    int count = cbNeeded / sizeof(HMODULE);
    for (int i = 0; i < count; i++) {
        char modName[MAX_PATH];
        if (GetModuleFileNameExA(m_hProcess, hMods[i], modName, MAX_PATH)) {
            std::string name = modName;
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            
            // Look for Unity-related modules
            if (name.find("libunity.so") != std::string::npos ||
                name.find("unityplayer.dll") != std::string::npos ||
                name.find("gameassembly.dll") != std::string::npos) {
                
                MODULEINFO modInfo;
                if (GetModuleInformation(m_hProcess, hMods[i], &modInfo, sizeof(modInfo))) {
                    m_unityBase = (uintptr_t)modInfo.lpBaseOfDll;
                    m_moduleSize = modInfo.SizeOfImage;
                    return true;
                }
            }
        }
    }

    return false;
}

bool MemoryManager::GetModuleInfo(const wchar_t* moduleName, uintptr_t& base, uintptr_t& size) {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    
    if (!EnumProcessModules(m_hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        return false;
    }

    int count = cbNeeded / sizeof(HMODULE);
    for (int i = 0; i < count; i++) {
        wchar_t modName[MAX_PATH];
        if (GetModuleFileNameExW(m_hProcess, hMods[i], modName, MAX_PATH)) {
            if (wcsstr(modName, moduleName)) {
                MODULEINFO modInfo;
                if (GetModuleInformation(m_hProcess, hMods[i], &modInfo, sizeof(modInfo))) {
                    base = (uintptr_t)modInfo.lpBaseOfDll;
                    size = modInfo.SizeOfImage;
                    return true;
                }
            }
        }
    }

    return false;
}

bool MemoryManager::Read(uintptr_t address, void* buffer, size_t size) {
    if (!m_initialized || !address) return false;
    
    SIZE_T bytesRead = 0;
    return ReadProcessMemory(m_hProcess, (LPCVOID)address, buffer, size, &bytesRead) && bytesRead == size;
}

bool MemoryManager::Write(uintptr_t address, const void* buffer, size_t size) {
    if (!m_initialized || !address) return false;
    
    DWORD oldProtect;
    if (!VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    
    SIZE_T bytesWritten = 0;
    bool result = WriteProcessMemory(m_hProcess, (LPVOID)address, buffer, size, &bytesWritten) && bytesWritten == size;
    
    VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
    return result;
}

uintptr_t MemoryManager::ResolvePointer(uintptr_t base, const std::vector<uintptr_t>& offsets) {
    uintptr_t address = base;
    
    for (size_t i = 0; i < offsets.size(); i++) {
        if (!Read(address, &address, sizeof(uintptr_t))) {
            return 0;
        }
        address += offsets[i];
    }
    
    return address;
}

uintptr_t MemoryManager::FindPattern(const char* moduleName, const char* pattern, const char* mask) {
    uintptr_t moduleBase = 0, moduleSize = 0;
    if (!GetModuleInfo(std::wstring(moduleName, moduleName + strlen(moduleName)).c_str(), moduleBase, moduleSize)) {
        return 0;
    }

    size_t patternLen = strlen(mask);
    std::vector<char> data(moduleSize);
    
    if (!Read(moduleBase, data.data(), moduleSize)) {
        return 0;
    }

    for (size_t i = 0; i < moduleSize - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] == 'x' && data[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return moduleBase + i;
        }
    }

    return 0;
}

uintptr_t MemoryManager::GetGameManager() {
    if (m_gameManager) return m_gameManager;
    
    uintptr_t staticInstance = m_unityBase + Offsets::GameManager_StaticInstance;
    m_gameManager = Read<uintptr_t>(staticInstance);
    return m_gameManager;
}

uintptr_t MemoryManager::GetPlayerManager() {
    if (m_playerManager) return m_playerManager;
    
    uintptr_t staticInstance = m_unityBase + Offsets::PlayerManager_StaticInstance;
    m_playerManager = Read<uintptr_t>(staticInstance);
    return m_playerManager;
}

uintptr_t MemoryManager::GetLocalPlayer() {
    uintptr_t playerManager = GetPlayerManager();
    if (!playerManager) return 0;
    
    m_localPlayer = Read<uintptr_t>(playerManager + Offsets::PlayerManager_LocalPlayer);
    return m_localPlayer;
}

std::vector<uintptr_t> MemoryManager::GetAllPlayers() {
    std::vector<uintptr_t> players;
    
    uintptr_t playerManager = GetPlayerManager();
    if (!playerManager) return players;
    
    uintptr_t playerList = Read<uintptr_t>(playerManager + Offsets::PlayerManager_PlayerList);
    if (!playerList) return players;
    
    // HashSet iteration (simplified)
    uintptr_t count = Read<uintptr_t>(playerList + 0x10);
    
    for (uintptr_t i = 0; i < count && i < 64; i++) {
        uintptr_t player = Read<uintptr_t>(playerList + 0x20 + i * sizeof(uintptr_t));
        if (player && player != m_localPlayer) {
            players.push_back(player);
        }
    }
    
    return players;
}

GameStructs::Vector3 MemoryManager::GetPlayerPosition(uintptr_t player) {
    if (!player) return {};
    
    uintptr_t movementController = Read<uintptr_t>(player + Offsets::PlayerController_MovementController);
    if (!movementController) return {};
    
    uintptr_t translationData = Read<uintptr_t>(movementController + Offsets::MovementController_TranslationData);
    if (!translationData) return {};
    
    return Read<GameStructs::Vector3>(translationData + Offsets::TranslationData_Position);
}

GameStructs::Vector3 MemoryManager::GetPlayerHead(uintptr_t player) {
    GameStructs::Vector3 pos = GetPlayerPosition(player);
    pos.z += 1.7f; // Approximate head height
    return pos;
}

int MemoryManager::GetPlayerHealth(uintptr_t player) {
    if (!player) return 0;
    return Read<int>(player + Offsets::PlayerController_Health);
}

int MemoryManager::GetPlayerTeam(uintptr_t player) {
    if (!player) return 0;
    return Read<int>(player + Offsets::PlayerController_Team);
}

bool MemoryManager::IsPlayerVisible(uintptr_t player) {
    if (!player) return false;
    return Read<bool>(player + Offsets::PlayerController_IsVisible);
}

bool MemoryManager::IsPlayerAlive(uintptr_t player) {
    return GetPlayerHealth(player) > 0;
}

std::string MemoryManager::GetPlayerName(uintptr_t player) {
    if (!player) return "";
    
    uintptr_t namePtr = Read<uintptr_t>(player + Offsets::PlayerController_Name);
    if (!namePtr) return "";
    
    char buffer[64] = {0};
    Read(namePtr, buffer, sizeof(buffer) - 1);
    return std::string(buffer);
}

GameStructs::Vector3 MemoryManager::GetLocalPosition() {
    return GetPlayerPosition(GetLocalPlayer());
}

GameStructs::Vector3 MemoryManager::GetLocalHead() {
    return GetPlayerHead(GetLocalPlayer());
}

int MemoryManager::GetLocalHealth() {
    return GetPlayerHealth(GetLocalPlayer());
}

int MemoryManager::GetLocalTeam() {
    return GetPlayerTeam(GetLocalPlayer());
}

GameStructs::ViewMatrix MemoryManager::GetViewMatrix() {
    GameStructs::ViewMatrix vm;
    
    // Get camera instance
    uintptr_t camera = Read<uintptr_t>(m_unityBase + Offsets::Camera_Main);
    if (camera) {
        vm.matrix = Read<GameStructs::Matrix4x4>(camera + Offsets::Camera_ViewMatrix);
    }
    
    return vm;
}

bool MemoryManager::WorldToScreen(const GameStructs::Vector3& world, GameStructs::Vector2& screen) {
    GameStructs::ViewMatrix vm = GetViewMatrix();
    return vm.WorldToScreen(world, screen, m_screenWidth, m_screenHeight);
}

float MemoryManager::GetCameraFOV() {
    uintptr_t camera = Read<uintptr_t>(m_unityBase + Offsets::Camera_Main);
    if (!camera) return 90.0f;
    return Read<float>(camera + Offsets::Camera_FOV);
}

int MemoryManager::GetCurrentAmmo(uintptr_t player) {
    if (!player) return 0;
    
    uintptr_t weaponController = Read<uintptr_t>(player + Offsets::PlayerController_WeaponController);
    if (!weaponController) return 0;
    
    return Read<int>(weaponController + Offsets::WeaponController_Ammo);
}

bool MemoryManager::IsReloading(uintptr_t player) {
    if (!player) return false;
    
    uintptr_t weaponController = Read<uintptr_t>(player + Offsets::PlayerController_WeaponController);
    if (!weaponController) return false;
    
    return Read<bool>(weaponController + Offsets::WeaponController_IsReloading);
}

GameStructs::PlayerInfo MemoryManager::GetPlayerInfo(uintptr_t player) {
    GameStructs::PlayerInfo info;
    info.address = player;
    info.health = GetPlayerHealth(player);
    info.team = GetPlayerTeam(player);
    info.isVisible = IsPlayerVisible(player);
    info.isAlive = IsPlayerAlive(player);
    info.name = GetPlayerName(player);
    info.position = GetPlayerPosition(player);
    info.headPosition = GetPlayerHead(player);
    info.ammo = GetCurrentAmmo(player);
    
    // Calculate distance to local player
    GameStructs::Vector3 localPos = GetLocalPosition();
    info.distance = localPos.Distance(info.position);
    
    return info;
}
