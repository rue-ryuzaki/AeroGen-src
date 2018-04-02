#ifndef AEROGEN_MXDLA_MXDLA_H
#define AEROGEN_MXDLA_MXDLA_H

#include "mxfield.h"
#include "../basegenerator.h"

class MxDLA : public Generator
{
public:
    MxDLA(QObject* parent);
    virtual ~MxDLA();

    MxField* field() const;
    void    generate(const Sizes& sizes, const RunParams& params);
    double  surfaceArea(double density, uint32_t steps) const;
    void    density(double density, double& denAero, double& porosity) const;
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);

private:
    MxField* m_fld = nullptr;

    MxDLA(const MxDLA&) = delete;
    MxDLA& operator =(const MxDLA&) = delete;
};

#endif // AEROGEN_MXDLA_MXDLA_H
