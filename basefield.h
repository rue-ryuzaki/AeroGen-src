#ifndef BASEFIELD_H
#define	BASEFIELD_H

#include <cstdint>
#include <vector>

#include "basecell.h"

#define NitroDiameter 0.34

template <class T>
T cube(T value)
{
    return value * value * value;
}
template <class T>
double VfromR(T value)
{
    return (4.0 / 3.0) * M_PI * double(cube(value));
}
inline double   RfromV(double v) { return pow((3.0 * v) / (4.0 * M_PI), 1.0 / 3.0); }
inline double   SfromR(double r) { return 4.0 * M_PI * r * r; }
inline double   RfromS(double s) { return sqrt(0.25 * s / M_PI); }
inline double   VfromD(double d) { return M_PI * cube(d) / 6.0; }
inline double   SfromR2D(double r) { return  M_PI * r * r; }
inline double   RfromS2D(double s) { return sqrt(s / M_PI); }
inline double   square(double x) { return x * x; }
inline double   Dmin(double d, double psi) { return d * sqrt(1.0 - psi * psi); }

class Field
{
public:
    Field(const char* fileName, txt_format format) : m_sizes() { }

    explicit Field(Sizes sizes = Sizes(50, 50, 50)) : m_sizes(sizes) { }
    virtual ~Field() { }
    
    virtual Sizes             sizes() const = 0;
    virtual std::vector<Cell> cells() const = 0;

    virtual void initialize(double porosity, double cellsize) = 0;
    virtual uint32_t monteCarlo(uint32_t stepMax) = 0;

    void toFile(const char* fileName, txt_format format) const;

protected:
    virtual void toDAT(const char* fileName) const = 0;
    virtual void toDLA(const char* fileName) const = 0;
    virtual void toTXT(const char* fileName) const = 0;
    virtual void fromDAT(const char* fileName) = 0;
    virtual void fromDLA(const char* fileName) = 0;
    virtual void fromTXT(const char* fileName) = 0;
    
    Sizes   m_sizes;
};

#endif	// BASEFIELD_H
