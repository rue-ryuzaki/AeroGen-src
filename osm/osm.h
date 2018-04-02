#ifndef AEROGEN_OSM_OSM_H
#define AEROGEN_OSM_OSM_H

#include <vector>

#include "ofield.h"
#include "../basegenerator.h"

class OSM : public Generator
{
public:
    OSM(QObject* parent);
    virtual ~OSM();
    
    OField* field() const;
    void    generate(const Sizes& sizes, const RunParams& params);
    double  surfaceArea(double density, uint32_t steps) const;
    void    density(double density, double& denAero, double& porosity) const;
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);
    
private:
    void    reBuild(uint32_t& count, std::vector<Pare>& pares, std::vector<sPar>& spars,
                    std::vector<OCell>& cells);

    OField* m_fld = nullptr;

    OSM(const OSM&) = delete;
    OSM& operator =(const OSM&) = delete;
};

#endif // AEROGEN_OSM_OSM_H
