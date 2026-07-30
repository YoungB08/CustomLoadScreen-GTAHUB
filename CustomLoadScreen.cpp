#include "CustomLoadScreen.h"
#include "loader.h"

typedef void (*cef_create_browser_t)(int id, const char* url, bool is_local, bool focused);
typedef void (*cef_set_visibility_t)(int id, bool visible);
typedef void (*cef_destroy_browser_t)(int id);
typedef void (*cef_execute_js_t)(int id, const char* js);

static cef_create_browser_t fn_cef_create_browser = nullptr;
static cef_set_visibility_t fn_cef_set_visibility = nullptr;
static cef_destroy_browser_t fn_cef_destroy_browser = nullptr;
static cef_execute_js_t fn_cef_execute_js = nullptr;
static bool cefChecked = false;
static bool cefActive = false;
static const int BROWSER_ID = 9988;

static void InitCEF()
{
    if (cefChecked) return;
    cefChecked = true;

    HMODULE hCEF = GetModuleHandleA("cef.dll");
    if (!hCEF) hCEF = GetModuleHandleA("cef.asi");
    if (!hCEF) hCEF = GetModuleHandleA("cef_browser.dll");

    if (hCEF)
    {
        fn_cef_create_browser = (cef_create_browser_t)GetProcAddress(hCEF, "create_browser");
        if (!fn_cef_create_browser) fn_cef_create_browser = (cef_create_browser_t)GetProcAddress(hCEF, "cef_create_browser");

        fn_cef_set_visibility = (cef_set_visibility_t)GetProcAddress(hCEF, "set_browser_visibility");
        if (!fn_cef_set_visibility) fn_cef_set_visibility = (cef_set_visibility_t)GetProcAddress(hCEF, "cef_set_visibility");

        fn_cef_destroy_browser = (cef_destroy_browser_t)GetProcAddress(hCEF, "destroy_browser");
        if (!fn_cef_destroy_browser) fn_cef_destroy_browser = (cef_destroy_browser_t)GetProcAddress(hCEF, "cef_destroy_browser");

        fn_cef_execute_js = (cef_execute_js_t)GetProcAddress(hCEF, "execute_js");
        if (!fn_cef_execute_js) fn_cef_execute_js = (cef_execute_js_t)GetProcAddress(hCEF, "cef_execute_js");

        if (fn_cef_create_browser)
        {
            Log("[CEF] Found CEF API in module %p. Loading gtahub.html...", hCEF);
            fn_cef_create_browser(BROWSER_ID, "gtahub.html", true, false);
            cefActive = true;
        }
    }
}

CustomLoadScreen::CustomLoadScreen()
{
    // Constructor
    if (g_class.DirectX != nullptr) {
        g_class.DirectX->SetPresentCallback(
                    [this](const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride,
                           const RGNDATA *pDirtyRegion) {
            return Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
        });
    }
}

CustomLoadScreen::~CustomLoadScreen()
{
    // Destructor
    if (cefActive && fn_cef_destroy_browser) {
        fn_cef_destroy_browser(BROWSER_ID);
        cefActive = false;
    }
    if (g_class.DirectX != nullptr) {
        g_class.DirectX->ClearPresentCallback();
        if (pFont != nullptr)
            g_class.DirectX->d3d9_ReleaseFont(pFont);
        if (pTexture != nullptr)
            g_class.DirectX->d3d9_ReleaseTexture(pTexture);
    }
}

void CustomLoadScreen::Loop()
{
    // Main loop
}

bool CustomLoadScreen::Event(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Events
    return true;
}

#include "loader.h"

HRESULT CustomLoadScreen::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
    if ((g_vars.gameSatate >= 7 && GetModuleHandleA("samp.dll") == 0) || g_vars.gameSatate == 9)
    {
        if (cefActive && fn_cef_set_visibility) {
            fn_cef_set_visibility(BROWSER_ID, false);
        }
        return D3D_OK;
    }

    if (!g_class.DirectX || !g_class.DirectX->d3d9_device())
        return D3D_OK;

    InitCEF();

    int screenWidth = *(int*)0x00C9C040;
    int screenHeight = *(int*)0x00C9C044;
    if (screenWidth <= 0) screenWidth = 800;
    if (screenHeight <= 0) screenHeight = 600;

    // Calculate progress percentage
    int percentage = static_cast<int>(11.111111 * g_vars.gameSatate);
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;

    if (cefActive && fn_cef_execute_js)
    {
        std::string js = "if(window.setProgress) setProgress(" + std::to_string(percentage) + ");";
        fn_cef_execute_js(BROWSER_ID, js.c_str());
    }

    if (!init){
        Log("[PRESENT] First frame Present. Creating Font and Texture (%dx%d)", screenWidth, screenHeight);
        pFont = g_class.DirectX->d3d9_CreateFont("Arial", 11, 5);
        pTexture = g_class.DirectX->d3d9_CreateTexture(screenWidth, screenHeight);
        if (pTexture) {
            Log("[PRESENT] Loading CustomLoadScreen.png texture");
            pTexture->Load("CustomLoadScreen.png");
        }
        init = true;
        Log("[PRESENT] Initialization complete");
    }

    if (!init || !pTexture || !pFont)
        return D3D_OK;

    // Render
    pTexture->Begin();
    pTexture->Clear(eCdBlack);

    // Draw stylized text with HTML Hex color tags
    pFont->PrintShadow(20, 20, -1, "{FF3333}GTAHUB {FFFFFF}| Roleplay Server");
    pFont->PrintShadow(20, 42, -1, "{AAAAAA}Status: {FFFFFF}Loading game assets... {FFD700}" + std::to_string(percentage) + "%");

    pTexture->End();
    pTexture->Render(0, 0, screenWidth, screenHeight); // Draw texture

    return D3D_OK;
}
