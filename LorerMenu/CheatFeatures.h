#pragma once
#include "Offsets.h"
#include "MemoryManager.h"
#include "LorerMenu.h"
#include <imgui.h>
#include <vector>
#include <cmath>

class CheatFeatures {
public:
    CheatFeatures();
    ~CheatFeatures();
    
    void Initialize(MemoryManager* memManager, Lorer::LorerMenu* menu);
    void Update();
    void RenderESP(ImDrawList* drawList);
    
    // Feature implementations
    void RunAimbot();
    void RenderPlayerESP(ImDrawList* drawList, const GameStructs::PlayerInfo& player, const GameStructs::PlayerInfo& localPlayer);
    void RunBunnyHop();
    void RunNoFlash();
    void RunFOVChanger();
    void RunTriggerBot();
    
    // Helper functions
    GameStructs::Vector2 CalcAngle(const GameStructs::Vector3& src, const GameStructs::Vector3& dst);
    float GetFovToPlayer(const GameStructs::PlayerInfo& player);
    uintptr_t GetBestTarget();
    void SmoothAim(const GameStructs::Vector2& angle, float smoothness);
    
    // Input simulation
    void MouseMove(int x, int y);
    void MouseDown(int button = 0);
    void MouseUp(int button = 0);
    void KeyDown(int key);
    void KeyUp(int key);
    
private:
    MemoryManager* m_memManager = nullptr;
    Lorer::LorerMenu* m_menu = nullptr;
    
    bool m_initialized = false;
    
    // Screen center
    int m_screenCenterX = 960;
    int m_screenCenterY = 540;
    
    // Target data
    uintptr_t m_currentTarget = 0;
    GameStructs::Vector2 m_lastAimAngle;
    
    // Timing
    float m_lastUpdateTime = 0.0f;
    float m_deltaTime = 0.0f;
    
    // Input state
    bool m_leftMouseDown = false;
    bool m_spaceDown = false;
};

// Math utilities
namespace Math {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float RAD2DEG = 180.0f / PI;
    constexpr float DEG2RAD = PI / 180.0f;
    
    inline float Clamp(float value, float min, float max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
    
    inline float Distance2D(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return sqrtf(dx * dx + dy * dy);
    }
    
    inline GameStructs::Vector2 CalcAngle(const GameStructs::Vector3& src, const GameStructs::Vector3& dst) {
        GameStructs::Vector2 angle;
        GameStructs::Vector3 delta = dst - src;
        
        float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
        
        angle.x = asinf(delta.z / hyp) * RAD2DEG;
        angle.y = atan2f(delta.y, delta.x) * RAD2DEG;
        
        return angle;
    }
    
    inline GameStructs::Vector2 SmoothAngle(const GameStructs::Vector2& current, const GameStructs::Vector2& target, float smoothness) {
        GameStructs::Vector2 result;
        result.x = current.x + (target.x - current.x) / smoothness;
        result.y = current.y + (target.y - current.y) / smoothness;
        return result;
    }
    
    inline GameStructs::Vector3 Lerp(const GameStructs::Vector3& a, const GameStructs::Vector3& b, float t) {
        return a + (b - a) * t;
    }
    
    inline bool IsInFOV(const GameStructs::Vector2& screenPos, float fov, int screenCenterX, int screenCenterY) {
        float distance = Distance2D(screenPos.x, screenPos.y, screenCenterX, screenCenterY);
        return distance <= fov;
    }
}
