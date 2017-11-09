#ifndef AEROGEN_MULTIDLA_CELLSFIELD_H
#define AEROGEN_MULTIDLA_CELLSFIELD_H

//#include <QMetaType>

#include <vector>

#include "mcoord.h"
#include "magicdla.h"
#include "../basefield.h"

size_t elementsFromSize(const MCoord&);

class CellsField : public Field
{
public:
    CellsField();
    CellsField(const char* fileName, txt_format format);
    CellsField(const MCoord&, const MCoord&, double cellSize);
    ~CellsField();
    
    inline double radius() const { return m_cellSize * 0.5; } // sqrt(1.16) / 2; 0.5385;
    inline double side() const { return m_cellSize / (2.0 * r); }

    std::vector<Cell> cells() const;
    Sizes       sizes() const;

    void        initialize(double porosity, double cellsize);
    uint32_t    monteCarlo(uint32_t stepMax) const;
    
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
    void        toDAT(const char* fileName) const;
    void        toDLA(const char* fileName) const;
    void        toTXT(const char* fileName) const;
    void        fromDAT(const char* fileName);
    void        fromDLA(const char* fileName);
    void        fromTXT(const char* fileName);

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
    CellsField(const CellsField&) = delete;
    CellsField& operator =(const CellsField&) = delete;
};

//Q_DECLARE_METATYPE(CellsField);
        
#endif // AEROGEN_MULTIDLA_CELLSFIELD_H
