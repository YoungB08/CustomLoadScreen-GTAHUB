#include "CustomLoadScreen.h"

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
        return D3D_OK;

    if (!g_class.DirectX || !g_class.DirectX->d3d9_device())
        return D3D_OK;

    int screenWidth = *(int*)0x00C9C040;
    int screenHeight = *(int*)0x00C9C044;
    if (screenWidth <= 0) screenWidth = 800;
    if (screenHeight <= 0) screenHeight = 600;

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

    // Calculate progress percentage
    int percentage = static_cast<int>(11.111111 * g_vars.gameSatate);
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;

    // Draw stylized text with HTML Hex color tags
    pFont->PrintShadow(20, 20, -1, "{FF3333}GTAHUB {FFFFFF}| Roleplay Server");
    pFont->PrintShadow(20, 42, -1, "{AAAAAA}Status: {FFFFFF}Loading game assets... {FFD700}" + std::to_string(percentage) + "%");

    pTexture->End();
    pTexture->Render(0, 0, screenWidth, screenHeight); // Draw texture

    return D3D_OK;
}
