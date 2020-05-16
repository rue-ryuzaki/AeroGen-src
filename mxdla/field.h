#ifndef AEROGEN_MXDLA_FIELD_H
#define AEROGEN_MXDLA_FIELD_H

#include <vector>

#include "../basefield.h"
#include "../flexible_field.h"

namespace mxdla {
class XField : public Field
{
public:
    XField(const char* fileName, txt_format format);
    XField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = true);
    virtual ~XField() { }

    inline double radius() const { return m_cellSize * 0.5; }
    inline double side() const { return m_cellSize; }

    Sizes       sizes()                                          const override;
    std::vector<Cell> cells()                                    const override;
    void        initialize(double porosity, double cellsize)           override;
    uint32_t    monteCarlo(uint32_t stepMax)                     const override;

    void        initDla(double por, uint32_t initial, uint32_t step, uint32_t hit);

private:
    void        toDAT(const char* fileName)                      const override;
    void        toDLA(const char* fileName)                      const override;
    void        toTXT(const char* fileName)                      const override;
    void        fromDAT(const char* fileName)                          override;
    void        fromDLA(const char* fileName)                          override;
    void        fromTXT(const char* fileName)                          override;

    bool        isInside(uint32_t r[3], uint32_t coord[3]) const;

    std::vector<std::vector<std::vector<uint8_t> > > m_field;
    Sizes       m_sides;
    double      m_cellSize;
};
} // mxdla

#endif // AEROGEN_MXDLA_FIELD_H
