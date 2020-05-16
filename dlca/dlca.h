#ifndef AEROGEN_DLCA_DLCA_H
#define AEROGEN_DLCA_DLCA_H

#include "field.h"
#include "../basegenerator.h"

class DLCA : public Generator
{
public:
    DLCA(QObject* parent);
    virtual ~DLCA();

    dlca::XField* field() const;
    void    generate(const Sizes& sizes, const RunParams& params);
    double  surfaceArea(double density, uint32_t steps) const;
    void    density(double density, double& denAero, double& porosity) const;
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);

private:
    dlca::XField* m_fld = nullptr;

    DLCA(const DLCA&) = delete;
    DLCA& operator =(const DLCA&) = delete;
};

#endif // AEROGEN_DLCA_DLCA_H
