#ifndef XDLA_H
#define	XDLA_H

#include <QGLWidget>
#include <string>

#include "xfield.h"
#include "../basegenerator.h"

class xDLA : public Generator {
public:
    xDLA(QObject * parent) : Generator(parent) { }
    virtual ~xDLA() { delete fld; }
    
    xField * GetField() const { return fld; }
    void Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize);
    double SurfaceArea(double density);
    void Density(double density, double & denAero, double & porosity);
    
    void Save(const char * fileName, txt_format format) const { fld->toFile(fileName, format); }
    void Save(std::string fileName, txt_format format) const { fld->toFile(fileName.c_str(), format); }
    
    void Load(const char * fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new xField(fileName, format);
        finished = true;
    }
    void Load(std::string fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new xField(fileName.c_str(), format);
        finished = true;
    }
private:
    xField * fld = nullptr;
};

#endif	/* XDLA_H */
