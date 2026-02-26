#pragma once
#include "LorerMenu.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>

namespace Lorer {

    class LorerOverlay {
    public:
        static LorerOverlay& GetInstance() {
            static LorerOverlay instance;
            return instance;
        }

        bool Initialize(HWND hWnd, ID3D11Device* pDevice, ID3d11DeviceContext* pContext) {
            if (m_initialized) return true;

            m_hWnd = hWnd;
            m_pDevice = pDevice;
            m_pContext = pContext;

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.IniFilename = nullptr;
            io.LogFilename = nullptr;

            ImGui::StyleColorsDark();

            if (!ImGui_ImplWin32_Init(hWnd)) return false;
            if (!ImGui_ImplDX11_Init(pDevice, pContext)) return false;

            m_menu.Initialize();
            m_initialized = true;
            return true;
        }

        void Shutdown() {
            if (!m_initialized) return;
            
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            m_initialized = false;
        }

        void BeginFrame() {
            if (!m_initialized) return;
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
        }

        void EndFrame() {
            if (!m_initialized) return;
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }

        void Render() {
            if (!m_initialized) return;

            BeginFrame();

            if (m_menu.IsMenuOpen()) {
                m_menu.Render();
            }

            if (m_menu.GetSettingsConfig().showWatermark && !m_menu.IsMenuOpen()) {
                RenderWatermark();
            }

            RenderESP();

            EndFrame();
        }

        void HandleInput(UINT msg, WPARAM wParam, LPARAM lParam) {
            if (msg == WM_KEYDOWN && wParam == VK_INSERT) {
                m_menu.ToggleMenu();
            }
        }

        LorerMenu& GetMenu() { return m_menu; }

    private:
        LorerOverlay() = default;
        ~LorerOverlay() = default;
        LorerOverlay(const LorerOverlay&) = delete;
        LorerOverlay& operator=(const LorerOverlay&) = delete;

        void RenderWatermark() {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(150, 30), ImGuiCond_Always);
            
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.12f, 0.8f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.56f, 0.19f, 0.96f, 0.8f));
            
            if (ImGui::Begin("##Watermark", nullptr, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoInputs)) {
                
                ImGui::TextColored(ImVec4(0.56f, 0.19f, 0.96f, 1.0f), "LORER");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| v1.0");
            }
            ImGui::End();
            
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(2);
        }

        void RenderESP() {
            if (!m_menu.GetVisualsConfig().espEnabled) return;

            ImGui::GetBackgroundDrawList();

            // ESP rendering would go here
            // This is a placeholder for the actual ESP implementation
            // that would read game memory and draw boxes/lines
        }

        bool m_initialized = false;
        HWND m_hWnd = nullptr;
        ID3D11Device* m_pDevice = nullptr;
        ID3D11DeviceContext* m_pContext = nullptr;
        LorerMenu m_menu;
    };

}