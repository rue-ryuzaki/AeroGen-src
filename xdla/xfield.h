#ifndef XFIELD_H
#define XFIELD_H

#include <vector>

#include "../basefield.h"

class xField : public Field
{
public:
    xField(const char* fileName, txt_format format);
    xField(Sizes sizes = Sizes(50, 50, 50));
    virtual ~xField() { }

    Sizes   getSizes() const;
    void    Initialize(double porosity, double cellsize);
    std::vector<Cell> getCells() const;
    int     MonteCarlo(int stepMax);

private:
    void toDAT(const char* fileName) const;
    void toDLA(const char* fileName) const;
    void toTXT(const char* fileName) const;
    void fromDAT(const char* fileName);
    void fromDLA(const char* fileName);
    void fromTXT(const char* fileName);
};

#endif // XFIELD_H
