#ifndef BASEFORMATS_H
#define BASEFORMATS_H

#include <cstdint>
#include <cmath>
#include <vector>
#include <stdexcept>
// if M_PI not defined - ex in mingw
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <QString>

#include "figure.h"

#define ToRadian(x) ((x) * M_PI / 180.0)

struct Sizes
{
    Sizes(uint32_t x = 0, uint32_t y = 0, uint32_t z = 0)
        : x(x),
          y(y),
          z(z)
    { }

    const uint32_t& operator[] (size_t index) const
    {
        switch (index) {
            case 0 :
                return x;
            case 1 :
                return y;
            case 2 :
                return z;
            default :
                throw std::invalid_argument("invalid Sizes index");
        }
    }

    inline uint32_t volume() const
    {
        return x * y * z;
    }

    uint32_t x;
    uint32_t y;
    uint32_t z;
};

struct sPar
{
    sPar(uint32_t val, uint32_t count = 0)
        : val(val),
          count(count)
    { }

    uint32_t val;
    uint32_t count;
};

struct Pare
{
    Pare(uint32_t a, uint32_t b)
        : a(a),
          b(b)
    { }

    uint32_t a;
    uint32_t b;
};

typedef std::vector<uint32_t> vui;

enum img_format {
    img_png,
    img_jpg
};

enum txt_format {
    txt_dat,
    txt_dla,
    txt_txt
};

struct ImageF
{
    virtual ~ImageF() { }
    
    virtual img_format Format() const = 0;
    virtual QString    Filter() const = 0;
    virtual QString    Ex()     const = 0;
};

class ImagePNG : public ImageF
{
public:
    img_format Format() const override { return img_png; }
    QString    Filter() const override { return "PNG Image (*.png)"; }
    QString    Ex()     const override { return ".png"; }
};

class ImageJPG : public ImageF
{
public:
    img_format Format() const override { return img_jpg; }
    QString    Filter() const override { return "JPG Image (*.jpg)"; }
    QString    Ex()     const override { return ".jpg"; }
};

struct TextF
{
    virtual ~TextF() { }
    
    virtual txt_format Format() const = 0;
    virtual QString    Filter() const = 0;
    virtual QString    Ex()     const = 0;
};

class TextTXT : public TextF
{
public:
    txt_format Format() const override { return txt_txt; }
    QString    Filter() const override { return "TXT format (*.txt)"; }
    QString    Ex()     const override { return ".txt"; }
};

class TextDLA : public TextF
{
public:
    txt_format Format() const override { return txt_dla; }
    QString    Filter() const override { return "DLA format (*.dla)"; }
    QString    Ex()     const override { return ".dla"; }
};

class TextDAT : public TextF
{
public:
    txt_format Format() const override { return txt_dat; }
    QString    Filter() const override { return "DAT format (*.dat)"; }
    QString    Ex()     const override { return ".dat"; }
};

typedef ImageF* pImgF;
typedef TextF* pTxtF;

#endif // BASEFORMATS_H
