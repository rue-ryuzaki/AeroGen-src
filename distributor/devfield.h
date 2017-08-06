#ifndef DEVFIELD_H
#define DEVFIELD_H

#include "../basefield.h"
#include <vector>

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
    
    static bool   overlap(int x, int y, int z, Coord<double>& centre, double r);
    static double leng(int x, int y, int z, Coord<double>& centre);
    int     solidCount() const;
    void    maskField(double r);
    long    maskCountAndClear();
    void    clearMask();
    std::vector<iCoord> createShifts(double r) const;

    int     m_div;
    int***  m_field;
    // 0 - empty
    // 2 - mask
    // 4 - solid
    int***  m_mask;
    Sizes   m_size;
};

#endif // DEVFIELD_H
