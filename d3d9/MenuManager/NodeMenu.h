#ifndef NodeMenu_H
#define NodeMenu_H

#include <windows.h>
#include "../d3drender.h"
#include <string>
#define CALLBACK __stdcall

class CNodeMenu
{
    friend class CContextMenu;
    friend class CListing;
    friend class CMenu;
    friend class CNode;
    friend class CVerticalLayout;
    friend class CSlider;
public:
    CNodeMenu( CNodeMenu* parent, POINT = { 0, 0 }, CD3DFont* = nullptr, bool = false );

    virtual ~CNodeMenu();

    virtual void onDraw( int, int );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual bool isInizialize();

    virtual void SetPosition( POINT );
    virtual POINT Position();

    virtual bool isShowed();
    virtual void SetShow( bool );

    virtual bool isSelectable();
    virtual void SetSelectable( bool );

    virtual int Height();
    virtual int Width();

    virtual void SetDescription( const std::string& );
    virtual std::string Description();

    CNodeMenu* parent() const { return _parent; }

protected:
    CD3DFont *_font;
    CD3DRender *_draw;
    CNodeMenu *_menu;
    CNodeMenu *_parent;

    std::string _description;
    POINT _pos;
    int _height;
    int _width;
    POINT _MP;
    POINT _SO;
    bool _selectable;

    SRColor _colorSelect;

    POINT GetMousePos();
    virtual bool isMouseOnWidget( int = 0, int = 0 );
    virtual void SetMousePos( POINT );
    virtual void SetMenu( CNodeMenu *menu );

private:
    bool _Init;
    bool _show;
    bool _deleteFont;
};

#endif // NodeMenu_H
