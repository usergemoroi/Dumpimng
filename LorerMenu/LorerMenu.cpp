#include "LorerMenu.h"
#include <imgui_internal.h>
#include <cmath>

namespace Lorer {

    const char* const LorerMenu::m_bones[] = { "Head", "Neck", "Chest", "Stomach", "Pelvis" };
    const char* const LorerMenu::m_snaplinePositions[] = { "Top", "Center", "Bottom" };

    static ImColor LerpColor(ImColor a, ImColor b, float t) {
        return ImColor(
            a.Value.x + (b.Value.x - a.Value.x) * t,
            a.Value.y + (b.Value.y - a.Value.y) * t,
            a.Value.z + (b.Value.z - a.Value.z) * t,
            a.Value.w + (b.Value.w - a.Value.w) * t
        );
    }

    static void DrawGradientRect(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImColor col1, ImColor col2, bool horizontal = true) {
        if (horizontal) {
            for (float i = 0.0f; i < (max.x - min.x); i += 1.0f) {
                float t = i / (max.x - min.x);
                drawList->AddLine(ImVec2(min.x + i, min.y), ImVec2(min.x + i, max.y), LerpColor(col1, col2, t));
            }
        } else {
            for (float i = 0.0f; i < (max.y - min.y); i += 1.0f) {
                float t = i / (max.y - min.y);
                drawList->AddLine(ImVec2(min.x, min.y + i), ImVec2(max.x, min.y + i), LerpColor(col1, col2, t));
            }
        }
    }

    LorerMenu::LorerMenu() {
        memset(m_settings.configName, 0, sizeof(m_settings.configName));
        strcpy_s(m_settings.configName, "default");
    }

    LorerMenu::~LorerMenu() {}

    void LorerMenu::Initialize() {
        if (m_initialized) return;
        m_initialized = true;
        ApplyStyle();
    }

    void LorerMenu::ApplyStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.14f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.24f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.32f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.12f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.14f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.28f, 0.38f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.48f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.56f, 0.19f, 0.96f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.56f, 0.19f, 0.96f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.76f, 0.36f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.56f, 0.19f, 0.96f, 0.50f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.56f, 0.19f, 0.96f, 0.70f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.19f, 0.96f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.56f, 0.19f, 0.96f, 0.40f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.19f, 0.96f, 0.60f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.19f, 0.96f, 0.80f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.56f, 0.19f, 0.96f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.19f, 0.96f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.56f, 0.19f, 0.96f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.19f, 0.96f, 0.70f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.56f, 0.19f, 0.96f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.56f, 0.19f, 0.96f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.56f, 0.19f, 0.96f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.56f, 0.19f, 0.96f, 0.35f);

        style.WindowPadding = ImVec2(0, 0);
        style.FramePadding = ImVec2(8, 6);
        style.CellPadding = ImVec2(6, 4);
        style.ItemSpacing = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 6);
        style.TouchExtraPadding = ImVec2(0, 0);
        style.IndentSpacing = 20;
        style.ScrollbarSize = 12;
        style.GrabMinSize = 10;
        style.WindowBorderSize = 1;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 0;
        style.TabBorderSize = 0;
        style.WindowRounding = 8;
        style.ChildRounding = 6;
        style.FrameRounding = 4;
        style.PopupRounding = 6;
        style.ScrollbarRounding = 8;
        style.GrabRounding = 4;
        style.TabRounding = 4;
        style.AntiAliasedLines = true;
        style.AntiAliasedFill = true;
    }

    void LorerMenu::ToggleMenu() {
        m_isOpen = !m_isOpen;
    }

    void LorerMenu::Render() {
        if (!m_isOpen) return;
        if (!m_initialized) Initialize();

        ImGui::SetNextWindowSize(m_windowSize, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.98f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

        if (ImGui::Begin("##LorerMainWindow", &m_isOpen, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar)) {
            
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();

            // Top gradient header
            DrawGradientRect(drawList, 
                ImVec2(windowPos.x, windowPos.y), 
                ImVec2(windowPos.x + windowSize.x, windowPos.y + 50),
                ImColor(90, 30, 160, 255), 
                ImColor(140, 50, 200, 255),
                true);
            
            // Accent line
            drawList->AddRectFilled(
                ImVec2(windowPos.x, windowPos.y + 50),
                ImVec2(windowPos.x + windowSize.x, windowPos.y + 54),
                ImColor(140, 50, 200, 255));

            ImGui::SetCursorPosY(10);
            ImGui::SetCursorPosX(20);
            
            // Title
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::TextUnformatted("LORER");
            ImGui::PopStyleColor();
            ImGui::PopFont();
            
            ImGui::SameLine(windowSize.x - 100);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            if (ImGui::Button("Close", ImVec2(70, 25))) {
                m_isOpen = false;
            }
            ImGui::PopStyleColor();

            ImGui::SetCursorPosY(60);
            
            // Tabs
            RenderTabs();

            ImGui::SetCursorPosY(105);
            ImGui::SetCursorPosX(10);
            
            ImGui::BeginChild("##Content", ImVec2(windowSize.x - 20, windowSize.y - 120), false, ImGuiWindowFlags_NoScrollbar);
            
            switch (m_currentTab) {
                case 0: RenderAimbotTab(); break;
                case 1: RenderVisualsTab(); break;
                case 2: RenderMiscTab(); break;
                case 3: RenderSettingsTab(); break;
            }
            
            ImGui::EndChild();
            
            RenderFooter();
        }
        ImGui::End();
        
        ImGui::PopStyleVar(2);
    }

    void LorerMenu::RenderHeader() {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("LORER").x) / 2);
        ImGui::TextUnformatted("LORER");
    }

    void LorerMenu::RenderTabs() {
        ImGui::SetCursorPosX(10);
        
        const char* tabs[] = { "Aimbot", "Visuals", "Misc", "Settings" };
        
        for (int i = 0; i < 4; i++) {
            ImGui::SameLine();
            ImGui::PushID(i);
            
            bool isSelected = (m_currentTab == i);
            
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.56f, 0.19f, 0.96f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.56f, 0.19f, 0.96f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.56f, 0.19f, 0.96f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.20f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.32f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.45f, 1.0f));
            }
            
            if (ImGui::Button(tabs[i], ImVec2(110, 35))) {
                m_currentTab = i;
            }
            
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
    }

    bool LorerMenu::ToggleButton(const char* label, bool* v, float width) {
        ImGui::PushID(label);
        
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        float height = 20.0f;
        float radius = height * 0.50f;
        if (width <= 0.0f) width = ImGui::CalcItemWidth();
        
        ImVec2 labelSize = ImGui::CalcTextSize(label);
        float toggleWidth = 44.0f;
        
        ImGui::InvisibleButton("##toggle", ImVec2(toggleWidth, height));
        
        if (ImGui::IsItemClicked()) {
            *v = !(*v);
        }
        
        float t = *v ? 1.0f : 0.0f;
        static float animationProgress[256] = { 0 };
        static int buttonIds[256] = { 0 };
        int id = ImGui::GetID("##toggle");
        
        float animSpeed = 0.08f;
        float* progress = &animationProgress[id % 256];
        
        if (*v && *progress < 1.0f) {
            *progress += animSpeed;
            if (*progress > 1.0f) *progress = 1.0f;
        } else if (!*v && *progress > 0.0f) {
            *progress -= animSpeed;
            if (*progress < 0.0f) *progress = 0.0f;
        }
        
        t = *progress;
        
        ImU32 bgColor = *v ? ImColor(140, 50, 200, 255) : ImColor(45, 45, 55, 255);
        ImU32 circleColor = ImColor(255, 255, 255, 255);
        
        ImVec2 min = ImVec2(p.x, p.y);
        ImVec2 max = ImVec2(p.x + toggleWidth, p.y + height);
        
        drawList->AddRectFilled(min, max, bgColor, height * 0.5f);
        
        float circleX = p.x + radius + t * (toggleWidth - radius * 2.0f);
        ImVec2 circleCenter = ImVec2(circleX, p.y + radius);
        
        if (*v) {
            drawList->AddCircleFilled(circleCenter, radius - 2.0f, ImColor(180, 100, 255, 100), 24);
        }
        drawList->AddCircleFilled(circleCenter, radius - 2.0f, circleColor, 24);
        
        ImGui::SameLine(0, 10);
        ImGui::TextUnformatted(label);
        
        ImGui::PopID();
        
        return ImGui::IsItemClicked();
    }

    bool LorerMenu::ColorPicker(const char* label, Color& color, bool showAlpha) {
        ImVec4 col = color.ToImVec4();
        bool changed = ImGui::ColorEdit4(label, &col.x, 
            (showAlpha ? 0 : ImGuiColorEditFlags_NoAlpha) | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        if (changed) {
            color.r = col.x;
            color.g = col.y;
            color.b = col.z;
            color.a = col.w;
        }
        return changed;
    }

    void LorerMenu::HelpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void LorerMenu::SeparatorWithText(const char* text) {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.56f, 0.19f, 0.96f, 1.0f));
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Spacing();
    }

    void LorerMenu::RenderAimbotTab() {
        SeparatorWithText("Main Settings");
        
        ToggleButton("Enable Aimbot", &m_aimbot.enabled);
        
        if (m_aimbot.enabled) {
            ImGui::Indent(10);
            
            ImGui::PushItemWidth(200);
            ImGui::SliderFloat("FOV", &m_aimbot.fov, 0.1f, 360.0f, "%.1f°");
            ImGui::SameLine();
            HelpMarker("Field of View for target detection");
            
            ImGui::SliderFloat("Smoothness", &m_aimbot.smoothness, 1.0f, 30.0f, "%.1f");
            ImGui::SameLine();
            HelpMarker("Higher = smoother but slower aim");
            
            ImGui::SliderFloat("Aim Speed", &m_aimbot.aimSpeed, 0.1f, 10.0f, "%.1f");
            
            ImGui::PopItemWidth();
            
            ImGui::Combo("Target Bone", &m_aimbot.targetBone, m_bones, IM_ARRAYSIZE(m_bones));
            
            ImGui::Spacing();
            SeparatorWithText("Additional Options");
            
            ToggleButton("Visible Check", &m_aimbot.visibleCheck);
            ToggleButton("Silent Aim", &m_aimbot.silentAim);
            ToggleButton("Draw FOV Circle", &m_aimbot.drawFOV);
            
            if (m_aimbot.drawFOV) {
                ImGui::Indent(10);
                ImGui::ColorEdit4("FOV Color", &m_aimbot.fovColor.r, ImGuiColorEditFlags_NoInputs);
                ImGui::Unindent(10);
            }
            
            ImGui::Unindent(10);
        }
    }

    void LorerMenu::RenderVisualsTab() {
        SeparatorWithText("ESP Settings");
        
        ToggleButton("Enable ESP", &m_visuals.espEnabled);
        
        if (m_visuals.espEnabled) {
            ImGui::Indent(10);
            
            ToggleButton("Box ESP", &m_visuals.boxESP);
            ToggleButton("Health Bar", &m_visuals.healthESP);
            ToggleButton("Name ESP", &m_visuals.nameESP);
            ToggleButton("Distance ESP", &m_visuals.distanceESP);
            ToggleButton("Skeleton ESP", &m_visuals.skeletonESP);
            ToggleButton("Snaplines", &m_visuals.snaplines);
            ToggleButton("Head Dot", &m_visuals.headDot);
            ToggleButton("Weapon ESP", &m_visuals.weaponESP);
            
            ImGui::Spacing();
            
            if (m_visuals.snaplines) {
                ImGui::Combo("Snapline Position", &m_visuals.snaplinePosition, m_snaplinePositions, IM_ARRAYSIZE(m_snaplinePositions));
            }
            
            ImGui::Spacing();
            SeparatorWithText("Colors");
            
            ImGui::ColorEdit4("Enemy Color", &m_visuals.enemyColor.r, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Team Color", &m_visuals.teamColor.r, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Visible Color", &m_visuals.visibleColor.r, ImGuiColorEditFlags_NoInputs);
            
            ImGui::Unindent(10);
        }
    }

    void LorerMenu::RenderMiscTab() {
        SeparatorWithText("Movement");
        
        ToggleButton("Bunny Hop", &m_misc.bunnyHop);
        ToggleButton("Auto Strafe", &m_misc.autoStrafe);
        
        ImGui::Spacing();
        SeparatorWithText("Visual Effects");
        
        ToggleButton("No Flash", &m_misc.noFlash);
        ToggleButton("Radar Hack", &m_misc.radar);
        
        ImGui::Spacing();
        SeparatorWithText("FOV Changer");
        
        ToggleButton("Enable FOV Changer", &m_misc.fovChanger);
        
        if (m_misc.fovChanger) {
            ImGui::Indent(10);
            ImGui::PushItemWidth(200);
            ImGui::SliderInt("FOV Value", &m_misc.fovValue, 70, 140, "%d°");
            ImGui::PopItemWidth();
            ImGui::Unindent(10);
        }
        
        ImGui::Spacing();
        SeparatorWithText("Advanced");
        
        ToggleButton("Fake Lag", &m_misc.fakeLag);
        
        if (m_misc.fakeLag) {
            ImGui::Indent(10);
            ImGui::PushItemWidth(200);
            ImGui::SliderInt("Lag Amount", &m_misc.fakeLagAmount, 1, 16, "%d ticks");
            ImGui::PopItemWidth();
            ImGui::Unindent(10);
        }
    }

    void LorerMenu::RenderSettingsTab() {
        SeparatorWithText("Menu Settings");
        
        ImGui::Combo("Menu Hotkey", &m_settings.menuKey, "INSERT\0HOME\0END\0F1\0F2\0F3\0\0");
        
        ToggleButton("Show Watermark", &m_settings.showWatermark);
        ToggleButton("Stream Proof", &m_settings.streamProof);
        
        ImGui::Spacing();
        SeparatorWithText("Updates");
        
        ToggleButton("Auto-Update Offsets", &m_settings.autoUpdateOffsets);
        ToggleButton("Save Config on Exit", &m_settings.saveOnExit);
        
        ImGui::Spacing();
        SeparatorWithText("Configuration");
        
        ImGui::PushItemWidth(150);
        ImGui::InputText("Config Name", m_settings.configName, sizeof(m_settings.configName));
        ImGui::PopItemWidth();
        
        ImGui::Spacing();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.45f, 1.0f));
        
        if (ImGui::Button("Save Config", ImVec2(120, 30))) {
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Config", ImVec2(120, 30))) {
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Defaults", ImVec2(120, 30))) {
            m_aimbot = AimbotConfig();
            m_visuals = VisualsConfig();
            m_misc = MiscConfig();
        }
        
        ImGui::PopStyleColor(2);
    }

    void LorerMenu::RenderFooter() {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 25);
        ImGui::SetCursorPosX(10);
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.6f, 1.0f));
        ImGui::TextUnformatted("Lorer v1.0 | Made with ");
        ImGui::SameLine(0, 0);
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::TextUnformatted("<3");
        ImGui::PopStyleColor();
        
        ImGui::SameLine(0, 3);
        ImGui::TextUnformatted(" | Standoff 2 v0.37.1");
        ImGui::PopStyleColor();
    }

}
