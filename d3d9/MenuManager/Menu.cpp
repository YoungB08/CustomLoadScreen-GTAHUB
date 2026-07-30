#include "Menu.h"
#include "../proxydirectx.h"

std::vector<CMenu*> MenuList;

CMenu::CMenu(QString title, POINT size, QObject *parent, CNode* node ) : CNodeMenu( parent )
{
    _title = title;
    _size = size;
    _description = "";
    _move = false;
    _context = nullptr;
    _menu = this;
    SetSelectable( false );

    if ( node != nullptr ){
        _node = node;
    }
    else _node = new CNode( this, { 0, 0 }, size );
    _node->SetMenu( this );

    _Init = false;
    MenuList.push_back( this );
}

CMenu::~CMenu()
{
    g_class.DirectX->d3d9_ReleaseTexture(_texture);
    VectorErase( MenuList, this );
}

bool CMenu::isInizialize()
{
    if ( _Init )
        return true;

    if ( g_class.DirectX->d3d9_device() == nullptr )
        return false;

    if ( !CNodeMenu::isInizialize() )
        return false;

    if ( !_node->isInizialize() )
        return false;

    _pos.x = (SCREEN_X - _size.x) / 2;
    _pos.y = (SCREEN_Y - _size.y) / 2;

    _header = _font->DrawHeight() + 3;
    _height = _size.y + _header;
    _width = _size.x + 6;
    _texture = g_class.DirectX->d3d9_CreateTexture( _size.x - (3 + _font->DrawLength( "X" )), _header );

    _Init = true;
    return true;
}

void CMenu::onDraw( int so_V, int so_H )
{
    if ( !isInizialize() )
        return;

    if ( _move ){
        SetMousePos( GetMousePos() );
        _pos.x = _MP.x - _mvOffset.x;
        _pos.y = _MP.y - _mvOffset.y;
    }
    else if ( isMouseOnHeader() )
        SetMenuHelper( _description );

    _texture->Begin();
    _texture->Clear( 0xFF000000 );
    _font->PrintShadow( 1, 0, -1, _title );
    _texture->End();
    _texture->Render( _pos.x, _pos.y );
    if ( isMouseOnClose() && isActive() ){
        _draw->D3DBox( _pos.x + (_size.x - (3 + _font->DrawLength( "X" ))),
                       _pos.y, 10 + _font->DrawLength( "X" ), _header, 0xFFFF0000 );
        SetMenuHelper( "Close" );
    }
    else{
        _draw->D3DBox( _pos.x + (_size.x - (3 + _font->DrawLength( "X" ))),
                       _pos.y, 10 + _font->DrawLength( "X" ), _header, 0xFFFF6060 );
    }
    _font->PrintShadow( _pos.x + (_size.x - _font->DrawLength( "X" ) + 2), _pos.y, -1, "X" );
    _draw->D3DBox( _pos.x, _pos.y + _header - 1, _size.x + 6, 1, -1 );

    _node->SetPosition( { _pos.x, _pos.y + _header } );
    POINT MP = GetMousePos();
    _node->SetMousePos( { MP.x - so_H, MP.y - so_V } );
    _node->onDraw( so_V, so_H );

    SetMousePos( GetMousePos() );

    if ( _context != nullptr ){
        p_helper.clear();
        if ( _context->isInizialize() )
            _context->onDraw();
    }

    if ( isMouseOnWidget() && !p_helper.empty() && IsForeground(this) ){
        POINT M = GetMousePos();
        float length = _font->DrawLength( p_helper[0] );
        for ( int i = 1; i < p_helper.size(); ++i ){
            if ( _font->DrawLength( p_helper[i] ) > length )
                length = _font->DrawLength( p_helper[i] );
        }
        int x_offset = 0;
        if ( M.x + 19 + length > SCREEN_X )
            x_offset = (M.x + 19 + length) - SCREEN_X;
        int y_offset = 0;
        if ( M.y + 19 + _font->DrawHeight() * p_helper.size() > SCREEN_Y )
            y_offset = (M.y + 19 + _font->DrawHeight() * p_helper.size()) - SCREEN_Y;
        _draw->D3DBoxBorder( (M.x + 16) - x_offset, M.y + 16 - y_offset, length + 3, _font->DrawHeight() * p_helper.size() + 3, -1, 0xF8000000 );
        for ( int i = 0; i < p_helper.size(); ++i )
            _font->PrintShadow( (M.x + 17) - x_offset, M.y + 16 + _font->DrawHeight() * i - y_offset, -1, p_helper[i] );
    }
    p_helper.clear();
    //p_helper.shrink_to_fit();
}

bool CMenu::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    SetMousePos(GetMousePos());

    if ( _context != nullptr ){
        if ( !_context->onEvents( hwnd, uMsg, wParam, lParam ) ){
            //            if ( _context->parent() == this )
            //                delete _context;
            _context = nullptr;
        }
        return false;
    }

    if ( !isMouseOnWidget() )
        return true;

    switch ( uMsg )
    {
    case WM_LBUTTONDOWN:
        if ( !IsForeground( this ) ){
            SetForeground( this );
            if ( isMouseOnClose() )
                return false;
        }
        if ( isMouseOnHeader() ){
            if ( !_move ){
                _mvOffset.x = _MP.x - _pos.x;
                _mvOffset.y = _MP.y - _pos.y;
                _move = true;
            }
            return false;
        }
        else if ( isMouseOnClose() )
            SetShow( false );
        break;
    case WM_LBUTTONUP:
        if ( _move ){
            _pos.x = _MP.x - _mvOffset.x;
            _pos.y = _MP.y - _mvOffset.y;
            _move = false;
            emit eventMove( this, _pos );
        }
        break;
    default:
        break;
    }

    if ( IsForeground( this ) )
        _node->onEvents( hwnd, uMsg, wParam, lParam );
    return false;
}

void CMenu::SetSize( POINT sz )
{
    delete _texture;
    _size = sz;
    _height = sz.y + _header;
    _width = sz.x + 6;
    _texture = new SRTexture( sz.x - (3 + _font->DrawLength( "X" )), _header );
    _node->SetSize( _size );
}

POINT CMenu::Size()
{
    return _size;
}

CNode* CMenu::Node()
{
    return _node;
}

void CMenu::SetMenuHelper( QString help )
{
    if ( help.isEmpty() ){
        p_helper.clear();
        return;
    }
    while ( help.indexOf( "\n" ) != -1 ){
        QRegExp re(R"(.*(\n(.*)))");
        if (re.indexIn(help) >= 0)
            p_helper.push_front( re.cap(2) );
        help = help.remove(re.cap(1));
    }
    p_helper.push_front( help );
}

bool CMenu::isMouseOnHeader()
{
    SetMousePos( GetMousePos() );
    if ( !isMouseOnWidget() )
        return false;
    if ( _MP.x < _pos.x + (_size.x - (3 + _font->DrawLength( "X" ))) && _MP.y < _pos.y + _header )
        return true;
    return false;
}

bool CMenu::isMouseOnClose()
{
    SetMousePos( GetMousePos() );
    if ( !isMouseOnWidget() )
        return false;
    if ( _MP.x > _pos.x + (_size.x - (3 + _font->DrawLength( "X" ))) && _MP.y < _pos.y + _header )
        return true;
    return false;
}

void CMenu::SetContextMenu( CContextMenu* contextMenu )
{
    _context = contextMenu;
    _context->SetPosition( GetMousePos() );
    _context->SetMenu( this );
}

bool CMenu::isActive()
{
    if ( IsForeground( this ) && _context == nullptr )
        return true;
    return false;
}

void SetForeground( CMenu* menu )
{
    for ( int i = 0; i < MenuList.size() - 1; ++i ){
        if ( menu == MenuList[i] ){
            myswap( MenuList[i], MenuList[MenuList.size() - 1] );
            break;
        }
    }
}

bool IsForeground( CMenu* menu )
{
    if ( menu == MenuList[MenuList.size() - 1] )
        return true;
    return false;
}
