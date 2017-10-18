#ifndef MXFIELD_H
#define MXFIELD_H

#include <vector>

#include "../basefield.h"

class MxField : public Field
{
public:
    MxField(const char* fileName, txt_format format);
    MxField(Sizes m_sizes = Sizes(50, 50, 50));
    virtual ~MxField() { }

    Sizes       sizes() const;
    void        initialize(double porosity, double cellsize);
    std::vector<Cell> cells() const;
    uint32_t    monteCarlo(uint32_t stepMax);

private:
    void    toDAT(const char* fileName) const;
    void    toDLA(const char* fileName) const;
    void    toTXT(const char* fileName) const;
    void    fromDAT(const char* fileName);
    void    fromDLA(const char* fileName);
    void    fromTXT(const char* fileName);

    std::vector<std::vector<std::vector<double> > > m_field;
};

#endif // MXFIELD_H
