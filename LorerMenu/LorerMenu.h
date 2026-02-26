#pragma once
#include <imgui.h>
#include <string>
#include <vector>

namespace Lorer {

    struct Color {
        float r, g, b, a;
        Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) 
            : r(r), g(g), b(b), a(a) {}
        ImU32 ToImU32() const { return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a)); }
        ImVec4 ToImVec4() const { return ImVec4(r, g, b, a); }
    };

    struct AimbotConfig {
        bool enabled = false;
        float fov = 90.0f;
        float smoothness = 5.0f;
        int targetBone = 0;
        bool visibleCheck = true;
        bool silentAim = false;
        bool drawFOV = true;
        float aimSpeed = 1.0f;
        Color fovColor = Color(1.0f, 0.0f, 0.5f, 0.5f);
    };

    struct VisualsConfig {
        bool espEnabled = false;
        bool boxESP = true;
        bool healthESP = true;
        bool nameESP = true;
        bool distanceESP = true;
        bool skeletonESP = false;
        bool snaplines = false;
        bool headDot = false;
        bool weaponESP = false;
        
        Color enemyColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
        Color teamColor = Color(0.0f, 1.0f, 0.0f, 1.0f);
        Color visibleColor = Color(1.0f, 1.0f, 0.0f, 1.0f);
        
        float boxThickness = 1.5f;
        int snaplinePosition = 0;
    };

    struct MiscConfig {
        bool bunnyHop = false;
        bool noFlash = false;
        bool radar = false;
        bool fovChanger = false;
        int fovValue = 90;
        bool autoStrafe = false;
        bool fakeLag = false;
        int fakeLagAmount = 14;
    };

    struct SettingsConfig {
        int menuKey = 0;
        bool streamProof = false;
        bool autoUpdateOffsets = true;
        bool saveOnExit = true;
        bool showWatermark = true;
        char configName[64] = "default";
    };

    class LorerMenu {
    public:
        LorerMenu();
        ~LorerMenu();

        void Initialize();
        void Render();
        void ToggleMenu();
        bool IsMenuOpen() const { return m_isOpen; }

        AimbotConfig& GetAimbotConfig() { return m_aimbot; }
        VisualsConfig& GetVisualsConfig() { return m_visuals; }
        MiscConfig& GetMiscConfig() { return m_misc; }
        SettingsConfig& GetSettingsConfig() { return m_settings; }

    private:
        void ApplyStyle();
        void RenderHeader();
        void RenderTabs();
        void RenderAimbotTab();
        void RenderVisualsTab();
        void RenderMiscTab();
        void RenderSettingsTab();
        void RenderFooter();
        
        bool ToggleButton(const char* label, bool* v, float width = 0.0f);
        bool ColorPicker(const char* label, Color& color, bool showAlpha = true);
        void HelpMarker(const char* desc);
        void SeparatorWithText(const char* text);

        bool m_isOpen = true;
        int m_currentTab = 0;
        ImVec2 m_windowSize = ImVec2(500, 420);
        
        AimbotConfig m_aimbot;
        VisualsConfig m_visuals;
        MiscConfig m_misc;
        SettingsConfig m_settings;

        float m_animProgress = 0.0f;
        bool m_initialized = false;

        static const char* const m_bones[];
        static const char* const m_snaplinePositions[];
    };

}
