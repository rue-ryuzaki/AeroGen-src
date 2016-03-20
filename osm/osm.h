#ifndef OSM_H
#define	OSM_H

#include <QGLWidget>
#include <iostream>
#include "ofield.h"
#include "../basegenerator.h"

using namespace std;

class OSM : public Generator {
public:
    OSM(QObject * parent) : Generator(parent) { };
    virtual ~OSM() { delete fld; }
    
    OField * GetField() const { return fld; }
    void Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize);
    double SurfaceArea(double density);
    void Density(double density, double & denAero, double & porosity);
    
    void Save(const char * fileName, txt_format format) const { fld->toFile(fileName, format); }
    void Save(string fileName, txt_format format) const { fld->toFile(fileName.c_str(), format); }
    
    void Load(const char * fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new OField(fileName, format);
        finished = true;
    }
    void Load(string fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new OField(fileName.c_str(), format);
        finished = true;
    }
    
private:
    OField * fld = nullptr;
    void ReBuild(uint & count, vector<Pare> & pares, vector<sPar>& spars, vector<OCell>& varcells);
};

#endif	/* OSM_H */
