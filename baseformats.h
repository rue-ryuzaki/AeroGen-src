#ifndef BASEFORMATS_H
#define BASEFORMATS_H

#include <QString>
#include <vector>
#include <cmath>
// if M_PI not defined - ex in mingw
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include "figure.h"

using namespace std;

typedef unsigned int uint;

#define ToRadian(x) ((x) * M_PI / 180.0)

struct Sizes {
    Sizes(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) { }
    int x;
    int y;
    int z;
};

struct sPar {
    sPar(uint val, uint count = 0) : val(val), count(count) { }
    uint val;
    uint count;
};

struct Pare {
    Pare(uint a, uint b) : a(a), b(b) { }
    uint a;
    uint b;
};

typedef vector<uint> vui;

enum img_format {
    img_png,
    img_jpg
};

enum txt_format {
    txt_dat,
    txt_dla,
    txt_txt
};

class ImageF {
public:
    ImageF() { }
    ~ImageF() { }
    
    virtual img_format Format() const = 0;
    virtual QString Filter() const = 0;
    virtual QString Ex() const = 0;
    
private:
};

class ImagePNG : public ImageF {
    img_format Format() const { return img_png; }
    QString Filter() const { return "PNG Image (*.png)"; }
    QString Ex() const { return ".png"; }
};

class ImageJPG : public ImageF {
    img_format Format() const { return img_jpg; }
    QString Filter() const { return "JPG Image (*.jpg)"; }
    QString Ex() const { return ".jpg"; }
};

class TextF {
public:
    TextF() { }
    ~TextF() { }
    
    virtual txt_format Format() const = 0;
    virtual QString Filter() const = 0;
    virtual QString Ex() const = 0;
    
private:
};

class TextTXT : public TextF {
    txt_format Format() const { return txt_txt; }
    QString Filter() const { return "TXT format (*.txt)"; }
    QString Ex() const { return ".txt"; }
};

class TextDLA : public TextF {
    txt_format Format() const { return txt_dla; }
    QString Filter() const { return "DLA format (*.dla)"; }
    QString Ex() const { return ".dla"; }
};

class TextDAT : public TextF {
    txt_format Format() const { return txt_dat; }
    QString Filter() const { return "DAT format (*.dat)"; }
    QString Ex() const { return ".dat"; }
};

typedef ImageF* pImgF;
typedef TextF* pTxtF;

#endif /* BASEFORMATS_H */
