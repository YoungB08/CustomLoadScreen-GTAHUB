#include "Text.h"

CText::CText(const std::string& text, SRColor color, CNodeMenu* parent, POINT pos, CD3DFont *font, bool deleteOnDestructor ) : CNodeMenu( parent, pos, font, deleteOnDestructor )
{
    if (parent == nullptr)
        return;
    _text = text;
    _color = color;
    _description = "";
    SetSelectable( false );

    _Init = false;
}

bool CText::isInizialize()
{
    if ( _Init )
        return true;

    if ( g_class.DirectX->d3d9_device() == nullptr )
        return false;

    if ( !CNodeMenu::isInizialize() )
        return false;

    _height = _font->DrawHeight();
    _width = _font->DrawLength( _text );

    _Init = true;
    return false;
}

void CText::onDraw( int so_V, int so_H )
{
    if ( !isInizialize() )
        return;

    _font->PrintShadow( _pos.x - so_H, _pos.y - so_V, _color, _text );

    CNodeMenu::onDraw( so_V, so_H );
}

void CText::setText( const std::string& text )
{
    _text = text;
    _width = _font->DrawLength( _text );
    
}

std::string CText::text()
{
    return _text;
}

void CText::setColor( SRColor color )
{
    _color = color;
    
}

SRColor CText::color()
{
    return _color;
}


