#ifndef OSM_H
#define	OSM_H

#include <QGLWidget>
#include <iostream>
#include "ofield.h"
#include "../basegenerator.h"

using namespace std;

class OSM : public Generator {
public:
    OSM(QObject * parent);
    virtual ~OSM();
    
    OField * GetField() const;
    void Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize);

    double SurfaceArea(double density);
    void Density(double density, double & denAero, double & porosity);
    
    void Save(const char * fileName, txt_format format) const;
    void Save(string fileName, txt_format format) const;
    
    void Load(const char * fileName, txt_format format);
    void Load(string fileName, txt_format format);
    
private:
    void ReBuild(uint & count, vector<Pare> & pares, vector<sPar>& spars, vector<OCell>& varcells);

    OField * fld = nullptr;
};

#endif	/* OSM_H */
