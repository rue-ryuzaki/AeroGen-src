#ifndef BASEFIELD_H
#define	BASEFIELD_H

#include "basecell.h"
#include <vector>

#define NitroDiameter 0.34

class Field {
public:
    Field(const char * fileName, txt_format format) { }
    Field(Sizes sizes = Sizes(50, 50, 50)) : sizes(sizes) { }
    virtual ~Field() { }
    
    virtual Sizes getSizes() const = 0;
    virtual vector<Cell> getCells() const = 0;

    virtual void Initialize(double porosity, double cellsize) = 0;
    virtual int MonteCarlo(int stepMax) = 0;

    void toFile(const char * fileName, txt_format format) const;

protected:
    virtual void toDAT(const char * fileName) const = 0;
    virtual void toDLA(const char * fileName) const = 0;
    virtual void toTXT(const char * fileName) const = 0;
    virtual void fromDAT(const char * fileName) = 0;
    virtual void fromDLA(const char * fileName) = 0;
    virtual void fromTXT(const char * fileName) = 0;

    double cube(double val) const { return pow(val, 3.0); }
    double VfromR(double r) const { return (4.0 / 3) * M_PI * cube(r); }
    double VfromD(double d) const { return M_PI * cube(d) / 6; }
    double square(double x) const { return x * x; }
    double Dmin(double d, double psi) const { return d * pow((1 - psi * psi), 0.5); }
    
    Sizes sizes;
};

#endif	/* BASEFIELD_H */
