#ifndef MULTIDLA_CELLSFIELD_H
#define	MULTIDLA_CELLSFIELD_H
//#include <QMetaType>

#include <vector>

#include "mcoord.h"
#include "magicdla.h"
#include "../basefield.h"

size_t GetElementsFromSize(const MCoord&);

class CellsField : public Field
{
public:
    CellsField();
    CellsField(const char* fileName, txt_format format);
    CellsField(const MCoord&, const MCoord&, double cellSize);
    ~CellsField();
    
    inline double getRadius() const { return cellSize * 0.5; } // sqrt(1.16) / 2; 0.5385;
    inline double getSide() const { return cellSize / (2 * r); }

    std::vector<Cell> getCells() const;
    Sizes getSizes() const;

    void    Initialize(double porosity, double cellsize);
    int     MonteCarlo(int stepMax);
    
    FieldElement GetElement(const MCoord&) const;
    bool    IsSet(const MCoord&) const;
    void    SetElement(const MCoord& c);
    void    UnSetElement(const MCoord& c);

    void    SetElementVal(const MCoord&, const FieldElement);
    FieldElement GetElementVal(const MCoord&);

    void    Clear();

    Coordinate GetTotalElements() const;
    Coordinate GetCellsCnt() const;

    MCoord  GetSize() const;
    MCoord  GetNullPnt() const;
    size_t  GetDims() const;

    void    Fill(FieldElement);

    void    Resize(MCoord& newSize, MCoord& leftUpperCorner);

    bool    IsElementInField(const MCoord&) const;
    
private:
    void toDAT(const char* fileName) const;
    void toDLA(const char* fileName) const;
    void toTXT(const char* fileName) const;
    void fromDAT(const char* fileName);
    void fromDLA(const char* fileName);
    void fromTXT(const char* fileName);

    bool is_overlapped(const MCoord& m1, double r1, double ixc, double iyc, double izc, double r2);

    const double EPS = -1e-4;
    MCoord mNullPnt;
    MCoord mSize;
    FieldElement* mCells;
    size_t mDims;
    const double r = 0.5385;
    double cellSize;
    
protected:
    Coordinate CoordToAbs(const MCoord&) const;
    //void CreateCache(const MCoord&);
};

//Q_DECLARE_METATYPE(CellsField);
        
#endif	// MULTIDLA_CELLSFIELD_H
