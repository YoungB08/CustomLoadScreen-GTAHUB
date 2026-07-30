#ifndef VerticalLayout_H
#define VerticalLayout_H

#include "Node.h"
#include <QObject>

class CVerticalLayout : public CNode
{
    Q_OBJECT

public:
    CVerticalLayout( QObject *parent, POINT = { 0, 0 } );

    virtual void onDraw( int = 0, int = 0 );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual void SetPosition( POINT );
    virtual void SetSize( POINT );

    virtual bool AddChield( CNodeMenu*, QString = "" );
    virtual bool DelChield( CNodeMenu* );
    virtual bool DelChield( QString );

protected:
    void DrawLayout( int = 0, int = 0 );

private:
    bool _Init;
};

#endif // VerticalLayout_H
