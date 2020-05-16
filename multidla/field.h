#ifndef AEROGEN_MULTIDLA_FIELD_H
#define AEROGEN_MULTIDLA_FIELD_H

#include <vector>

#include "coord.h"
#include "magicdla.h"
#include "../basefield.h"

namespace multidla {
size_t elementsFromSize(const MCoord&);

class XField : public Field
{
public:
    XField();
    XField(const char* fileName, txt_format format);
    XField(const MCoord&, const MCoord&, double cellSize);
    ~XField();
    
    inline double radius() const { return m_cellSize * 0.5; } // std::sqrt(1.16) / 2; 0.5385;
    inline double side() const { return m_cellSize / (2.0 * r); }

    std::vector<Cell> cells()                                    const override;
    Sizes       sizes()                                          const override;

    void        initialize(double porosity, double cellsize)           override;
    uint32_t    monteCarlo(uint32_t stepMax)                     const override;
    
    FieldElement element(const MCoord&) const;
    bool        isSet(const MCoord&) const;
    void        setElement(const MCoord& c);
    void        unSetElement(const MCoord& c);

    void        setElementVal(const MCoord&, const FieldElement);
    FieldElement elementVal(const MCoord&);

    void        clear();

    Coordinate  totalElements() const;
    Coordinate  cellsCnt() const;

    MCoord      size() const;
    MCoord      nullPnt() const;
    size_t      dims() const;

    void        fill(FieldElement);

    void        resize(const MCoord& newSize, const MCoord& leftUpperCorner);

    bool        isElementInField(const MCoord&) const;
    
private:
    void        toDAT(const char* fileName)                      const override;
    void        toDLA(const char* fileName)                      const override;
    void        toTXT(const char* fileName)                      const override;
    void        fromDAT(const char* fileName)                          override;
    void        fromDLA(const char* fileName)                          override;
    void        fromTXT(const char* fileName)                          override;

    bool        isOverlapped(const MCoord& m1, double r1, double ixc,
                             double iyc, double izc, double r2) const;

    MCoord      m_nullPnt;
    MCoord      m_size;
    FieldElement* m_cells;
    size_t      m_dims;
    const double r = 0.5385;
    double      m_cellSize;
    
protected:
    Coordinate coordToAbs(const MCoord&) const;
    //void CreateCache(const MCoord&);

private:
    XField(const XField&) = delete;
    XField& operator =(const XField&) = delete;
};
} // multidla
        
#endif // AEROGEN_MULTIDLA_CELLSFIELD_H
