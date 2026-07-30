#ifndef NodeMenu_H
#define NodeMenu_H

#include <windows.h>
#include "../d3drender.h"
#include <QObject>
#define CALLBACK __stdcall

//template <class T>
//union bit32{
//    T v;
//    byte b[sizeof( T )];
//};

class CNodeMenu : public QObject
{
    Q_OBJECT

    friend class CContextMenu;
    friend class CListing;
    friend class CMenu;
    friend class CNode;
    friend class CVerticalLayout;
    friend class CSlider;
public:
    CNodeMenu( QObject *parent, POINT = { 0, 0 }, CD3DFont* = nullptr, bool = false );

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

    virtual void SetDescription( QString );
    virtual QString Description();

protected:
    CD3DFont *_font;
    CD3DRender *_draw;
    CNodeMenu *_menu;

    QString _description;
    POINT _pos;
    int _height; // ������ ��������, �������� �� �����������
    int _width; // ����� ��������, �������� �� �����������
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

signals:
    void eventShow(CNodeMenu*, bool);
    void eventMove(CNodeMenu*, POINT);
    void eventClick(CNodeMenu*, UINT);
};

#endif // NodeMenu_H
