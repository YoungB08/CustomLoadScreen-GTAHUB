#include "loader.h"
#include "CustomLoadScreen.h"
#include "d3d9/proxydirectx.h"
#include <filesystem>
#include <string>

stGlobalHandles g_handle;
stGlobalClasses g_class;
stGlobalPVars g_vars;

static WNDPROC hOrigProc = NULL;
static CustomLoadScreen *pCustomLoadScreen = static_cast<CustomLoadScreen*>(nullptr);
static proxyIDirect3DDevice9* device = nullptr;

#include <cstdio>
#include <cstdarg>

void Log(const char* fmt, ...)
{
    static FILE* f = fopen("CustomLoadScreen.log", "w");
    if (!f) f = fopen("CustomLoadScreen.log", "a");
    if (!f) return;

    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fflush(f);
}

void __stdcall InstallD3DHook()
{
    IDirect3DDevice9* realDev = *reinterpret_cast<IDirect3DDevice9 **>(0xC97C28);
    if (realDev != nullptr && realDev != static_cast<IDirect3DDevice9*>(device))
    {
        Log("[HOOK] Installing D3D9 device proxy. realDev=%p", realDev);
        device = new proxyIDirect3DDevice9(realDev);
        *reinterpret_cast<IDirect3DDevice9 **>(0xC97C28) = static_cast<IDirect3DDevice9*>(device);
        g_class.d3d = *reinterpret_cast<IDirect3D9**>(0xC97C20);
        if (!g_class.DirectX)
            g_class.DirectX = new CDirectX(device);
        else
            g_class.DirectX->setDevice(device);

        if (!pCustomLoadScreen) {
            Log("[HOOK] Creating CustomLoadScreen instance");
            pCustomLoadScreen = new CustomLoadScreen();
        }

        if (pCustomLoadScreen && g_class.DirectX)
        {
            Log("[HOOK] Setting Present callback");
            g_class.DirectX->SetPresentCallback(
                [](const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride,
                       const RGNDATA *pDirtyRegion) {
                if (pCustomLoadScreen)
                    return pCustomLoadScreen->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
                return D3D_OK;
            });
        }
    }
}

LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (hwnd == *(HWND*)0xC97C1C)
    {
        for ( int i = static_cast<int>(MenuList.size() - 1); i >= 0; --i )
            if ( MenuList[static_cast<size_t>(i)]->isShowed() )
                if ( !MenuList[static_cast<size_t>(i)]->onEvents( hwnd, uMsg, wParam, lParam ) )
                    break;

        if (pCustomLoadScreen && !pCustomLoadScreen->Event(uMsg, wParam, lParam))
            return 0;
    }
    if (hOrigProc != NULL)
        return CallWindowProc(hOrigProc, hwnd, uMsg, wParam, lParam);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void __stdcall GameLoop()
{
    g_handle.d3d9 = GetModuleHandleA("d3d9.dll");
    if (g_handle.d3d9 == nullptr || g_handle.d3d9 == INVALID_HANDLE)
        return;

    InstallD3DHook();

    static bool hooked = false;
    if (!hooked && g_vars.hwnd != NULL) {
        WNDPROC oldProc = reinterpret_cast<WNDPROC>(SetWindowLongA(g_vars.hwnd, GWL_WNDPROC,
                                                             reinterpret_cast<LONG>(WndProc)));
        if (oldProc != NULL) {
            hOrigProc = oldProc;
            hooked = true;
        }
    }

    if (pCustomLoadScreen)
        pCustomLoadScreen->Loop();
}

void __stdcall WindowInitialize() //007455DB
{
    g_handle.d3d9 = GetModuleHandleA("d3d9.dll");
    if (!pCustomLoadScreen)
        pCustomLoadScreen = new CustomLoadScreen();
    InstallD3DHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID)
{
    static CCallHook *gameloopHook = nullptr;

    if (dwReasonForCall == DLL_PROCESS_ATTACH){
        Log("[INIT] CustomLoadScreen.asi attached to process");

        if (sizeof(CPed) != 1988){
            Log("[ERROR] Incorrect CPed size");
            MessageBox("Incorrect CPed == " + std::to_string(sizeof(CPed)), PROJECT_NAME, MB_OK);
            return FALSE;
        }
        if (sizeof(CVehicle) != 2584){
            Log("[ERROR] Incorrect CVehicle size");
            MessageBox("Incorrect CVehicle == " + std::to_string(sizeof(CVehicle)), PROJECT_NAME, MB_OK);
            return FALSE;
        }

        Log("[INIT] Installing GameLoop hook at 0x00748DA3");
        gameloopHook = new CCallHook(reinterpret_cast<void*>(0x00748DA3),
                                     eSafeCall(sc_registers | sc_flags), 5);
        gameloopHook->enable(GameLoop);
        Log("[INIT] GameLoop hook enabled successfully");
    }
    else if (dwReasonForCall == DLL_PROCESS_DETACH){
        Log("[EXIT] Detaching CustomLoadScreen.asi");
        delete gameloopHook;
        gameloopHook = nullptr;
        SetWindowLongA(g_vars.hwnd, GWL_WNDPROC, reinterpret_cast<LONG>(hOrigProc));
        delete pCustomLoadScreen;
        pCustomLoadScreen = nullptr;
        delete device;
        device = nullptr;
        delete g_class.DirectX;
        g_class.DirectX = nullptr;
    }

    return TRUE;
}

int MessageBox(const std::string& text, const std::string& title, UINT type)
{
    return MessageBoxA(g_vars.hwnd, text.c_str(), title.c_str(), type);
}
