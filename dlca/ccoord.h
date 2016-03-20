#ifndef DLCA_COORD_H
#define	DLCA_COORD_H

#include "../basecoord.h"

class CCoord {
public:
    CCoord (double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) { }
    virtual ~CCoord() { }
    
    CCoord operator* (const double &) const;
    CCoord operator* (const unsigned int &) const;
    CCoord operator/ (const unsigned int &) const;
    CCoord operator+ (const CCoord &) const;
    CCoord operator- (const CCoord &) const;
    CCoord operator+ (const Sizes &) const;
    CCoord operator- (const Sizes &) const;
    CCoord & operator=(const CCoord & rhs);
    double x;
    double y;
    double z;
private:
};

typedef CCoord CVector3;

#endif	/* DLCA_COORD_H */
