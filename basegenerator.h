#ifndef BASEGENERATOR_H
#define	BASEGENERATOR_H

#include <QGLWidget>
#include <string>

#include "basefield.h"

struct distrib
{
    distrib (double r = 0.0, double vol = 0.0)
        : r(r),
          vol(vol),
          count()
    { }

    double r;
    double vol;
    double count;
};

class Generator {
public:
    Generator(QObject * parent) : mainwindow(parent) { }
    virtual ~Generator() { delete fld; }
    
    virtual Field * GetField() const { return fld; }
    virtual void    Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize) = 0;

    virtual double  SurfaceArea(double density) = 0;
    virtual void    Density(double density, double & denAero, double & porosity) = 0;

    bool Finished() const { return finished; }
    void Cancel(bool value) { cancel = value; }
    
    virtual void Save(const char * fileName, txt_format format) const = 0;
    virtual void Save(std::string fileName, txt_format format) const = 0;

    virtual void Load(const char * fileName, txt_format format) = 0;
    virtual void Load(std::string fileName, txt_format format) = 0;

    bool run = false;

protected:
    static int     cube(int val) { return (val * val * val); }
    static double  VfromR(int r) { return (4.0 / 3) * M_PI * cube(r); }
    static double  cube(double val) { return pow(val, 3.0); }
    static double  VfromR(double r) { return (4.0 / 3) * M_PI * cube(r); }
    static double  RfromV(double v) { return pow((3 * v)/(4 * M_PI), 1.0 / 3.0); }
    static double  SfromR(double r) { return 4.0 * M_PI * r * r; }
    static double  RfromS(double s) { return sqrt(0.25 * s / M_PI); }
    static double  VfromD(double d) { return M_PI * cube(d) / 6; }
    static double  SfromR2D(double r) { return  M_PI * r * r; }
    static double  RfromS2D(double s) { return sqrt(s / M_PI); }
    
    bool    calculated = false;
    bool    finished   = false;
    bool    cancel     = false;

    QObject * mainwindow;

private:
    Field * fld = nullptr;
};

#endif	/* BASEGENERATOR_H */
