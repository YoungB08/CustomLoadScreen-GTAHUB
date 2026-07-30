#ifndef CCREATETEXTURE_H
#define CCREATETEXTURE_H

#include "d3drender.h"
#include "color.h"
#include <string>

class SRTexture
{
    enum eTextureSource{
        eTS_none,
        eTS_file,
        eTS_mem
    };

    friend class proxyIDirect3DDevice9;
public:
    SRTexture(int width, int height);
    virtual ~SRTexture();

    virtual void Begin();
    virtual void End();

    virtual bool Clear( SRColor color = 0 );
    virtual bool Render( int X, int Y, int W = -1, int H = -1, float R = 0.0f );
    virtual HRESULT Save( const std::string& fileName );
    // TODO: save to mem
    virtual HRESULT Load( const std::string& fileName );
    virtual HRESULT Load( uint addr, uint size );
    virtual bool textureSizeAsBkg();

    virtual void ReInit( int width, int height );

    virtual ID3DXSprite* GetSprite();
    virtual IDirect3DTexture9* GetTexture();

protected:
    void Release();
    void Initialize( IDirect3DDevice9 *pDevice = nullptr );
    virtual void LoadTexture( const std::string& fileName );
    virtual void LoadTexture( uint addr, uint size );
    virtual bool Draw(IDirect3DTexture9 *texture, int X, int Y, int W = -1, int H = -1, float R = 0.0f );

    ID3DXSprite*            pSprite;
    IDirect3DTexture9*      pTexture;
    POINT                   textureSize;
    IDirect3DDevice9*       pDevice;

    IDirect3DTexture9*      pTexture_bkg = nullptr;
    POINT                   textureSize_bkg;
    eTextureSource          source_bkg;
    std::string             path_bkg;
    uint                    addr_bkg;
    uint                    size_bkg;

    bool                    isRenderToTexture;
    bool                    isReleased;
    bool                    kIsCalledFirstInitialize;

private:
    LPDIRECT3DSURFACE9      PP1S = NULL;
    LPDIRECT3DSURFACE9      DS = NULL;
    LPDIRECT3DSURFACE9      OldRT, OldDS;

    CD3DRender              *render;
};

#endif // CCREATETEXTURE_H
