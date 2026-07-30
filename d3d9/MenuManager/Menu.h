#ifndef Menu_H
#define Menu_H

#define SCREEN_X *(int*)0x00C9C040
#define SCREEN_Y *(int*)0x00C9C044

#include "Node.h"
#include "ContextMenu.h"
#include <QRegExp>
#include <QVector>
#include <QObject>

template<typename T> void VectorErase( std::vector<T> &vec, T v );
template<typename T> void myswap( T &a, T &b );

class CMenu;
extern std::vector<CMenu*> MenuList;

class CMenu : public CNodeMenu
{
    Q_OBJECT

    friend class CNodeMenu;
public:
    CMenu( QString, POINT, QObject *parent = 0, CNode* = nullptr );
    ~CMenu();

    virtual void onDraw( int = 0, int = 0 );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual bool isInizialize();

    virtual void SetSize( POINT );
    virtual POINT Size();

    virtual CNode* Node();

    virtual void SetContextMenu( CContextMenu* );

    virtual bool isActive();

protected:
    SRTexture *_texture;
    CNode *_node;
    CContextMenu *_context;

    QString _title;
    POINT _size;
    int _header;

    bool isMouseOnHeader();
    bool isMouseOnClose();

    virtual void SetMenuHelper( QString );

    QVector<QString> p_helper;

private:
    bool _Init;

    bool _move;
    POINT _mvOffset;
};

void SetForeground( CMenu* menu );
bool IsForeground( CMenu*menu );

template<typename T>
inline void VectorErase( std::vector<T> &vec, T v )
{
    for ( int i = 0; i < vec.size(); ++i ){
        if ( vec[i] == v ){
            vec.erase( vec.begin() + i );
            break;
        }
    }
}

template<typename T>
inline void myswap( T &a, T &b )
{
    T c = a;
    a = b;
    b = c;
}

#endif // Menu_H
