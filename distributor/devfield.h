#ifndef AEROGEN_DISTRIBUTOR_DEVFIELD_H
#define AEROGEN_DISTRIBUTOR_DEVFIELD_H

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
    
    static DevField* loadFromField(const Field* fld, double d, bool isToroid);
    double  volume(double r);

private:
    DevField(const Sizes& size, double d, bool isToroid);

    uint32_t    solidCount() const;
    void        maskField(double r);
    uint32_t    maskCountAndClear();
    void        clearMask();
    std::vector<iCoord> createShifts(double r) const;

    uint16_t    m_div;
    std::vector<std::vector<std::vector<uint8_t> > > m_field;
    // 0 - empty
    // 2 - mask
    // 4 - solid
#ifdef FMASK
    std::vector<std::vector<std::vector<uint16_t> > >  m_mask;
#endif
    bool        m_isToroid;

    DevField(const DevField&) = delete;
    DevField& operator =(const DevField&) = delete;
};

#endif // AEROGEN_DISTRIBUTOR_DEVFIELD_H
