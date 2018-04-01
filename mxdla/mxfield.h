#ifndef AEROGEN_MXDLA_MXFIELD_H
#define AEROGEN_MXDLA_MXFIELD_H

#include <vector>

#include "../basefield.h"
#include "../flexible_field.h"

class MxField : public Field
{
public:
    MxField(const char* fileName, txt_format format);
    MxField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = true);
    virtual ~MxField() { }

    inline double radius() const { return m_cellSize * 0.5; }
    inline double side() const { return m_cellSize; }

    Sizes       sizes() const;
    void        initialize(double porosity, double cellsize);
    std::vector<Cell> cells() const;
    uint32_t    monteCarlo(uint32_t stepMax) const;

    void    initDla(double por, uint32_t initial, uint32_t step, uint32_t hit);

private:
    void    toDAT(const char* fileName) const override;
    void    toDLA(const char* fileName) const override;
    void    toTXT(const char* fileName) const override;
    void    fromDAT(const char* fileName) override;
    void    fromDLA(const char* fileName) override;
    void    fromTXT(const char* fileName) override;

    bool    isInside(uint32_t r[3], uint32_t coord[3]) const;

    std::vector<std::vector<std::vector<uint8_t> > > m_field;
    Sizes   m_sides;
    double  m_cellSize;
};

#endif // AEROGEN_MXDLA_MXFIELD_H
