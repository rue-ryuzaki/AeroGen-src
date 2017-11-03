#ifndef XDLA_XFIELD_H
#define XDLA_XFIELD_H

#include <vector>

#include "../basefield.h"

class xField : public Field
{
public:
    xField(const char* fileName, txt_format format);
    xField(const Sizes& sizes = Sizes(50, 50, 50));
    virtual ~xField() { }

    Sizes       sizes() const;
    void        initialize(double porosity, double cellsize);
    std::vector<Cell> cells() const;
    uint32_t    monteCarlo(uint32_t stepMax) const;

private:
    void    toDAT(const char* fileName) const;
    void    toDLA(const char* fileName) const;
    void    toTXT(const char* fileName) const;
    void    fromDAT(const char* fileName);
    void    fromDLA(const char* fileName);
    void    fromTXT(const char* fileName);
};

#endif // XDLA_XFIELD_H
