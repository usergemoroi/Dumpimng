#pragma once
#include <cstdint>

namespace Offsets {
    // Standoff 2 v0.37.1 - Memory Offsets
    // libunity.so base address required
    
    // GameManager (Static Instance)
    constexpr uintptr_t GameManager_StaticInstance = 0x18;
    
    // PlayerManager (Static Instance via LazySingleton)
    constexpr uintptr_t PlayerManager_StaticInstance = 0x18;
    constexpr uintptr_t PlayerManager_LocalPlayer = 0x68;      // <FDBAECBCFCHCAAE>k__BackingField
    constexpr uintptr_t PlayerManager_PlayerList = 0x38;       // HAHCFEHECFEDHHC (HashSet)
    
    // PlayerController
    constexpr uintptr_t PlayerController_Health = 0x150;       // <GBGDAFDAABGEDCB>k__BackingField
    constexpr uintptr_t PlayerController_Team = 0x154;         // <BGFGGFFCADDEFCE>k__BackingField
    constexpr uintptr_t PlayerController_MovementController = 0x98;  // <ECAFDHEFEDHFDAB>k__BackingField
    constexpr uintptr_t PlayerController_WeaponController = 0xA0;
    constexpr uintptr_t PlayerController_IsVisible = 0x1A0;
    constexpr uintptr_t PlayerController_Name = 0x1C0;
    constexpr uintptr_t PlayerController_PlayerId = 0x1D0;
    
    // MovementController
    constexpr uintptr_t MovementController_TranslationData = 0xB0;
    constexpr uintptr_t MovementController_RotationData = 0xC0;
    constexpr uintptr_t MovementController_Velocity = 0xD0;
    
    // TranslationData
    constexpr uintptr_t TranslationData_Position = 0x1C;       // BFCAFBAGDCBGHHB (Vector3)
    
    // WeaponController
    constexpr uintptr_t WeaponController_CurrentWeapon = 0x10;
    constexpr uintptr_t WeaponController_Ammo = 0x18;
    constexpr uintptr_t WeaponController_MaxAmmo = 0x1C;
    constexpr uintptr_t WeaponController_IsReloading = 0x20;
    constexpr uintptr_t WeaponController_Recoil = 0x30;
    
    // GrenadeManager
    constexpr uintptr_t GrenadeManager_Count = 0x10;
    constexpr uintptr_t GrenadeManager_List = 0x18;
    
    // BombManager
    constexpr uintptr_t BombManager_IsPlanted = 0x10;
    constexpr uintptr_t BombManager_Position = 0x18;
    constexpr uintptr_t BombManager_Timer = 0x24;
    
    // Camera
    constexpr uintptr_t Camera_Main = 0x00;
    constexpr uintptr_t Camera_ViewMatrix = 0xDC;
    constexpr uintptr_t Camera_FOV = 0xEC;
    
    // Screen dimensions (for ESP calculations)
    constexpr int ScreenWidth = 1920;
    constexpr int ScreenHeight = 1080;
    
    // Bone IDs for Aimbot
    enum class BoneIndex : int {
        Head = 6,
        Neck = 5,
        Spine = 4,
        Chest = 3,
        Stomach = 2,
        Pelvis = 1,
        LeftHand = 11,
        RightHand = 21,
        LeftFoot = 26,
        RightFoot = 36
    };
    
    // Team IDs
    enum class TeamId : int {
        Terrorists = 2,
        CounterTerrorists = 3,
        Spectator = 1
    };
}

// Structures for game data
namespace GameStructs {
    struct Vector3 {
        float x, y, z;
        
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        
        Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }
        
        Vector3 operator-(const Vector3& other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        
        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
        
        float Length() const {
            return sqrtf(x * x + y * y + z * z);
        }
        
        float Distance(const Vector3& other) const {
            return (*this - other).Length();
        }
        
        float Dot(const Vector3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }
    };
    
    struct Vector2 {
        float x, y;
        
        Vector2() : x(0), y(0) {}
        Vector2(float x, float y) : x(x), y(y) {}
        
        float Length() const {
            return sqrtf(x * x + y * y);
        }
        
        float Distance(const Vector2& other) const {
            return (*this - other).Length();
        }
        
        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }
    };
    
    struct Matrix4x4 {
        union {
            struct {
                float m00, m01, m02, m03;
                float m10, m11, m12, m13;
                float m20, m21, m22, m23;
                float m30, m31, m32, m33;
            };
            float m[4][4];
            float v[16];
        };
        
        Matrix4x4() {
            for (int i = 0; i < 16; i++) v[i] = 0;
            m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
        }
    };
    
    struct PlayerInfo {
        uintptr_t address;
        int health;
        int team;
        int ammo;
        bool isVisible;
        bool isAlive;
        std::string name;
        Vector3 position;
        Vector3 headPosition;
        float distance;
    };
    
    struct ViewMatrix {
        Matrix4x4 matrix;
        
        bool WorldToScreen(const Vector3& world, Vector2& screen, int screenW, int screenH) {
            float x = matrix.v[0] * world.x + matrix.v[1] * world.y + matrix.v[2] * world.z + matrix.v[3];
            float y = matrix.v[4] * world.x + matrix.v[5] * world.y + matrix.v[6] * world.z + matrix.v[7];
            float w = matrix.v[12] * world.x + matrix.v[13] * world.y + matrix.v[14] * world.z + matrix.v[15];
            
            if (w < 0.001f) return false;
            
            float invW = 1.0f / w;
            screen.x = (x * invW) * (screenW / 2.0f) + (screenW / 2.0f);
            screen.y = -(y * invW) * (screenH / 2.0f) + (screenH / 2.0f);
            
            return true;
        }
    };
}
