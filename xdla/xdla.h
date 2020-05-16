#ifndef AEROGEN_XDLA_XDLA_H
#define AEROGEN_XDLA_XDLA_H

#include "field.h"
#include "../basegenerator.h"

class xDLA : public Generator
{
public:
    xDLA(QObject* parent);
    virtual ~xDLA();

    xdla::XField* field() const;
    void    generate(const Sizes& sizes, const RunParams& params);
    double  surfaceArea(double density, uint32_t steps) const;
    void    density(double density, double& denAero, double& porosity) const;
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);

private:
    xdla::XField* m_fld = nullptr;

    xDLA(const xDLA&) = delete;
    xDLA& operator =(const xDLA&) = delete;
};

#endif // AEROGEN_XDLA_XDLA_H
