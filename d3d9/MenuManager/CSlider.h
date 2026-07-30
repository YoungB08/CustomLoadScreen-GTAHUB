#ifndef CSLIDER_H
#define CSLIDER_H

#include "Text.h"
#include "../texture.h"
#include <QObject>

class CSlider : public CNodeMenu
{
    Q_OBJECT
public:
    CSlider(QObject *parent, POINT pos, uint length, float start = 1.0f, float end = 99.0f, float value = 1.0f, CD3DFont* = nullptr);

    virtual ~CSlider();

    virtual bool isInizialize();

    virtual void onDraw( int, int );
    virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

    virtual void setValue( float );
    virtual float value();

    virtual void setColors( SRColor, SRColor, SRColor );
    virtual void getColors( SRColor*, SRColor*, SRColor* );

protected:
    SRColor _roller;
    SRColor _mat;
    SRColor _text;
    SRTexture *_texture;
    uint _length;
    float _start;
    float _end;
    float _value;
    bool _moveRoller;
    int _rollerX;

    virtual bool isMouseOnSlider( int = 0, int = 0 );

private:
    bool _init;

signals:
    void eventValueChanged(CSlider*, float);
    void eventValueEdited(CSlider*, float);
};

#endif // CSLIDER_H
