#include "CheatFeatures.h"
#include <windows.h>
#include <chrono>
#include <algorithm>

CheatFeatures::CheatFeatures() {}

CheatFeatures::~CheatFeatures() {}

void CheatFeatures::Initialize(MemoryManager* memManager, Lorer::LorerMenu* menu) {
    m_memManager = memManager;
    m_menu = menu;
    m_initialized = true;
    
    // Calculate screen center
    m_screenCenterX = Offsets::ScreenWidth / 2;
    m_screenCenterY = Offsets::ScreenHeight / 2;
}

void CheatFeatures::Update() {
    if (!m_initialized || !m_memManager || !m_menu) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    float currentTime = std::chrono::duration<float>(now.time_since_epoch()).count();
    m_deltaTime = currentTime - m_lastUpdateTime;
    m_lastUpdateTime = currentTime;
    
    // Update features based on menu settings
    Lorer::AimbotConfig& aimbot = m_menu->GetAimbotConfig();
    Lorer::MiscConfig& misc = m_menu->GetMiscConfig();
    
    // Run features
    if (aimbot.enabled) {
        RunAimbot();
    }
    
    if (misc.bunnyHop) {
        RunBunnyHop();
    }
    
    if (misc.noFlash) {
        RunNoFlash();
    }
    
    if (misc.fovChanger) {
        RunFOVChanger();
    }
}

void CheatFeatures::RenderESP(ImDrawList* drawList) {
    if (!m_initialized || !m_memManager || !m_menu) return;
    
    Lorer::VisualsConfig& espConfig = m_menu->GetVisualsConfig();
    if (!espConfig.espEnabled) return;
    
    uintptr_t localPlayer = m_memManager->GetLocalPlayer();
    if (!localPlayer) return;
    
    GameStructs::PlayerInfo localInfo = m_memManager->GetPlayerInfo(localPlayer);
    std::vector<uintptr_t> players = m_memManager->GetAllPlayers();
    
    for (uintptr_t player : players) {
        GameStructs::PlayerInfo playerInfo = m_memManager->GetPlayerInfo(player);
        
        // Skip dead players
        if (!playerInfo.isAlive) continue;
        
        // Skip teammates if needed (optional)
        // if (playerInfo.team == localInfo.team) continue;
        
        RenderPlayerESP(drawList, playerInfo, localInfo);
    }
    
    // Draw FOV circle for aimbot
    Lorer::AimbotConfig& aimbot = m_menu->GetAimbotConfig();
    if (aimbot.enabled && aimbot.drawFOV) {
        ImU32 fovColor = aimbot.fovColor.ToImU32();
        float fovRadius = aimbot.fov * 2.67f; // Scale FOV to screen pixels
        
        drawList->AddCircle(
            ImVec2(m_screenCenterX, m_screenCenterY),
            fovRadius,
            fovColor,
            64,
            1.5f
        );
    }
}

void CheatFeatures::RenderPlayerESP(ImDrawList* drawList, const GameStructs::PlayerInfo& player, const GameStructs::PlayerInfo& localPlayer) {
    Lorer::VisualsConfig& config = m_menu->GetVisualsConfig();
    
    // Get screen position
    GameStructs::Vector2 screenPos, headScreenPos;
    if (!m_memManager->WorldToScreen(player.position, screenPos)) return;
    if (!m_memManager->WorldToScreen(player.headPosition, headScreenPos)) return;
    
    // Determine color based on team and visibility
    ImU32 color;
    bool isEnemy = player.team != localPlayer.team;
    
    if (isEnemy) {
        if (player.isVisible) {
            color = config.visibleColor.ToImU32();
        } else {
            color = config.enemyColor.ToImU32();
        }
    } else {
        color = config.teamColor.ToImU32();
    }
    
    // Calculate box dimensions
    float height = abs(screenPos.y - headScreenPos.y) * 1.5f;
    float width = height * 0.5f;
    float boxLeft = headScreenPos.x - width / 2;
    float boxTop = headScreenPos.y - height * 0.1f;
    
    // Box ESP
    if (config.boxESP) {
        drawList->AddRect(
            ImVec2(boxLeft, boxTop),
            ImVec2(boxLeft + width, boxTop + height),
            color,
            0.0f,
            0,
            config.boxThickness
        );
    }
    
    // Health bar
    if (config.healthESP) {
        float healthPercent = player.health / 100.0f;
        float healthBarWidth = 4.0f;
        float healthBarHeight = height;
        
        // Background
        drawList->AddRectFilled(
            ImVec2(boxLeft - healthBarWidth - 2, boxTop),
            ImVec2(boxLeft - 2, boxTop + healthBarHeight),
            ImColor(0, 0, 0, 150)
        );
        
        // Health
        float healthGreen = healthPercent * 255.0f;
        float healthRed = (1.0f - healthPercent) * 255.0f;
        ImU32 healthColor = ImColor(healthRed, healthGreen, 0, 255);
        
        drawList->AddRectFilled(
            ImVec2(boxLeft - healthBarWidth - 2, boxTop + healthBarHeight * (1.0f - healthPercent)),
            ImVec2(boxLeft - 2, boxTop + healthBarHeight),
            healthColor
        );
        
        // Health text
        char healthText[16];
        snprintf(healthText, sizeof(healthText), "%d", player.health);
        drawList->AddText(
            ImVec2(boxLeft - healthBarWidth - 20, boxTop + healthBarHeight - 15),
            ImColor(255, 255, 255, 255),
            healthText
        );
    }
    
    // Name ESP
    if (config.nameESP && !player.name.empty()) {
        ImVec2 textSize = ImGui::CalcTextSize(player.name.c_str());
        drawList->AddText(
            ImVec2(headScreenPos.x - textSize.x / 2, boxTop - 20),
            color,
            player.name.c_str()
        );
    }
    
    // Distance ESP
    if (config.distanceESP) {
        char distText[32];
        snprintf(distText, sizeof(distText), "%.1fm", player.distance);
        
        ImVec2 textSize = ImGui::CalcTextSize(distText);
        drawList->AddText(
            ImVec2(headScreenPos.x - textSize.x / 2, boxTop + height + 5),
            color,
            distText
        );
    }
    
    // Snaplines
    if (config.snaplines) {
        ImVec2 lineStart, lineEnd;
        
        switch (config.snaplinePosition) {
            case 0: // Top
                lineStart = ImVec2(m_screenCenterX, 0);
                break;
            case 1: // Center
                lineStart = ImVec2(m_screenCenterX, m_screenCenterY);
                break;
            case 2: // Bottom
            default:
                lineStart = ImVec2(m_screenCenterX, Offsets::ScreenHeight);
                break;
        }
        
        lineEnd = ImVec2(screenPos.x, screenPos.y);
        drawList->AddLine(lineStart, lineEnd, color, 1.0f);
    }
    
    // Skeleton ESP
    if (config.skeletonESP) {
        // Simplified skeleton - just draw lines connecting body parts
        GameStructs::Vector2 hip, leftKnee, rightKnee, leftFoot, rightFoot;
        
        // Approximate skeleton positions
        GameStructs::Vector3 hipWorld = player.position;
        hipWorld.z += 1.0f;
        
        GameStructs::Vector3 leftFootWorld = player.position;
        leftFootWorld.x -= 0.2f;
        
        GameStructs::Vector3 rightFootWorld = player.position;
        rightFootWorld.x += 0.2f;
        
        if (m_memManager->WorldToScreen(hipWorld, hip)) {
            // Draw spine to head
            drawList->AddLine(ImVec2(hip.x, hip.y), ImVec2(headScreenPos.x, headScreenPos.y), color, 1.5f);
        }
    }
    
    // Weapon ESP
    if (config.weaponESP) {
        char weaponText[32];
        snprintf(weaponText, sizeof(weaponText), "Ammo: %d", player.ammo);
        
        ImVec2 textSize = ImGui::CalcTextSize(weaponText);
        drawList->AddText(
            ImVec2(headScreenPos.x - textSize.x / 2, boxTop + height + 20),
            ImColor(200, 200, 200, 255),
            weaponText
        );
    }
    
    // Head dot
    if (config.headDot) {
        drawList->AddCircleFilled(
            ImVec2(headScreenPos.x, headScreenPos.y),
            4.0f,
            color,
            16
        );
    }
}

void CheatFeatures::RunAimbot() {
    if (!m_memManager) return;
    
    Lorer::AimbotConfig& config = m_menu->GetAimbotConfig();
    
    // Check if aimbot key is pressed (usually mouse button)
    if (!GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_RBUTTON)) {
        m_currentTarget = 0;
        return;
    }
    
    // Find best target
    uintptr_t target = GetBestTarget();
    if (!target) return;
    
    GameStructs::PlayerInfo targetInfo = m_memManager->GetPlayerInfo(target);
    GameStructs::Vector3 localPos = m_memManager->GetLocalPosition();
    
    // Get target position based on bone selection
    GameStructs::Vector3 targetPos;
    switch (config.targetBone) {
        case 0: // Head
            targetPos = targetInfo.headPosition;
            break;
        case 1: // Neck
            targetPos = targetInfo.headPosition;
            targetPos.z -= 0.1f;
            break;
        case 2: // Chest
            targetPos = targetInfo.position;
            targetPos.z += 1.2f;
            break;
        case 3: // Stomach
            targetPos = targetInfo.position;
            targetPos.z += 0.8f;
            break;
        default:
            targetPos = targetInfo.headPosition;
            break;
    }
    
    // Calculate angle to target
    GameStructs::Vector2 targetAngle = Math::CalcAngle(localPos, targetPos);
    
    // Smooth the aim
    targetAngle = Math::SmoothAngle(m_lastAimAngle, targetAngle, config.smoothness);
    m_lastAimAngle = targetAngle;
    
    // Apply aim (mouse movement)
    SmoothAim(targetAngle, config.smoothness);
}

uintptr_t CheatFeatures::GetBestTarget() {
    if (!m_memManager) return 0;
    
    Lorer::AimbotConfig& config = m_menu->GetAimbotConfig();
    uintptr_t localPlayer = m_memManager->GetLocalPlayer();
    if (!localPlayer) return 0;
    
    GameStructs::PlayerInfo localInfo = m_memManager->GetPlayerInfo(localPlayer);
    std::vector<uintptr_t> players = m_memManager->GetAllPlayers();
    
    uintptr_t bestTarget = 0;
    float bestFov = config.fov;
    float bestDistance = FLT_MAX;
    
    for (uintptr_t player : players) {
        GameStructs::PlayerInfo info = m_memManager->GetPlayerInfo(player);
        
        // Skip dead players
        if (!info.isAlive) continue;
        
        // Skip teammates
        if (info.team == localInfo.team) continue;
        
        // Visible check
        if (config.visibleCheck && !info.isVisible) continue;
        
        // Check FOV
        float fov = GetFovToPlayer(info);
        if (fov > config.fov) continue;
        
        // Get screen position to check if in center
        GameStructs::Vector2 screenPos;
        if (!m_memManager->WorldToScreen(info.headPosition, screenPos)) continue;
        
        float distanceToCenter = Math::Distance2D(
            screenPos.x, screenPos.y,
            m_screenCenterX, m_screenCenterY
        );
        
        // Prioritize closest to crosshair
        if (distanceToCenter < bestDistance) {
            bestDistance = distanceToCenter;
            bestTarget = player;
        }
    }
    
    return bestTarget;
}

float CheatFeatures::GetFovToPlayer(const GameStructs::PlayerInfo& player) {
    GameStructs::Vector2 screenPos;
    if (!m_memManager->WorldToScreen(player.headPosition, screenPos)) {
        return FLT_MAX;
    }
    
    return Math::Distance2D(
        screenPos.x, screenPos.y,
        m_screenCenterX, m_screenCenterY
    );
}

void CheatFeatures::SmoothAim(const GameStructs::Vector2& angle, float smoothness) {
    // Convert angle to screen pixels
    float fov = m_memManager->GetCameraFOV();
    float pixelsPerDegree = Offsets::ScreenWidth / fov;
    
    // Calculate how much to move mouse
    float moveX = angle.y * pixelsPerDegree * 0.1f / smoothness;
    float moveY = angle.x * pixelsPerDegree * 0.1f / smoothness;
    
    // Apply movement
    MouseMove(static_cast<int>(moveX), static_cast<int>(moveY));
}

GameStructs::Vector2 CheatFeatures::CalcAngle(const GameStructs::Vector3& src, const GameStructs::Vector3& dst) {
    return Math::CalcAngle(src, dst);
}

void CheatFeatures::RunBunnyHop() {
    // Check if space is pressed and player is on ground
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        uintptr_t localPlayer = m_memManager->GetLocalPlayer();
        if (!localPlayer) return;
        
        // Check if player is on ground (simplified)
        GameStructs::Vector3 velocity = m_memManager->GetPlayerPosition(localPlayer);
        
        // Simulate jump
        keybd_event(VK_SPACE, 0, 0, 0);
        keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
    }
}

void CheatFeatures::RunNoFlash() {
    // Implementation would modify flash effect in memory
    // This is a placeholder for the actual memory modification
}

void CheatFeatures::RunFOVChanger() {
    Lorer::MiscConfig& config = m_menu->GetMiscConfig();
    
    if (!config.fovChanger) return;
    
    // Modify camera FOV in memory
    uintptr_t camera = m_memManager->Read<uintptr_t>(m_memManager->GetUnityBase() + Offsets::Camera_Main);
    if (camera) {
        m_memManager->Write<float>(camera + Offsets::Camera_FOV, static_cast<float>(config.fovValue));
    }
}

void CheatFeatures::RunTriggerBot() {
    Lorer::MiscConfig& config = m_menu->GetMiscConfig();
    
    // Check if crosshair is on enemy
    uintptr_t target = GetBestTarget();
    if (target && GetFovToPlayer(m_memManager->GetPlayerInfo(target)) < 5.0f) {
        MouseDown();
        m_leftMouseDown = true;
    } else if (m_leftMouseDown) {
        MouseUp();
        m_leftMouseDown = false;
    }
}

void CheatFeatures::MouseMove(int x, int y) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = x;
    input.mi.dy = y;
    SendInput(1, &input, sizeof(INPUT));
}

void CheatFeatures::MouseDown(int button) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = (button == 0) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &input, sizeof(INPUT));
}

void CheatFeatures::MouseUp(int button) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = (button == 0) ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
    SendInput(1, &input, sizeof(INPUT));
}

void CheatFeatures::KeyDown(int key) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    SendInput(1, &input, sizeof(INPUT));
}

void CheatFeatures::KeyUp(int key) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}
