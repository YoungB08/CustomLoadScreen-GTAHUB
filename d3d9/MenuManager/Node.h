#ifndef Node_H
#define Node_H

#include "NodeMenu.h"
#include "../texture.h"
#include <QVector>
#include <QObject>

struct stNodeChield{
    CNodeMenu*	node;
    QString name;
};

class CNode : public CNodeMenu
{
    Q_OBJECT

public:
    CNode( QObject *parent, POINT = { 0, 0 }, POINT = { 100, 100 } );
    virtual ~CNode();

    virtual void onDraw( int = 0, int = 0 );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual bool isInizialize();

    virtual void SetSize( POINT );
    virtual POINT Size();

    virtual int ScrollVertical();
    virtual void SetScrollVertical( int );
    virtual int ScrollHorizontal();
    virtual void SetScrollHorizontal( int );

    virtual void SetScrollStep( int );
    virtual void ScrollColor( SRColor&, SRColor&, SRColor& );
    virtual void SetScrollColor( SRColor, SRColor, SRColor );

    virtual bool AddChield( CNodeMenu*, QString = "" );
    virtual bool DelChield( CNodeMenu* );
    virtual bool DelChield( QString );
    virtual CNodeMenu* GetChield( QString );

    virtual SRColor ColorBkg();
    virtual void SetColorBkg( SRColor );

protected:
    SRTexture *_texture;
    QVector<stNodeChield> _nodes;

    POINT _size;
    int _scrollOffsetVertical;
    int _scrollOffsetHorizontal;
    int _scrollSizeVertical;
    int _scrollSizeHorizontal;
    int _scrollStep;
    SRColor _scrollFrame;
    SRColor _scrollMat;
    SRColor _scrollRoller;

    SRColor _colorBkg;

    void DrawScrollBarVertical( int = 0, int = 0 );
    void DrawScrollBarHorizontal( int = 0, int = 0 );
    bool isMouseInNode( int = 0, int = 0 );

private:
    bool _Init;
};

#endif // Node_H
