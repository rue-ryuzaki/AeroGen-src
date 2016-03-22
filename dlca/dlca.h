#ifndef DLCA_H
#define	DLCA_H

#include <QGLWidget>
#include <iostream>
#include "cfield.h"
#include "../basegenerator.h"

using namespace std;

class DLCA : public Generator {
public:
    DLCA(QObject * parent);
    virtual ~DLCA();
    
    CField * GetField() const;
    void Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize);

    double SurfaceArea(double density);
    void Density(double density, double & denAero, double & porosity);
    
    void Save(const char * fileName, txt_format format) const;
    void Save(string fileName, txt_format format) const;
    
    void Load(const char * fileName, txt_format format);
    void Load(string fileName, txt_format format);
    
private:
    CField * fld = nullptr;
};

#endif	/* DLCA_H */
