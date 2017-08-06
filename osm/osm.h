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
    
    OField* field() const;
    void    generate(const Sizes& sizes, double por, int initial, int step, int hit,
                     size_t cluster, double cellsize);

    double  surfaceArea(double density) const;
    void    density(double density, double& denAero, double& porosity) const;
    
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);
    
private:
    void    reBuild(uint& count, std::vector<Pare>& pares, std::vector<sPar>& spars,
                    std::vector<OCell>& varcells);

    OField* m_fld = nullptr;
};

#endif	// OSM_H
