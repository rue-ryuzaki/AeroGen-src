#ifndef AEROGEN_DLCA2_DLCA2_H
#define AEROGEN_DLCA2_DLCA2_H

#include "field.h"
#include "../basegenerator.h"

class DLCA2 : public Generator
{
public:
    DLCA2(QObject* parent);
    virtual ~DLCA2();

    dlca2::XField* field() const;
    void    generate(const Sizes& sizes, const RunParams& params);
    double  surfaceArea(double density, uint32_t steps) const;
    void    density(double density, double& denAero, double& porosity) const;
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);

private:
    dlca2::XField* m_fld = nullptr;

    DLCA2(const DLCA2&) = delete;
    DLCA2& operator =(const DLCA2&) = delete;
};

#endif // AEROGEN_DLCA2_DLCA2_H
