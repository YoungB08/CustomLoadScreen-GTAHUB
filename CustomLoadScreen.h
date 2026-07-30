#ifndef CustomLoadScreen_H
#define CustomLoadScreen_H

#include "loader.h"
#include "d3d9/d3drender.h"
#include "d3d9/texture.h"
#include "d3d9/MenuManager/Menu.h"
#include "d3d9/MenuManager/ContextMenu.h"
#include "d3d9/MenuManager/Text.h"
#include "d3d9/MenuManager/Node.h"
#include "d3d9/MenuManager/Listing.h"
#include "d3d9/MenuManager/VerticalLayout.h"
#include "d3d9/MenuManager/CSlider.h"
#include <gdiplus.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")

class CustomLoadScreen
{
public:
    explicit CustomLoadScreen();
    virtual ~CustomLoadScreen();
    virtual void Loop();
    virtual bool Event(UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT Present(CONST RECT* pSrc, CONST RECT* pDst, HWND hWnd, CONST RGNDATA* pDirty);

private:
    void DrawUI(Gdiplus::Graphics& g, int W, int H, int pct, DWORD tick);
    void DrawRoundedRect(Gdiplus::Graphics& g, Gdiplus::Brush* brush,
                         float x, float y, float w, float h, float r);
    void DrawRoundedRectBorder(Gdiplus::Graphics& g, Gdiplus::Pen* pen,
                               float x, float y, float w, float h, float r);

    SRTexture*            pBgTex    = nullptr;
    SRTexture*            pLogoTex  = nullptr;
    IDirect3DTexture9*    pUITex    = nullptr;
    ID3DXSprite*          pSprite   = nullptr;
    Gdiplus::Bitmap*      pCanvas   = nullptr;
    ULONG_PTR             gdipToken = 0;
    bool                  init      = false;

    // SAMP
    bool        sampReady = false;
    std::string sampHost;
    std::string sampAddr;
    int         sampPort    = 7777;
    int         sampPlayers = -1;

    DWORD startTick = GetTickCount();
    int   lastPct    = -1;
    float displayPct = 0.0f;
    DWORD lastTick   = 0;
};

#endif