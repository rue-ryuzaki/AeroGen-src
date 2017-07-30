#ifndef OSM_H
#define	OSM_H

#include <QGLWidget>
#include <string>
#include <vector>

#include "ofield.h"
#include "../basegenerator.h"

class OSM : public Generator
{
public:
    OSM(QObject* parent);
    virtual ~OSM();
    
    OField* GetField() const;
    void Generate(const Sizes& sizes, double por, int initial, int step, int hit,
                  size_t cluster, double cellsize);

    double SurfaceArea(double density) const;
    void Density(double density, double& denAero, double& porosity) const;
    
    void Save(const char* fileName, txt_format format) const;
    void Load(const char* fileName, txt_format format);
    
private:
    void ReBuild(uint& count, std::vector<Pare>& pares, std::vector<sPar>& spars, std::vector<OCell>& varcells);

    OField* fld = nullptr;
};

#endif	// OSM_H
