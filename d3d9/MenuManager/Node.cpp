#include "Node.h"
#include "../proxydirectx.h"

CNode::CNode( QObject *parent, POINT pos, POINT size ) : CNodeMenu( parent, pos )
{
    if (parent == nullptr)
        throw "Parent for CNode can't be null";
    _size = size;
    _height = _size.y + 6;
    _width = _size.x + 6;
    _scrollOffsetVertical = 0;
    _scrollOffsetHorizontal = 0;
    _scrollSizeVertical = _size.y;
    _scrollSizeHorizontal = _size.x;
    _scrollStep = 10;
    _scrollFrame = 0xFF00FF00;
    _scrollMat = 0xFF0000FF;
    _scrollRoller = 0xFFFF0000;
    _colorBkg = 0xFF000000;

    _Init = false;
}

CNode::~CNode()
{
    for ( int i = 0; i < _nodes.size(); ++i )
        if ( _nodes[i].node->parent() == this )
            delete _nodes[i].node;

    g_class.DirectX->d3d9_ReleaseTexture(_texture);
}

bool CNode::isInizialize()
{
    if ( _Init )
        return true;

    if ( g_class.DirectX->d3d9_device() == nullptr )
        return false;

    if ( !CNodeMenu::isInizialize() )
        return false;

    if ( _menu == nullptr )
        return false;

    for ( int i = 0; i < _nodes.size(); ++i ){
        _nodes[i].node->SetMenu( _menu );
        if ( !_nodes[i].node->isInizialize() )
            return false;
    }
    _texture = g_class.DirectX->d3d9_CreateTexture( _size.x, _size.y );

    _Init = true;
    return true;
}

void CNode::onDraw( int so_V, int so_H )
{
    if ( !isInizialize() )
        return;

    _texture->Begin();
    _texture->Clear( _colorBkg );

    for ( int i = 0; i < _nodes.size(); ++i ){
        if ( _nodes[i].node->isShowed() ){

            int sd = _nodes[i].node->Position().y + _nodes[i].node->Height();
            if ( sd > _scrollSizeVertical )
                _scrollSizeVertical = sd;

            sd = _nodes[i].node->Position().x + _nodes[i].node->Width();
            if ( sd > _scrollSizeHorizontal )
                _scrollSizeHorizontal = sd;

            if ( isMouseInNode(so_V, so_H) )
                _nodes[i].node->SetMousePos( { _MP.x - _pos.x + so_H, _MP.y - _pos.y + so_V } );
            else _nodes[i].node->SetMousePos( { -1, -1 } );
            _nodes[i].node->onDraw( _scrollOffsetVertical, _scrollOffsetHorizontal );
        }
    }

    _texture->End();
    _texture->Render( _pos.x - so_H, _pos.y - so_V );
    DrawScrollBarVertical( so_V, so_H );
    DrawScrollBarHorizontal( so_V, so_H );
    CNodeMenu::onDraw( so_V, so_H );
}

void CNode::DrawScrollBarVertical( int so_V, int so_H )
{
    int fullScroll = _size.y - 4;
    float rollerSize = (float)_size.y / ((float)_scrollSizeVertical) * (float)fullScroll;
    float rollerOffset = (float)_size.y / ((float)_scrollSizeVertical) * (float)_scrollOffsetVertical;

    _draw->D3DBox( _pos.x + _size.x - so_H, _pos.y - so_V, 6, _size.y, _scrollFrame );
    _draw->D3DBox( _pos.x + _size.x + 2 - so_H, (_pos.y + 2) - so_V, 2, fullScroll, _scrollMat );
    _draw->D3DBox( _pos.x + _size.x + 2 - so_H, (_pos.y + 2 + rollerOffset) - so_V, 2, rollerSize, _scrollRoller );
}

void CNode::DrawScrollBarHorizontal( int so_V, int so_H )
{
    float rollerSize = (float)_size.x / ((float)_scrollSizeHorizontal) * (float)_size.x;
    float rollerOffset = (float)_size.x / ((float)_scrollSizeHorizontal) * (float)_scrollOffsetHorizontal;

    _draw->D3DBox( _pos.x - so_H, (_pos.y + _size.y) - so_V, _size.x + 6, 6, _scrollFrame );
    _draw->D3DBox( _pos.x + 3 - so_H, (_pos.y + _size.y + 2) - so_V, _size.x, 2, _scrollMat );
    _draw->D3DBox( _pos.x + 3 + rollerOffset - so_H, (_pos.y + _size.y + 2) - so_V, rollerSize, 2, _scrollRoller );
}

bool CNode::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( !isMouseInNode( _SO.y, _SO.x ) )
        return true;
    for ( int i = 0; i < _nodes.size(); ++i ){
        if ( _nodes[i].node->isShowed() )
            if ( !_nodes[i].node->onEvents( hwnd, uMsg, wParam, lParam ) )
                return false;
    }
    switch ( uMsg )
    {
    case WM_MOUSEWHEEL:
        if ( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 ){
            _scrollOffsetVertical += _scrollStep;
            if ( _scrollOffsetVertical + _size.y > _scrollSizeVertical )
                _scrollOffsetVertical = _scrollSizeVertical - _size.y;
        }
        else{
            _scrollOffsetVertical -= _scrollStep;
            if ( _scrollOffsetVertical < 0 )
                _scrollOffsetVertical = 0;
        }
        return false;
    case WM_KEYDOWN:
        switch ( wParam )
        {
        case VK_UP:
            _scrollOffsetVertical -= _scrollStep;
            if ( _scrollOffsetVertical < 0 )
                _scrollOffsetVertical = 0;
            return false;
        case VK_DOWN:
            _scrollOffsetVertical += _scrollStep;
            if ( _scrollOffsetVertical + _size.y > _scrollSizeVertical )
                _scrollOffsetVertical = _scrollSizeVertical - _size.y;
            return false;
        case VK_LEFT:
            _scrollOffsetHorizontal -= _scrollStep;
            if ( _scrollOffsetHorizontal < 0 )
                _scrollOffsetHorizontal = 0;
            return false;
        case VK_RIGHT:
            _scrollOffsetHorizontal += _scrollStep;
            if ( _scrollOffsetHorizontal + _size.x > _scrollSizeHorizontal )
                _scrollOffsetHorizontal = _scrollSizeHorizontal - _size.x;
            return false;
        case VK_PRIOR:
            _scrollOffsetVertical = 0;
            return false;
        case VK_NEXT:
            _scrollOffsetVertical = _scrollSizeVertical - _size.y;
            return false;
        case VK_HOME:
            _scrollOffsetHorizontal = 0;
            return false;
        case VK_END:
            _scrollOffsetHorizontal = _scrollSizeHorizontal - _size.x;
            return false;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return CNodeMenu::onEvents(hwnd, uMsg, wParam, lParam);
}

void CNode::SetSize( POINT sz )
{
    delete _texture;
    _size = sz;
    _height = sz.y + 6;
    _width = sz.x + 6;
    _texture = new SRTexture( sz.x, sz.y );
}

POINT CNode::Size()
{
    return _size;
}

int CNode::ScrollVertical()
{
    return _scrollOffsetVertical - _size.y;
}

void CNode::SetScrollVertical( int scroll )
{
    _scrollOffsetVertical = scroll;
    if ( _scrollOffsetVertical + _size.y > _scrollSizeVertical )
        _scrollOffsetVertical = _scrollSizeVertical - _size.y;
    else if ( _scrollOffsetVertical < 0 )
        _scrollOffsetVertical = 0;
}

int CNode::ScrollHorizontal()
{
    return _scrollOffsetHorizontal - _size.y;
}

void CNode::SetScrollHorizontal( int scroll )
{
    _scrollOffsetHorizontal = scroll;
    if ( _scrollOffsetHorizontal + _size.x > _scrollSizeHorizontal )
        _scrollOffsetHorizontal = _scrollSizeHorizontal - _size.x;
    else if ( _scrollOffsetHorizontal < 0 )
        _scrollOffsetHorizontal = 0;
}

void CNode::SetScrollStep( int step )
{
    _scrollStep = step;
}

void CNode::ScrollColor(SRColor &frame, SRColor &mat, SRColor &roller )
{
    frame = _scrollFrame;
    mat = _scrollMat;
    roller = _scrollRoller;
}

void CNode::SetScrollColor(SRColor frame, SRColor mat, SRColor roller )
{
    _scrollFrame = frame;
    _scrollMat = mat;
    _scrollRoller = roller;
}

bool CNode::AddChield( CNodeMenu* node, QString name )
{
    if (name.length() > 1)
        for ( int i = 0; i < _nodes.size(); ++i )
            if ( _nodes[i].name == name )
                return false;

    _nodes.push_back( { node, name } );
    node->SetMenu( _menu );
    return true;
}

bool CNode::DelChield( CNodeMenu* node )
{
    for ( int i = 0; i < _nodes.size(); ++i ){
        if ( _nodes[i].node == node ){
            if ( node->parent() == this )
                delete node;
            _nodes.erase( _nodes.begin() + i );
            return true;
        }
    }
    return false;
}

bool CNode::DelChield( QString name )
{
    if ( name.isEmpty() )
        return false;

    for ( int i = 0; i < _nodes.size(); ++i ){
        if ( _nodes[i].name == name ){
            if ( _nodes[i].node->parent() == this )
                delete _nodes[i].node;
            _nodes.erase( _nodes.begin() + i );
            return true;
        }
    }
    return false;
}

CNodeMenu* CNode::GetChield( QString name )
{
    if ( name.isEmpty() )
        throw "MenuManager error: name of chield can not be empty";

    for ( int i = 0; i < _nodes.size(); ++i )
        if ( _nodes[i].name == name )
            return _nodes[i].node;

    throw "MenuManager error: Chield \"" + name + "\" has not defined";
}

bool CNode::isMouseInNode( int so_V, int so_H )
{
    POINT M = _MP;
    int PX = _pos.x - so_H;
    int PY = _pos.y - so_V;
    if ( M.x > PX && M.x < PX + _size.x && M.y > PY && M.y < PY + _size.y )
        return true;
    return false;
}

SRColor CNode::ColorBkg()
{
    return _colorBkg;
}

void CNode::SetColorBkg(SRColor color )
{
    _colorBkg = color;
}
