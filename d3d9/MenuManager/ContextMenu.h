#ifndef ContextMenu_H
#define ContextMenu_H

#include <QVector>
#include "NodeMenu.h"
#include "Text.h"
//#include "Menu.h"
#include <QObject>

class CContextMenu : public CNodeMenu
{
    Q_OBJECT

public:
    CContextMenu( QObject *parent, SRColor = 0xE8283848 );
    virtual ~CContextMenu();

    virtual void onDraw( int = 0, int = 0 );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual bool isInizialize();

    virtual void SetPosition( POINT );

    virtual void AddVariant( QString, SRColor = -1 );
    virtual void SetDescription( QString );

protected:
    QVector<CText*> _vars;

    SRColor _color;

    bool isMouseOnVariant( int );

private:
    bool _Init;

signals:
    void eventContextMenu(CContextMenu*, int);
};

#endif // ContextMenu_H
