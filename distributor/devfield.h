#ifndef DEVFIELD_H
#define DEVFIELD_H

#include "../basefield.h"
#include <vector>

enum {
    d_empty = 0,
    d_mask  = 2,
    d_solid = 4
};

class DevField {
public:
    virtual ~DevField();
    
    static DevField * LoadFromField(const Field * fld, double d);
    double getVolume(double r);
private:
    DevField(Sizes size, double d);
    
    bool overlap(int x, int y, int z, Coord<double>& centre, double r) const;
    double leng(int x, int y, int z, Coord<double>& centre) const;
    int solidCount();
    void maskField(double r);
    long maskCountAndClear();
    void clearMask();
    vector<Coord<int> > createShifts(double r) const; 
    int div;
    
    int *** field;
    // 0 - empty
    // 2 - mask
    // 4 - solid
    int *** mfield;
    Sizes size;
};

#endif /* DEVFIELD_H */
