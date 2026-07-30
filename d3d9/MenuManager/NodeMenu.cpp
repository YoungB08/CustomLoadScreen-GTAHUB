#include "NodeMenu.h"
#include "Menu.h"
#include "../proxydirectx.h"

CNodeMenu::CNodeMenu(QObject *parent, POINT pos, CD3DFont *font, bool deleteOnDestructor ) : QObject( parent )
{
    _pos = pos;
    _show = true;
    _menu = nullptr;
    _SO = { 0, 0 };
    _colorSelect = 0;
    _selectable = true;

    if ( font != nullptr ){
        _font = font;
        _deleteFont = deleteOnDestructor;
    }
    else {
        _font = g_class.DirectX->d3d9_CreateFont( "Arial", 10, FCR_BORDER );
        _deleteFont = true;
    }
    _draw = g_class.DirectX->d3d9_CreateRender( 40 );

    _Init = false;
}

CNodeMenu::~CNodeMenu()
{
    if ( _deleteFont )
        g_class.DirectX->d3d9_ReleaseFont( _font );
    g_class.DirectX->d3d9_ReleaseRender( _draw );
}
void CNodeMenu::onDraw( int so_V, int so_H )
{
    if ( !isInizialize() )
        return;

    _SO.x = so_H;
    _SO.y = so_V;

    if ( isMouseOnWidget( so_V, so_H ) && _menu != nullptr ){

        if ( ((CMenu*)_menu)->isActive() ){

            if ( _menu != this && _selectable )
                _draw->D3DBoxBorder( _pos.x - so_H - 1, _pos.y - so_V - 1, _width + 2, _height + 2, _colorSelect, 0 );

            if ( !_description.isEmpty() )
                ((CMenu*)_menu)->SetMenuHelper( _description );
        }
    }
}

bool CNodeMenu::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( !isMouseOnWidget( _SO.y, _SO.x ) )
        return true;

    switch ( uMsg )
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        emit eventClick( this, uMsg );
        return false;
    default:
        break;
    }

    return true;
}

bool CNodeMenu::isInizialize()
{
    if ( _Init )
        return true;

    if ( g_class.DirectX->d3d9_device() == nullptr )
        return false;

    _Init = true;
    return true;
}

void CNodeMenu::SetPosition( POINT pos )
{
    _pos = pos;
    emit eventMove( this, _pos );
}

POINT CNodeMenu::Position()
{
    return _pos;
}

bool CNodeMenu::isShowed()
{
    return _show;
}

void CNodeMenu::SetShow( bool show )
{
    _show = show;
    emit eventShow(this, show);
}

bool CNodeMenu::isSelectable()
{
    return _selectable;
}

void CNodeMenu::SetSelectable( bool selectable )
{
    _selectable = selectable;
}

int CNodeMenu::Height()
{
    return _height;
}

int CNodeMenu::Width()
{
    return _width;
}

POINT CNodeMenu::GetMousePos()
{
    POINT M;
    GetCursorPos( &M );
    ScreenToClient( *(HWND*)0xC97C1C, &M );
    return M;
}

void CNodeMenu::SetMousePos( POINT MP )
{
    _MP = MP;
}

bool CNodeMenu::isMouseOnWidget( int so_V, int so_H )
{
    POINT M = _MP;
    int PX = _pos.x - so_H;
    int PY = _pos.y - so_V;
    if ( M.x > PX && M.x < PX + _width && M.y > PY && M.y < PY + _height )
        return true;
    return false;
}

void CNodeMenu::SetDescription( QString description )
{
    _description = description;
}

QString CNodeMenu::Description()
{
    return _description;
}

void CNodeMenu::SetMenu( CNodeMenu *menu )
{
    _menu = menu;
}
