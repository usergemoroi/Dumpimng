/*
 * Lorer Menu Integration Example
 * 
 * This file demonstrates how to integrate the Lorer menu
 * into a DirectX 11 hook for the Standoff 2 project.
 * 
 * Usage:
 * 1. Include this header in your DLL main file
 * 2. Call LorerIntegration::Initialize() in your DllMain
 * 3. Call LorerIntegration::OnPresent() from your Present hook
 * 4. Call LorerIntegration::HandleInput() from your WndProc hook
 */

#pragma once
#include "LorerOverlay.h"
#include <d3d11.h>
#include <MinHook.h>

namespace LorerIntegration {

    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_KEYDOWN && wParam == VK_INSERT) {
            Lorer::LorerOverlay::GetInstance().GetMenu().ToggleMenu();
        }

        if (Lorer::LorerOverlay::GetInstance().GetMenu().IsMenuOpen()) {
            ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
            return true;
        }

        return CallWindowProcW((WNDPROC)g_originalWndProc, hWnd, msg, wParam, lParam);
    }

    static LONG_PTR g_originalWndProc = 0;
    static bool g_initialized = false;

    bool Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext) {
        if (g_initialized) return true;

        if (!Lorer::LorerOverlay::GetInstance().Initialize(hWnd, pDevice, pContext)) {
            return false;
        }

        g_originalWndProc = SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

        g_initialized = true;
        return true;
    }

    void OnPresent(IDXGISwapChain* pSwapChain) {
        if (!g_initialized) {
            ID3D11Device* pDevice = nullptr;
            if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
                ID3D11DeviceContext* pContext = nullptr;
                pDevice->GetImmediateContext(&pContext);

                DXGI_SWAP_CHAIN_DESC desc;
                pSwapChain->GetDesc(&desc);
                
                Initialize(desc.OutputWindow, pDevice, pContext);
                
                pDevice->Release();
            }
        }

        Lorer::LorerOverlay::GetInstance().Render();
    }

    void Shutdown() {
        Lorer::LorerOverlay::GetInstance().Shutdown();
        g_initialized = false;
    }

    // Utility functions for cheat features
    namespace Features {

        bool IsAimbotEnabled() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetAimbotConfig().enabled;
        }

        float GetAimbotFOV() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetAimbotConfig().fov;
        }

        float GetAimbotSmoothness() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetAimbotConfig().smoothness;
        }

        int GetTargetBone() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetAimbotConfig().targetBone;
        }

        bool IsESPEnabled() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetVisualsConfig().espEnabled;
        }

        const Lorer::VisualsConfig& GetESPConfig() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetVisualsConfig();
        }

        bool IsBunnyHopEnabled() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetMiscConfig().bunnyHop;
        }

        bool IsNoFlashEnabled() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetMiscConfig().noFlash;
        }

        int GetFOVValue() {
            return Lorer::LorerOverlay::GetInstance().GetMenu().GetMiscConfig().fovValue;
        }

    }

}

/*
 * Example integration in DllMain:
 * 
 * // Hook function for Present
 * HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
 *     LorerIntegration::OnPresent(pSwapChain);
 *     return oPresent(pSwapChain, SyncInterval, Flags);
 * }
 * 
 * BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
 *     switch (ul_reason_for_call) {
 *     case DLL_PROCESS_ATTACH:
 *         DisableThreadLibraryCalls(hModule);
 *         CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
 *             // Install DirectX hooks...
 *             // ...
 *             return 0;
 *         }, nullptr, 0, nullptr);
 *         break;
 *     case DLL_PROCESS_DETACH:
 *         LorerIntegration::Shutdown();
 *         break;
 *     }
 *     return TRUE;
 * }
 */
