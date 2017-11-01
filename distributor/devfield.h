#ifndef DEVFIELD_H
#define DEVFIELD_H

#include <vector>

#include "../basefield.h"

//#define FMASK

class DevField
{
    enum {
        d_empty = 0,
        d_mask  = 2,
        d_solid = 4
    };

public:
    virtual ~DevField();
    
    static DevField* loadFromField(const Field* fld, double d);
    double  volume(double r);

private:
    DevField(Sizes m_size, double d);
    
    static bool   overlap(int32_t x, int32_t y, int32_t z, const dCoord& centre, double r);
    static double leng(int32_t x, int32_t y, int32_t z, const dCoord& centre);
    uint32_t    solidCount() const;
    void        maskField(double r);
    uint32_t    maskCountAndClear();
    void        clearMask();
    std::vector<iCoord> createShifts(double r) const;

    uint16_t    m_div;
    uint8_t***  m_field;
    // 0 - empty
    // 2 - mask
    // 4 - solid
#ifdef FMASK
    int16_t***  m_mask;
#endif
    Sizes       m_size;

    DevField(const DevField&) = delete;
    DevField& operator =(const DevField&) = delete;
};

#endif // DEVFIELD_H
