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

HRESULT CustomLoadScreen::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
    if ((g_vars.gameSatate >= 7 && GetModuleHandleA("samp.dll") == 0) || g_vars.gameSatate == 9)
        return D3D_OK;

    if (!g_class.DirectX || !g_class.DirectX->d3d9_device())
        return D3D_OK;

    if (!init){
        init = true;
        pFont = g_class.DirectX->d3d9_CreateFont("Arial", 11, 5);
        pTexture = g_class.DirectX->d3d9_CreateTexture(g_class.params->BackBufferWidth,
                                                       g_class.params->BackBufferHeight);
        pTexture->Load("CustomLoadScreen.png");
        //pTexture->textureSizeAsBkg();
    }

    if (!init)
        return D3D_OK;

    // Render
    pTexture->Begin();
    pTexture->Clear(eCdBlack);

    // Draw project name
    pFont->PrintShadow(5, 5, -1, std::string(PROJECT_NAME) + " by SR_team");
    pFont->PrintShadow(5, 20, -1, "Loading: " + std::to_string(11.111111 * g_vars.gameSatate) + "%");

    pTexture->End();
    pTexture->Render(0, 0, g_class.params->BackBufferWidth, g_class.params->BackBufferHeight); // Draw texture

    return D3D_OK;
}
