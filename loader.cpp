#include "loader.h"
#include "CustomLoadScreen.h"
#include "d3d9/proxydirectx.h"
#include <QFileInfo>

stGlobalHandles g_handle;
stGlobalClasses g_class;
stGlobalPVars g_vars;

static WNDPROC hOrigProc = NULL;
static CustomLoadScreen *pCustomLoadScreen = static_cast<CustomLoadScreen*>(nullptr);
static IDirect3DDevice9* device;

void __stdcall InstallD3DHook()
{
    device = new proxyIDirect3DDevice9( *reinterpret_cast<IDirect3DDevice9 **>(0xC97C28));
    *reinterpret_cast<IDirect3DDevice9 **>(0xC97C28) = dynamic_cast<IDirect3DDevice9*>(device);
    g_class.d3d = *reinterpret_cast<IDirect3D9**>(0xC97C20);
    g_class.DirectX = new CDirectX(device);
}

LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (hwnd == *(HWND*)0xC97C1C)
    {
        for ( int i = static_cast<int>(MenuList.size() - 1); i >= 0; --i )
            if ( MenuList[static_cast<size_t>(i)]->isShowed() )
                if ( !MenuList[static_cast<size_t>(i)]->onEvents( hwnd, uMsg, wParam, lParam ) )
                    break;

        if (!pCustomLoadScreen->Event(uMsg, wParam, lParam))
            return 0;
    }
    return CallWindowProc(hOrigProc, hwnd, uMsg, wParam, lParam);
}

void __stdcall GameLoop()
{
    // g_handle.samp = GetModuleHandleA("samp.dll");
    // if (g_handle.samp == nullptr || g_handle.samp == INVALID_HANDLE)
    // 	return;

    g_handle.d3d9 = GetModuleHandleA("d3d9.dll");
    if (g_handle.d3d9 == nullptr || g_handle.d3d9 == INVALID_HANDLE)
        return;

    // g_class.samp = *reinterpret_cast<stSAMP**>(g_handle.dwSAMP + SAMP_INFO_OFFSET);
    // if (g_class.samp == nullptr)
    // 	return;

    static bool hooked = false;
    if (hooked)
        return pCustomLoadScreen->Loop();
    hooked = true;

//    InstallD3DHook();
//    pCustomLoadScreen = new CustomLoadScreen();
    hOrigProc = reinterpret_cast<WNDPROC>(SetWindowLongA(g_vars.hwnd, GWL_WNDPROC,
                                                         reinterpret_cast<LONG>(WndProc)));
}

void __stdcall WindowInitialize() //007455DB
{
    g_handle.d3d9 = GetModuleHandleA("d3d9.dll");
//    GameLoop();
    InstallD3DHook();
    pCustomLoadScreen = new CustomLoadScreen();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID)
{
    static CCallHook *gameloopHook;
    static CCallHook *windowInitHook;

    if (dwReasonForCall == DLL_PROCESS_ATTACH){

        if (sizeof(CPed) != 1988){
            MessageBox("Incorrect CPed == " + QString::number(sizeof(CPed)), PROJECT_NAME, MB_OK);
            return FALSE;
        }
        if (sizeof(CVehicle) != 2584){
            MessageBox("Incorrect CVehicle == " + QString::number(sizeof(CVehicle)), PROJECT_NAME, MB_OK);
            return FALSE;
        }
        if (GetModuleHandleA((QString(PROJECT_NAME) + ".asi").toStdString().c_str()) != hModule){
            char name[256];
            GetModuleFileNameA(hModule, name, 256);
            MessageBox("Incorrect file name.\n"
                       "Please rename " + QFileInfo(name).fileName() +
                       " to " + QString(PROJECT_NAME) + ".asi",
                       PROJECT_NAME, MB_OK);
            return FALSE;
        }


        windowInitHook = new CCallHook(reinterpret_cast<void*>(0x007F67C1),
                                       eSafeCall(sc_registers | sc_flags), 5);
        windowInitHook->enable(WindowInitialize);

        gameloopHook = new CCallHook(reinterpret_cast<void*>(0x00748DA3),
                                     eSafeCall(sc_registers | sc_flags), 6);
        gameloopHook->enable(GameLoop);
    }
    else if (dwReasonForCall == DLL_PROCESS_DETACH){
        delete windowInitHook;
        delete gameloopHook;
        SetWindowLongA(g_vars.hwnd, GWL_WNDPROC, reinterpret_cast<LONG>(hOrigProc));
        delete pCustomLoadScreen;
        pCustomLoadScreen = nullptr;
        delete dynamic_cast<proxyIDirect3DDevice9*>(device);
        delete g_class.DirectX;
    }

    return TRUE;
}

int MessageBox(QString text, QString title, UINT type)
{
    return MessageBoxA(g_vars.hwnd, text.toStdString().c_str(), title.toStdString().c_str(), type);
}
