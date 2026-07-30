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

class CustomLoadScreen
{
public:
    explicit CustomLoadScreen();
    virtual ~CustomLoadScreen();

    virtual void Loop();
    virtual bool Event(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    CD3DFont *pFont = nullptr;
    SRTexture* pTexture = nullptr;
    bool init = false;

    HRESULT Present ( CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride,
                      CONST RGNDATA *pDirtyRegion );
};

#endif // CustomLoadScreen_H
