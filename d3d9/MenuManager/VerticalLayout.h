#ifndef VerticalLayout_H
#define VerticalLayout_H

#include "Node.h"
#include <string>

class CVerticalLayout : public CNode
{

public:
    CVerticalLayout( CNodeMenu* parent, POINT = { 0, 0 } );

    virtual void onDraw( int = 0, int = 0 );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual void SetPosition( POINT );
    virtual void SetSize( POINT );

    virtual bool AddChield( CNodeMenu*, const std::string& = "" );
    virtual bool DelChield( CNodeMenu* );
    virtual bool DelChield( const std::string& );

protected:
    void DrawLayout( int = 0, int = 0 );

private:
    bool _Init;
};

#endif // VerticalLayout_H
