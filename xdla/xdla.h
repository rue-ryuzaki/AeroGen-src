#ifndef XDLA_H
#define	XDLA_H

#include <QGLWidget>
#include <string>

#include "xfield.h"
#include "../basegenerator.h"

class xDLA : public Generator
{
public:
    xDLA(QObject* parent) : Generator(parent) { }
    virtual ~xDLA() { delete fld; }
    
    xField* GetField() const { return fld; }
    void Generate(const Sizes& sizes, double por, int initial, int step, int hit,
                  size_t cluster, double cellsize);
    double SurfaceArea(double density) const;
    void Density(double density, double& denAero, double& porosity) const;
    
    void Save(const char* fileName, txt_format format) const { fld->toFile(fileName, format); }
    
    void Load(const char* fileName, txt_format format)
    {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new xField(fileName, format);
        finished = true;
    }

private:
    xField* fld = nullptr;
};

#endif	// XDLA_H
