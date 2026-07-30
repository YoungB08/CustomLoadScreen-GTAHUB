#ifndef COLOR_H
#define COLOR_H


enum ColorDominater{
    eCdBlack,           // 000000
    eCdGray,            // 808080
    eCdWhite,           // FFFFFF
    eCdRed,             // FF0000
    eCdGreen,           // 00FF00
    eCdBlue,            // 0000FF
    eCdYellow,          // FFFF00
    eCdCyan,            // 00FFFF
    eCdMagenta,         // FF00FF
    eCdOrange,          // FF8000
    eCdLimeGreen,       // 80FF00
    eCdCyanBlue,        // 0080FF
    eCdLiteGreen,       // 00FF80
    eCdManganous,       // FF0080
    eCdPurple,          // 8000FF
    eCdUnknown
};

struct SRColor{
    union{
        struct{
            unsigned char blue;
            unsigned char green;
            unsigned char red;
            unsigned char alpha;
        };
        unsigned int argb;
    };

    SRColor(unsigned int clr_ = 0xFFFFFFFF);
    SRColor(int red_, int green_, int blue_);
    SRColor(int alpha_, int red_, int green_, int blue_);
    SRColor(float red_, float green_, float blue_);
    SRColor(float alpha_, float red_, float green_, float blue_);
    SRColor(SRColor const &clr_);
    SRColor(ColorDominater dominater, unsigned char alpha_ = 0xFF);

    ColorDominater dominater();
    void setDominater(ColorDominater dominater);

    SRColor invert();
};

#endif // COLOR_H
