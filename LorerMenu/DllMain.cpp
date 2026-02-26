/*
 * Lorer - Standoff 2 Cheat
 * Main DLL Entry Point
 * 
 * Target: Standoff 2 v0.37.1 on BlueStacks 5
 * Graphics: DirectX 11
 */

#include <windows.h>
#include <d3d11.h>
#include <MinHook.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "MemoryManager.h"
#include "CheatFeatures.h"
#include "LorerMenu.h"
#include "Offsets.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "MinHook.lib")

// Global instances
MemoryManager g_memory;
CheatFeatures g_cheats;
Lorer::LorerMenu g_menu;

// DirectX globals
ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
HWND g_hWindow = nullptr;
bool g_initialized = false;

// Original function pointers
typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present_t oPresent = nullptr;

typedef LRESULT(CALLBACK* WndProc_t)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
WndProc_t oWndProc = nullptr;

// Forward declarations
bool InitializeImGui();
void ShutdownImGui();
LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Window procedure hook
LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Toggle menu on INSERT key
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
        g_menu.ToggleMenu();
    }
    
    // Pass input to ImGui when menu is open
    if (g_menu.IsMenuOpen()) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return true;
    }
    
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

// Present hook
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    // Initialize on first call
    if (!g_initialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pDevice))) {
            g_pDevice->GetImmediateContext(&g_pContext);
            
            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            g_hWindow = desc.OutputWindow;
            
            // Initialize ImGui
            if (InitializeImGui()) {
                // Initialize memory manager
                if (g_memory.Initialize()) {
                    // Initialize cheat features
                    g_cheats.Initialize(&g_memory, &g_menu);
                    
                    // Initialize menu
                    g_menu.Initialize();
                    
                    g_initialized = true;
                }
            }
        }
    }
    
    if (g_initialized) {
        // Begin ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // Update cheat logic
        g_cheats.Update();
        
        // Render ESP on background draw list
        ImDrawList* bgDrawList = ImGui::GetBackgroundDrawList();
        g_cheats.RenderESP(bgDrawList);
        
        // Render menu
        g_menu.Render();
        
        // Render ImGui
        ImGui::Render();
        g_pContext->OMSetRenderTargets(1, nullptr, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    
    return oPresent(pSwapChain, SyncInterval, Flags);
}

bool InitializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    
    // Hook window procedure
    oWndProc = (WndProc_t)SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
    
    // Initialize backends
    if (!ImGui_ImplWin32_Init(g_hWindow)) return false;
    if (!ImGui_ImplDX11_Init(g_pDevice, g_pContext)) return false;
    
    return true;
}

void ShutdownImGui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    // Restore window procedure
    if (oWndProc && g_hWindow) {
        SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, (LONG_PTR)oWndProc);
    }
}

// Create temporary swap chain to get Present address
bool HookDirectX() {
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, DefWindowProcW, 0L, 0L, 
                       GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, 
                       L"TempWindow", nullptr };
    RegisterClassExW(&wc);
    
    HWND tempWnd = CreateWindowExW(0, wc.lpszClassName, L"Temp", 
                                    WS_OVERLAPPEDWINDOW, 
                                    100, 100, 300, 300, 
                                    nullptr, nullptr, wc.hInstance, nullptr);
    
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = tempWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    ID3D11Device* pTempDevice = nullptr;
    ID3D11DeviceContext* pTempContext = nullptr;
    IDXGISwapChain* pTempSwapChain = nullptr;
    
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &pTempSwapChain, &pTempDevice, &featureLevel, &pTempContext
    );
    
    if (FAILED(hr)) {
        DestroyWindow(tempWnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return false;
    }
    
    // Get Present from vtable
    void** pVTable = *reinterpret_cast<void***>(pTempSwapChain);
    oPresent = (Present_t)pVTable[8];
    
    // Release temp objects
    pTempSwapChain->Release();
    pTempDevice->Release();
    pTempContext->Release();
    
    DestroyWindow(tempWnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    
    // Create hook
    if (MH_CreateHook(reinterpret_cast<LPVOID>(oPresent), 
                       reinterpret_cast<LPVOID>(&hkPresent), 
                       reinterpret_cast<LPVOID*>(&oPresent)) != MH_OK) {
        return false;
    }
    
    if (MH_EnableHook(reinterpret_cast<LPVOID>(oPresent)) != MH_OK) {
        return false;
    }
    
    return true;
}

void UnhookDirectX() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            
            // Initialize MinHook
            if (MH_Initialize() != MH_OK) {
                return FALSE;
            }
            
            // Hook DirectX Present
            if (!HookDirectX()) {
                MH_Uninitialize();
                return FALSE;
            }
            
            // Create worker thread
            CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
                // Wait for game to initialize
                Sleep(3000);
                
                // Main loop (optional - most work is done in Present hook)
                while (true) {
                    Sleep(100);
                }
                return 0;
            }, nullptr, 0, nullptr);
            
            break;
        }
        
        case DLL_PROCESS_DETACH: {
            ShutdownImGui();
            UnhookDirectX();
            break;
        }
    }
    
    return TRUE;
}
