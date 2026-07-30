#ifndef Listing_H
#define Listing_H

#include "VerticalLayout.h"
#include <QObject>

class CListing : public CNode
{
    Q_OBJECT

public:
    CListing( POINT, POINT, QObject *parent );
    ~CListing();

    virtual void onDraw( int = 0, int = 0 );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual bool isInizialize();

    virtual void SetSize( POINT );

    virtual bool AddChield( CNodeMenu*, QString = "" );
    virtual bool DelChield( CNodeMenu* );
    virtual bool DelChield( QString );
    virtual CNodeMenu* GetChield( QString );

protected:
    CVerticalLayout* _layout;

private:
    bool _Init;
};

#endif // Listing_H
