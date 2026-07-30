#ifndef Text_H
#define Text_H

#include "NodeMenu.h"
#include <QObject>

class CText : public CNodeMenu
{
    Q_OBJECT

public:
    CText( QString, SRColor, QObject *parent, POINT = { 0, 0 }, CD3DFont* = nullptr, bool = false );

    virtual bool isInizialize();

    virtual void onDraw( int = 0, int = 0 );

    virtual void setText( QString );
    virtual QString text();

    virtual void setColor( SRColor );
    virtual SRColor color();

protected:
    QString _text;
    SRColor _color;

private:
    bool _Init;

signals:
    void eventTextChanged(CText*, QString);
    void eventColorChanged(CText*, SRColor);
};

#endif // Text_H
