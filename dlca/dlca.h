#ifndef DLCA_H
#define	DLCA_H

#include <QGLWidget>
#include <iostream>
#include "cfield.h"
#include "../basegenerator.h"

using namespace std;

class DLCA : public Generator {
public:
    DLCA(QObject * parent) : Generator(parent) { };
    virtual ~DLCA() { delete fld; }
    
    CField * GetField() const { return fld; }
    void Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize);
    double SurfaceArea(double density);
    void Density(double density, double & denAero, double & porosity);
    
    void Save(const char * fileName, txt_format format) const { fld->toFile(fileName, format); }
    void Save(string fileName, txt_format format) const { fld->toFile(fileName.c_str(), format); }
    
    void Load(const char * fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new CField(fileName, format);
        finished = true;
    }
    void Load(string fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new CField(fileName.c_str(), format);
        finished = true;
    }
    
private:
    CField * fld = nullptr;
};

#endif	/* DLCA_H */
