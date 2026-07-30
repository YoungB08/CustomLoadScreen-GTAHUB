#include "CustomLoadScreen.h"
#include "loader.h"
#include "CGame/samp.h"
#include <string>
#include <gdiplus.h>

using namespace Gdiplus;

// ── SAMP memory ──────────────────────────────────────────────────────────────
struct RawSampData { char ip[257]; char hostname[259]; int port; int players; int ok; };
static void ReadSampRaw(RawSampData* out)
{
    out->ok = 0;
    HMODULE hSamp = GetModuleHandleA("samp.dll");
    if (!hSamp) return;
    uintptr_t* pp = (uintptr_t*)((uintptr_t)hSamp + SAMP_INFO_OFFSET);
    __try {
        if (!pp || !*pp) return;
        stSAMP* p = (stSAMP*)*pp;
        if (!p->szIP[0]) return;
        strncpy_s(out->ip,       p->szIP,       256);
        strncpy_s(out->hostname, p->szHostname, 258);
        out->port = (int)p->ulPort;
        // Count online players via pool
        if (p->pPools && p->pPools->pPlayer) {
            stPlayerPool* pool = p->pPools->pPlayer;
            int cnt = 0;
            DWORD maxID = pool->ulMaxPlayerID;
            if (maxID > 1004) maxID = 1004;
            for (DWORD pi = 0; pi <= maxID; pi++)
                if (pool->iIsListed[pi]) cnt++;
            out->players = cnt;
        }
        out->ok = 1;
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

// ── Helpers ──────────────────────────────────────────────────────────────────
static std::wstring ToWide(const std::string& s)
{
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], n);
    return w;
}

// ── GDI+ drawing helpers ──────────────────────────────────────────────────────
void CustomLoadScreen::DrawRoundedRect(Graphics& g, Brush* brush,
                                        float x, float y, float w, float h, float r)
{
    GraphicsPath path;
    path.AddArc(x,         y,         r*2, r*2, 180, 90);
    path.AddArc(x+w-r*2,  y,         r*2, r*2, 270, 90);
    path.AddArc(x+w-r*2,  y+h-r*2,   r*2, r*2,   0, 90);
    path.AddArc(x,         y+h-r*2,   r*2, r*2,  90, 90);
    path.CloseFigure();
    g.FillPath(brush, &path);
}

void CustomLoadScreen::DrawRoundedRectBorder(Graphics& g, Pen* pen,
                                              float x, float y, float w, float h, float r)
{
    GraphicsPath path;
    path.AddArc(x,         y,         r*2, r*2, 180, 90);
    path.AddArc(x+w-r*2,  y,         r*2, r*2, 270, 90);
    path.AddArc(x+w-r*2,  y+h-r*2,   r*2, r*2,   0, 90);
    path.AddArc(x,         y+h-r*2,   r*2, r*2,  90, 90);
    path.CloseFigure();
    g.DrawPath(pen, &path);
}

// ── Main UI draw ──────────────────────────────────────────────────────────────
void CustomLoadScreen::DrawUI(Graphics& g, int W, int H, int pct, DWORD tick)
{
    g.Clear(Color(0, 0, 0, 0)); // fully transparent canvas
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    float sc = (float)W / 1280.0f;

    // Panel dimensions (matching HTML)
    float pW   = 1050.0f * sc;
    float pPad = 30.0f   * sc;
    float pX   = (W - pW) * 0.5f;

    // Font sizes (scaled)
    float titlePt = 22.0f * sc;
    float pctPt   = 38.0f * sc;
    float normalPt= 11.0f * sc;
    float smallPt = 9.5f  * sc;

    FontFamily ff(L"Tahoma");
    Font fTitle(&ff, titlePt, FontStyleBold,   UnitPixel);
    Font fPct  (&ff, pctPt,   FontStyleBold,   UnitPixel);
    Font fNorm (&ff, normalPt,FontStyleRegular, UnitPixel);
    Font fSmall(&ff, smallPt, FontStyleRegular, UnitPixel);

    StringFormat sfLeft, sfRight, sfCenter;
    sfRight.SetAlignment(StringAlignmentFar);
    sfCenter.SetAlignment(StringAlignmentCenter);

    // Measure title height to compute panel height
    RectF titleBox;
    g.MeasureString(L"GTAHUB", -1, &fTitle, PointF(0,0), &titleBox);
    float titleH = titleBox.Height;

    RectF normBox;
    g.MeasureString(L"X", -1, &fNorm, PointF(0,0), &normBox);
    float normH = normBox.Height;

    float barH  = 16.0f * sc;
    float pPad2 = pPad * 2;

    float contentH = pPad                 // top pad
                   + titleH + 14*sc      // header
                   + barH   + 10*sc      // bar
                   + normH  + 12*sc      // status
                   + normH  + 14*sc      // info row
                   + normH              // footer
                   + pPad;              // bottom pad

    float pY = H - 45.0f*sc - contentH;
    float pH = contentH;

    // ── Panel glow (multiple layers) ─────────────────────────────────────────
    for (int i = 3; i >= 1; i--) {
        float expand = (float)i * 5.0f * sc;
        BYTE alpha   = (BYTE)(15 * (4 - i));
        SolidBrush glowBrush(Color(alpha, 255, 0, 0));
        DrawRoundedRect(g, &glowBrush, pX-expand, pY-expand,
                        pW+expand*2, pH+expand*2, (18.0f+expand)*sc);
    }

    // ── Panel background ─────────────────────────────────────────────────────
    SolidBrush panelBg(Color(185, 15, 15, 18));
    DrawRoundedRect(g, &panelBg, pX, pY, pW, pH, 18.0f*sc);

    // ── Panel border ─────────────────────────────────────────────────────────
    Pen borderPen(Color(64, 255, 0, 0), 1.0f*sc);
    DrawRoundedRectBorder(g, &borderPen, pX, pY, pW, pH, 18.0f*sc);

    float cx = pX + pPad;
    float cy = pY + pPad;

    // ── Header: Title ─────────────────────────────────────────────────────────
    // "GTAHUB" in red
    SolidBrush redBrush(Color(255, 255, 60, 60));
    g.DrawString(L"GTAHUB", -1, &fTitle, PointF(cx, cy), &sfLeft, &redBrush);

    RectF gtaBox;
    g.MeasureString(L"GTAHUB", -1, &fTitle, PointF(0,0), &gtaBox);

    // " ROLEPLAY SERVER" in white
    SolidBrush whiteBrush(Color(255, 255, 255, 255));
    g.DrawString(L" ROLEPLAY SERVER", -1, &fTitle,
                 PointF(cx + gtaBox.Width, cy), &sfLeft, &whiteBrush);

    // Percent (right side)
    std::wstring pctStr = std::to_wstring(pct) + L"%";
    SolidBrush redBrush2(Color(255, 255, 64, 64));
    RectF pctLayout(pX, cy, pW - pPad, pctPt + 8*sc);
    g.DrawString(pctStr.c_str(), -1, &fPct, pctLayout, &sfRight, &redBrush2);

    cy += titleH + 14.0f*sc;

    // ── Progress bar ─────────────────────────────────────────────────────────
    float bW    = pW - pPad2;
    float fillW = bW * pct / 100.0f;
    float radius= barH * 0.5f;

    // Bar track
    SolidBrush trackBrush(Color(255, 10, 10, 10));
    DrawRoundedRect(g, &trackBrush, cx, cy, bW, barH, radius);
    Pen trackBorder(Color(140, 60, 60, 60), 1.0f);
    DrawRoundedRectBorder(g, &trackBorder, cx, cy, bW, barH, radius);

    // Gradient fill
    if (fillW > 2) {
        LinearGradientBrush gradBrush(
            PointF(cx, cy), PointF(cx + fillW, cy),
            Color(255, 255, 0,   0),
            Color(255, 255, 120, 0));

        // Simple shimmer via gradient from dark-red to orange
        (void)tick; // shimmer handled by gradient direction

        // Clip to rounded rect area
        GraphicsPath clipPath;
        clipPath.AddArc(cx,          cy,          radius*2, barH, 180, 90);
        clipPath.AddArc(cx+fillW-radius*2, cy,    radius*2, barH, 270, 90);
        clipPath.AddArc(cx+fillW-radius*2, cy,    radius*2, barH,   0, 90);
        clipPath.AddArc(cx,          cy,          radius*2, barH,  90, 90);
        clipPath.CloseFigure();
        g.SetClip(&clipPath);
        g.FillRectangle(&gradBrush, cx, cy, fillW, barH);
        g.ResetClip();

        // Top highlight
        LinearGradientBrush highBrush(
            PointF(cx, cy), PointF(cx, cy + barH * 0.4f),
            Color(80, 255, 220, 140), Color(0, 255, 255, 255));
        g.SetClip(&clipPath);
        g.FillRectangle(&highBrush, cx, cy, fillW, barH * 0.4f);
        g.ResetClip();

        // Glow below bar
        for (int i = 1; i <= 3; i++) {
            Pen glowPen(Color((BYTE)(30/i), 255, 60, 0), (float)(i*2));
            DrawRoundedRectBorder(g, &glowPen, cx, cy, fillW, barH, radius);
        }
    }

    cy += barH + 10.0f*sc;

    // ── Status ────────────────────────────────────────────────────────────────
    SolidBrush grayBrush(Color(255, 150, 150, 150));
    std::wstring statusW = (pct >= 100) ? L"Joining server..." : L"Loading game assets...";
    g.DrawString(statusW.c_str(), -1, &fNorm, PointF(cx, cy), &sfLeft, &grayBrush);

    cy += normH + 14.0f*sc;

    // ── Info row ──────────────────────────────────────────────────────────────
    SolidBrush dimRed(Color(255, 255, 64, 64));
    SolidBrush dimWhite(Color(255, 200, 200, 200));
    SolidBrush dimGray(Color(255, 100, 100, 100));

    float colGap = 180.0f*sc;

    // Left side
    std::wstring hostW = sampReady ? ToWide(sampHost) : L"play.gtahub.vn";
    std::wstring addrW = sampReady ? ToWide(sampAddr + ":" + std::to_string(sampPort)) : L"--";

    auto drawItem = [&](float ix, float iy, const wchar_t* label, const wchar_t* val) {
        g.DrawString(L"\u25CF ", -1, &fNorm, PointF(ix, iy), &sfLeft, &dimRed);
        RectF dotBox; g.MeasureString(L"\u25CF ", -1, &fNorm, PointF(0,0), &dotBox);
        g.DrawString(val, -1, &fNorm, PointF(ix + dotBox.Width, iy), &sfLeft, &dimWhite);
    };

    drawItem(cx,                cy, L"", hostW.c_str());
    std::wstring playersW = (sampReady && sampPlayers >= 0)
        ? (std::to_wstring(sampPlayers) + L"/1000")
        : L"--/1000";
    drawItem(cx + colGap,        cy, L"", playersW.c_str());
    drawItem(cx + colGap*2,      cy, L"", addrW.c_str());

    // Right side
    DWORD el  = (tick - startTick) / 1000;
    wchar_t elBuf[16];
    swprintf_s(elBuf, L"%02d:%02d", el/60, el%60);

    float rightX = pX + pW - pPad;
    auto drawItemR = [&](float offFromRight, const wchar_t* val) {
        RectF box; g.MeasureString(val, -1, &fNorm, PointF(0,0), &box);
        float ix = rightX - offFromRight - box.Width - 14.0f*sc;
        g.DrawString(L"\u25CF ", -1, &fNorm, PointF(ix, cy), &sfLeft, &dimRed);
        RectF dotBox; g.MeasureString(L"\u25CF ", -1, &fNorm, PointF(0,0), &dotBox);
        g.DrawString(val, -1, &fNorm, PointF(ix + dotBox.Width, cy), &sfLeft, &dimWhite);
    };

    drawItemR(190.0f*sc, L"Anti-Cheat");
    drawItemR(80.0f*sc,  L"Assets");
    drawItemR(0.0f,       elBuf);

    // ── Footer ────────────────────────────────────────────────────────────────
    cy += normH + 14.0f*sc;
    g.DrawString(L"v2.5.1", -1, &fSmall, PointF(cx, cy), &sfLeft, &dimGray);
    RectF footerLayout(pX, cy, pW - pPad, smallPt + 4*sc);
    g.DrawString(L"www.gtahub.vn", -1, &fSmall, footerLayout, &sfRight, &dimGray);
}

// ── CustomLoadScreen lifecycle ────────────────────────────────────────────────
CustomLoadScreen::CustomLoadScreen()
{
    GdiplusStartupInput gsi;
    GdiplusStartup(&gdipToken, &gsi, nullptr);

    if (g_class.DirectX)
        g_class.DirectX->SetPresentCallback([this](const RECT*s,const RECT*d,HWND h,const RGNDATA*r){
            return Present(s,d,h,r);
        });
}

CustomLoadScreen::~CustomLoadScreen()
{
    if (pSprite)  { pSprite->Release(); pSprite = nullptr; }
    if (pUITex)   { pUITex->Release();  pUITex  = nullptr; }
    delete pCanvas; pCanvas = nullptr;

    if (g_class.DirectX) {
        g_class.DirectX->ClearPresentCallback();
        if (pBgTex)   g_class.DirectX->d3d9_ReleaseTexture(pBgTex);
        if (pLogoTex) g_class.DirectX->d3d9_ReleaseTexture(pLogoTex);
    }
    if (gdipToken) { GdiplusShutdown(gdipToken); gdipToken = 0; }
}

void CustomLoadScreen::Loop()  {}
bool CustomLoadScreen::Event(UINT, WPARAM, LPARAM) { return true; }

#include "loader.h"

HRESULT CustomLoadScreen::Present(const RECT*, const RECT*, HWND, const RGNDATA*)
{
    if ((g_vars.gameSatate >= 7 && !GetModuleHandleA("samp.dll")) || g_vars.gameSatate == 9)
        return D3D_OK;
    auto* dev = g_class.DirectX ? g_class.DirectX->d3d9_device() : nullptr;
    if (!dev) return D3D_OK;

    int W = *(int*)0x00C9C040; if (W <= 0) W = 1280;
    int H = *(int*)0x00C9C044; if (H <= 0) H = 720;

    // ── Init (once) ──────────────────────────────────────────────────────────
    if (!init) {
        Log("[PRESENT] Init %dx%d", W, H);

        pBgTex = g_class.DirectX->d3d9_CreateTexture(W, H);
        if (pBgTex) pBgTex->Load("CustomLoadScreen.png");

        pLogoTex = g_class.DirectX->d3d9_CreateTexture(W, H);
        if (pLogoTex) pLogoTex->Load("images/logo.png");

        // D3D9 overlay texture (ARGB, managed)
        dev->CreateTexture(W, H, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pUITex, nullptr);
        D3DXCreateSprite(dev, &pSprite);

        // GDI+ canvas
        pCanvas = new Bitmap(W, H, PixelFormat32bppARGB);

        init = true;
        Log("[PRESENT] Init done");
    }
    if (!pBgTex || !pUITex || !pSprite || !pCanvas) return D3D_OK;

    // ── SAMP info ─────────────────────────────────────────────────────────────
    if (!sampReady) {
        RawSampData raw{};
        ReadSampRaw(&raw);
        if (raw.ok) {
            sampHost  = raw.hostname[0] ? raw.hostname : raw.ip;
            sampAddr  = raw.ip;
            sampPort  = raw.port;
            sampReady = true;
            sampPlayers = raw.players;
            Log("[SAMP] %s:%d (%s)", raw.ip, raw.port, raw.hostname);
        }
    }

    // Real progress from game state
    int realPct = (int)(11.111111f * g_vars.gameSatate);
    if (realPct > 100) realPct = 100;
    if (realPct < 0)   realPct = 0;

    // Smooth visual progress: interpolate displayPct toward realPct over 5s total
    DWORD now = GetTickCount();
    if (lastTick == 0) lastTick = now;
    float dt = (float)(now - lastTick) / 1000.0f; // seconds since last frame
    lastTick = now;

    // Speed: cover 100% in 5 seconds = 20%/s, but never go past realPct
    float speed    = 100.0f / 5.0f; // 20%/s
    float maxDelta = speed * dt;
    float target   = (float)realPct;
    if (displayPct < target)
        displayPct = (displayPct + maxDelta < target ? displayPct + maxDelta : target);

    int pct = (int)displayPct;

    // ── Render background ─────────────────────────────────────────────────────
    pBgTex->Begin(); pBgTex->Clear(eCdBlack); pBgTex->End();
    pBgTex->Render(0, 0, W, H);

    // ── Draw UI via GDI+ into canvas ──────────────────────────────────────────
    {
        Graphics g(pCanvas);
        DrawUI(g, W, H, pct, GetTickCount());
    }

    // ── Copy canvas pixels -> D3D9 texture ────────────────────────────────────
    {
        D3DLOCKED_RECT lr;
        if (SUCCEEDED(pUITex->LockRect(0, &lr, nullptr, D3DLOCK_DISCARD))) {
            BitmapData bd;
            Rect gdiRect(0, 0, W, H);
            if (pCanvas->LockBits(&gdiRect, ImageLockModeRead,
                                  PixelFormat32bppARGB, &bd) == Ok)
            {
                auto* src = (BYTE*)bd.Scan0;
                auto* dst = (BYTE*)lr.pBits;
                for (int y = 0; y < H; y++, src += bd.Stride, dst += lr.Pitch)
                    memcpy(dst, src, W * 4);
                pCanvas->UnlockBits(&bd);
            }
            pUITex->UnlockRect(0);
        }
    }

    // ── Render overlay texture ────────────────────────────────────────────────
    pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    RECT srcRect = { 0, 0, W, H };
    D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
    pSprite->Draw(pUITex, &srcRect, nullptr, &origin, 0xFFFFFFFF);
    pSprite->End();

    // ── Logo overlay (if available) ───────────────────────────────────────────
    if (pLogoTex && pLogoTex->GetTexture()) {
        float lW = 520.0f * (W / 1280.0f);
        float lH = 175.0f * (W / 1280.0f);
        pLogoTex->Render((int)((W - lW) * 0.5f), (int)(H * 0.07f), (int)lW, (int)lH);
    }

    return D3D_OK;
}