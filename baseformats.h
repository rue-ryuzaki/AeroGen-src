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

template <class T>
void cleanEmptyClusters(std::vector<std::vector<T> >& arr)
{
    arr.erase(std::remove_if(arr.begin(), arr.end(),
                            [] (std::vector<T>& t) { return t.empty(); }),
              arr.end());
}

inline void inPareList(std::vector<std::vector<uint32_t> >& agregate, const Pare& pare)
{
    std::vector<uint32_t> agr;
    std::vector<uint32_t> lagr;
    for (uint32_t i = 0; i < agregate.size(); ++i) {
        for (const uint32_t& ui : agregate[i]) {
            if (pare.a == ui || pare.b == ui) {
                agr.push_back(ui);
                lagr.push_back(i);
            }
        }
    }

    switch (lagr.size()) {
        case 0 :
            agregate.push_back({ pare.a, pare.b });
            break;
        case 1 :
            // need include 1 cell
            for (const uint32_t& ui : agregate[lagr[0]]) {
                if (pare.a == ui) {
                    agregate[lagr[0]].push_back(pare.b);
                    break;
                }
                if (pare.b == ui) {
                    agregate[lagr[0]].push_back(pare.a);
                    break;
                }
            }
            break;
        case 2 :
            if (lagr[0] != lagr[1]) {
                // both in different clusters
                agregate[lagr[0]].reserve(agregate[lagr[0]].size() + agregate[lagr[1]].size());
                agregate[lagr[0]].insert(agregate[lagr[0]].end(), agregate[lagr[1]].begin(), agregate[lagr[1]].end());
                agregate[lagr[1]].clear();
                // clean empty clusters
                cleanEmptyClusters(agregate);
                break;
            }
    }
}

template <class T>
void agregateClusters(std::vector<std::vector<T> >& arr, const std::vector<Pare>& pares)
{
    std::vector<std::vector<uint32_t> > agregate;

    for (const Pare& p : pares) {
        inPareList(agregate, p);
    }

    // check more then 2 cluster agregation!

    for (const std::vector<uint32_t>& vu : agregate) {
        size_t cnt = vu.size();

        size_t smax = 0;
        size_t imax = 0;

        for (size_t i = 0; i < cnt; ++i) {
            size_t ms = arr[vu[i]].size();
            if (smax < ms) {
                smax = ms;
                imax = i;
            }
        }

        // agregation in 1 cluster
        for (size_t i = 0; i < cnt; ++i) {
            if (i != imax) {
                arr[vu[imax]].reserve(arr[vu[imax]].size() + arr[vu[i]].size());
                copy(arr[vu[i]].begin(), arr[vu[i]].end(), back_inserter(arr[vu[imax]]));
                arr[vu[i]].clear();
            }
        }
    }

    cleanEmptyClusters(arr);
}

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
