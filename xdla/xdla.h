#ifndef AEROGEN_XDLA_XDLA_H
#define AEROGEN_XDLA_XDLA_H

#include "xfield.h"
#include "../basegenerator.h"

class xDLA : public Generator
{
public:
    xDLA(QObject* parent);
    virtual ~xDLA();

    xField* field() const;
    void    generate(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                     uint32_t hit, uint32_t cluster, double cellsize);
    double  surfaceArea(double density) const;
    void    density(double density, double& denAero, double& porosity) const;
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);

private:
    xField* m_fld = nullptr;

    xDLA(const xDLA&) = delete;
    xDLA& operator =(const xDLA&) = delete;
};

#endif // AEROGEN_XDLA_XDLA_H
