/*
 * Lorer Loader - DLL Injection Tool
 * For Standoff 2 on BlueStacks 5
 */

#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <vector>
#include <shlwapi.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")

#define TARGET_PROCESS L"HD-Player.exe"
#define DLL_NAME L"Lorer.dll"

namespace Utils {

    DWORD GetProcessIdByName(const std::wstring& processName) {
        DWORD pid = 0;
        PROCESSENTRY32W pe32 = { sizeof(PROCESSENTRY32W) };
        
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

        if (Process32FirstW(hSnapshot, &pe32)) {
            do {
                if (_wcsicmp(pe32.szExeFile, processName.c_str()) == 0) {
                    pid = pe32.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
        return pid;
    }

    bool EnableDebugPrivilege() {
        HANDLE hToken;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            return false;
        }

        TOKEN_PRIVILEGES tp;
        LUID luid;
        if (!LookupPrivilegeValueW(nullptr, L"SeDebugPrivilege", &luid)) {
            CloseHandle(hToken);
            return false;
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        bool result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr);
        CloseHandle(hToken);
        
        return result && (GetLastError() == ERROR_SUCCESS);
    }

    bool InjectDLL(DWORD pid, const std::wstring& dllPath) {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!hProcess) {
            std::wcerr << L"[!] Failed to open process. Error: " << GetLastError() << std::endl;
            return false;
        }

        size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
        LPVOID pRemoteMem = VirtualAllocEx(hProcess, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!pRemoteMem) {
            std::wcerr << L"[!] Failed to allocate remote memory. Error: " << GetLastError() << std::endl;
            CloseHandle(hProcess);
            return false;
        }

        if (!WriteProcessMemory(hProcess, pRemoteMem, dllPath.c_str(), pathSize, nullptr)) {
            std::wcerr << L"[!] Failed to write to remote memory. Error: " << GetLastError() << std::endl;
            VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        LPTHREAD_START_ROUTINE pLoadLibraryW = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
        if (!pLoadLibraryW) {
            std::wcerr << L"[!] Failed to get LoadLibraryW address." << std::endl;
            VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        HANDLE hRemoteThread = CreateRemoteThread(hProcess, nullptr, 0, pLoadLibraryW, pRemoteMem, 0, nullptr);
        if (!hRemoteThread) {
            std::wcerr << L"[!] Failed to create remote thread. Error: " << GetLastError() << std::endl;
            VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        WaitForSingleObject(hRemoteThread, 5000);
        
        DWORD exitCode = 0;
        GetExitCodeThread(hRemoteThread, &exitCode);

        CloseHandle(hRemoteThread);
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);

        return exitCode != 0;
    }

    bool IsInjected(DWORD pid, const std::wstring& dllName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;

        MODULEENTRY32W me32 = { sizeof(MODULEENTRY32W) };
        bool found = false;

        if (Module32FirstW(hSnapshot, &me32)) {
            do {
                if (wcsstr(me32.szExePath, dllName.c_str())) {
                    found = true;
                    break;
                }
            } while (Module32NextW(hSnapshot, &me32));
        }

        CloseHandle(hSnapshot);
        return found;
    }

    void PrintBanner() {
        std::wcout << std::endl;
        std::wcout << L"  ╔══════════════════════════════════════════╗" << std::endl;
        std::wcout << L"  ║                                          ║" << std::endl;
        std::wcout << L"  ║   ██╗     ███████╗██████╗ ██████╗ ██████╗ ║" << std::endl;
        std::wcout << L"  ║   ██║     ██╔════╝██╔══██╗██╔══██╗██╔══██╗║" << std::endl;
        std::wcout << L"  ║   ██║     █████╗  ██████╔╝██████╔╝██║  ██║║" << std::endl;
        std::wcout << L"  ║   ██║     ██╔══╝  ██╔══██╗██╔══██╗██║  ██║║" << std::endl;
        std::wcout << L"  ║   ███████╗███████╗██║  ██║██║  ██║██████╔╝║" << std::endl;
        std::wcout << L"  ║   ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝ ║" << std::endl;
        std::wcout << L"  ║                                          ║" << std::endl;
        std::wcout << L"  ║   Standoff 2 Cheat - v1.0                ║" << std::endl;
        std::wcout << L"  ║   Target: BlueStacks 5 (HD-Player.exe)   ║" << std::endl;
        std::wcout << L"  ║                                          ║" << std::endl;
        std::wcout << L"  ╚══════════════════════════════════════════╝" << std::endl;
        std::wcout << std::endl;
    }

    void PrintStatus(const std::wstring& message, bool success) {
        if (success) {
            std::wcout << L"[\x1b[92m+\x1b[0m] " << message << std::endl;
        } else {
            std::wcout << L"[\x1b[91m-\x1b[0m] " << message << std::endl;
        }
    }

    void PrintInfo(const std::wstring& message) {
        std::wcout << L"[\x1b[94m*\x1b[0m] " << message << std::endl;
    }

    void PrintWarning(const std::wstring& message) {
        std::wcout << L"[\x1b[93m!\x1b[0m] " << message << std::endl;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Allocate console for output
    AllocConsole();
    FILE* pFile;
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    freopen_s(&pFile, "CONOUT$", "w", stderr);
    freopen_s(&pFile, "CONIN$", "r", stdin);
    
    SetConsoleTitleW(L"Lorer Loader v1.0");

    Utils::PrintBanner();

    // Enable debug privileges
    Utils::PrintInfo(L"Enabling debug privileges...");
    if (Utils::EnableDebugPrivilege()) {
        Utils::PrintStatus(L"Debug privileges enabled", true);
    } else {
        Utils::PrintWarning(L"Failed to enable debug privileges (may need admin rights)");
    }

    // Get DLL path
    wchar_t dllPath[MAX_PATH] = {0};
    GetModuleFileNameW(nullptr, dllPath, MAX_PATH);
    PathRemoveFileSpecW(dllPath);
    wcscat_s(dllPath, L"\\");
    wcscat_s(dllPath, DLL_NAME);

    Utils::PrintInfo(L"DLL Path: " + std::wstring(dllPath));

    // Check if DLL exists
    if (!PathFileExistsW(dllPath)) {
        Utils::PrintStatus(L"DLL not found: " + std::wstring(DLL_NAME), false);
        Utils::PrintInfo(L"Press Enter to exit...");
        std::cin.get();
        FreeConsole();
        return 1;
    }
    Utils::PrintStatus(L"DLL found", true);

    // Find target process
    Utils::PrintInfo(L"Searching for " + std::wstring(TARGET_PROCESS) + L"...");
    DWORD pid = Utils::GetProcessIdByName(TARGET_PROCESS);
    
    if (!pid) {
        Utils::PrintStatus(L"Target process not found", false);
        Utils::PrintInfo(L"Make sure BlueStacks 5 is running with Standoff 2");
        Utils::PrintInfo(L"Press Enter to exit...");
        std::cin.get();
        FreeConsole();
        return 1;
    }
    
    Utils::PrintStatus(L"Found target process (PID: " + std::to_wstring(pid) + L")", true);

    // Check if already injected
    if (Utils::IsInjected(pid, DLL_NAME)) {
        Utils::PrintWarning(L"DLL is already injected!");
        Utils::PrintInfo(L"Press Enter to exit...");
        std::cin.get();
        FreeConsole();
        return 0;
    }

    // Inject DLL
    Utils::PrintInfo(L"Injecting DLL...");
    if (Utils::InjectDLL(pid, dllPath)) {
        Utils::PrintStatus(L"DLL injected successfully!", true);
        Utils::PrintInfo(L"");
        Utils::PrintInfo(L"═══════════════════════════════════════════");
        Utils::PrintInfo(L"  Press INSERT to toggle menu");
        Utils::PrintInfo(L"  Use mouse to navigate menu");
        Utils::PrintInfo(L"═══════════════════════════════════════════");
        Utils::PrintInfo(L"");
    } else {
        Utils::PrintStatus(L"DLL injection failed!", false);
        Utils::PrintInfo(L"Try running as Administrator");
    }

    Utils::PrintInfo(L"Press Enter to exit...");
    std::cin.get();

    FreeConsole();
    return 0;
}
