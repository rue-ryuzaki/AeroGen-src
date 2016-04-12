#ifndef BASEGENERATOR_H
#define	BASEGENERATOR_H

#include <QGLWidget>
#include "basefield.h"

struct distrib {
    distrib (double r = 0.0, double vol = 0.0) : r(r), vol(vol) { }
    double r;
    double vol;
    double count;
};

class Generator {
public:
    Generator(QObject * parent) : mainwindow(parent) { };
    virtual ~Generator() { delete fld; };
    
    virtual Field * GetField() const { return fld; }
    virtual void    Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize) = 0;

    virtual double  SurfaceArea(double density) = 0;
    virtual void    Density(double density, double & denAero, double & porosity) = 0;

    bool Finished() const { return finished; }
    void Cancel(bool value) { cancel = value; }
    
    virtual void Save(const char * fileName, txt_format format) const = 0;
    virtual void Save(string fileName, txt_format format) const = 0;

    virtual void Load(const char * fileName, txt_format format) = 0;
    virtual void Load(string fileName, txt_format format) = 0;

    bool run = false;

protected:
    int     cube(int val) const { return (val * val * val); }
    double  VfromR(int r) const { return (4.0 / 3) * M_PI * cube(r); }
    double  cube(double val) const { return pow(val, 3.0); }
    double  VfromR(double r) const { return (4.0 / 3) * M_PI * cube(r); }
    double  RfromV(double v) const { return pow((3 * v)/(4 * M_PI), 1.0 / 3.0); }
    double  SfromR(double r) const { return 4.0 * M_PI * r * r; }
    double  RfromS(double s) const { return sqrt(0.25 * s / M_PI); }
    double  VfromD(double d) const { return M_PI * cube(d) / 6; }
    double  SfromR2D(double r) const { return  M_PI * r * r; }
    double  RfromS2D(double s) const { return sqrt(s / M_PI); }
    
    bool    calculated = false;
    bool    finished = false;
    bool    cancel = false;

    QObject * mainwindow;

private:
    Field * fld = nullptr;
};

#endif	/* BASEGENERATOR_H */
