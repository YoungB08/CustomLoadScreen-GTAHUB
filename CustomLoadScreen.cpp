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

static const char GTAHUB_EMBEDDED_HTML[] = R"html(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>GTAHUB Load Screen</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            user-select: none;
        }

        body {
            width: 100vw;
            height: 100vh;
            overflow: hidden;
            background: transparent;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            color: #ffffff;
        }

        .container {
            position: absolute;
            bottom: 40px;
            left: 50%;
            transform: translateX(-50%);
            width: 80%;
            max-width: 900px;
            background: rgba(15, 15, 20, 0.85);
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 50, 50, 0.3);
            border-radius: 12px;
            padding: 24px 32px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.8), 0 0 20px rgba(255, 50, 50, 0.2);
            display: flex;
            flex-direction: column;
            gap: 14px;
        }

        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .title {
            font-size: 24px;
            font-weight: 800;
            letter-spacing: 1.5px;
            color: #ff3333;
            text-transform: uppercase;
            text-shadow: 0 0 10px rgba(255, 51, 51, 0.6);
        }

        .title span {
            color: #ffffff;
            font-weight: 400;
            font-size: 18px;
        }

        .percentage {
            font-size: 20px;
            font-weight: 700;
            color: #ffd700;
        }

        .progress-bar-bg {
            width: 100%;
            height: 12px;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 6px;
            overflow: hidden;
            position: relative;
        }

        .progress-bar-fill {
            width: 0%;
            height: 100%;
            background: linear-gradient(90deg, #ff3333, #ff8800, #ffd700);
            border-radius: 6px;
            transition: width 0.3s ease-out;
            box-shadow: 0 0 10px rgba(255, 51, 51, 0.8);
        }

        .footer-text {
            font-size: 14px;
            color: #aaaaaa;
            text-align: right;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="title">GTAHUB <span>| Roleplay Server</span></div>
            <div class="percentage" id="percent-text">0%</div>
        </div>
        <div class="progress-bar-bg">
            <div class="progress-bar-fill" id="progress-fill"></div>
        </div>
        <div class="footer-text" id="status-text">Loading game assets...</div>
    </div>

    <script>
        function setProgress(percent) {
            const clamped = Math.min(100, Math.max(0, percent));
            document.getElementById('progress-fill').style.width = clamped + '%';
            document.getElementById('percent-text').innerText = clamped + '%';
            if (clamped >= 100) {
                document.getElementById('status-text').innerText = 'Joining server...';
            }
        }
    </script>
</body>
</html>)html";

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
            Log("[CEF] Found CEF API in module %p. Loading embedded HTML data URI...", hCEF);
            std::string dataUrl = "data:text/html;charset=utf-8,";
            for (char c : std::string(GTAHUB_EMBEDDED_HTML)) {
                if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
                    dataUrl += c;
                } else {
                    char buf[4];
                    sprintf(buf, "%%%02X", (unsigned char)c);
                    dataUrl += buf;
                }
            }
            fn_cef_create_browser(BROWSER_ID, dataUrl.c_str(), false, false);
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
