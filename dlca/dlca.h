#ifndef DLCA_H
#define	DLCA_H

#include "cfield.h"
#include "../basegenerator.h"

class DLCA : public Generator
{
public:
    DLCA(QObject* parent);
    virtual ~DLCA();
    
    CField* field() const;
    void    generate(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                     uint32_t hit, uint32_t cluster, double cellsize);

    double  surfaceArea(double density) const;
    void    density(double density, double& denAero, double& porosity) const;
    
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);
    
private:
    CField* m_fld = nullptr;
};

#endif	// DLCA_H
