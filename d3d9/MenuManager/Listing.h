#ifndef Listing_H
#define Listing_H

#include "VerticalLayout.h"
#include <string>

class CListing : public CNode
{

public:
    CListing( POINT, POINT, CNodeMenu* parent );
    ~CListing();

    virtual void onDraw( int = 0, int = 0 );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual bool isInizialize();

    virtual void SetSize( POINT );

    virtual bool AddChield( CNodeMenu*, const std::string& = "" );
    virtual bool DelChield( CNodeMenu* );
    virtual bool DelChield( const std::string& );
    virtual CNodeMenu* GetChield( const std::string& );

protected:
    CVerticalLayout* _layout;

private:
    bool _Init;
};

#endif // Listing_H
