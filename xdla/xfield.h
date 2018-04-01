#ifndef AEROGEN_XDLA_XFIELD_H
#define AEROGEN_XDLA_XFIELD_H

#include <vector>

#include "../basefield.h"

class xField : public Field
{
public:
    xField(const char* fileName, txt_format format);
    xField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = false);
    virtual ~xField() { }

    Sizes       sizes() const;
    void        initialize(double porosity, double cellsize);
    std::vector<Cell> cells() const;
    uint32_t    monteCarlo(uint32_t stepMax) const;

private:
    void    toDAT(const char* fileName) const override;
    void    toDLA(const char* fileName) const override;
    void    toTXT(const char* fileName) const override;
    void    fromDAT(const char* fileName) override;
    void    fromDLA(const char* fileName) override;
    void    fromTXT(const char* fileName) override;
};

#endif // AEROGEN_XDLA_XFIELD_H
