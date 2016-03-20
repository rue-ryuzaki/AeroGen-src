#ifndef MULTIDLA_CELLSFIELD_H
#define	MULTIDLA_CELLSFIELD_H
//#include <QMetaType>

#include "mcoord.h"
#include "magicdla.h"
#include "../basefield.h"

size_t GetElementsFromSize(const MCoord &);

class CellsField : public Field {
public:
    CellsField(const char * fileName, txt_format format);
    CellsField() : mNullPnt(MCoord()), mSize(MCoord()), cellSize(2.0) { }
    CellsField(const MCoord &, const MCoord &, double cellSize);
    ~CellsField() { delete [] mCells; }
    
    inline double getRadius() const { return cellSize * 0.5; } // sqrt(1.16) / 2; 0.5385;
    inline double getSide() const { return cellSize / (2 * r); }
    vector<Cell> getCells() const;
    Sizes getSizes() const {
        return Sizes((int)(mSize.GetCoord(0) * getSide()),
                (int)(mSize.GetCoord(1) * getSide()), (int)(mSize.GetCoord(2) * getSide()));
    }
    void Initialize(double porosity, double cellsize) { }
    
    FieldElement GetElement(const MCoord &) const;
    bool IsSet(const MCoord &) const;
    void SetElement(const MCoord & c) { SetElementVal(c, OCUPIED_CELL); }
    void UnSetElement(const MCoord & c) { SetElementVal(c, FREE_CELL); }

    void SetElementVal(const MCoord &, const FieldElement);
    FieldElement GetElementVal(const MCoord &);

    void Clear();

    Coordinate GetTotalElements() const;
    Coordinate GetCellsCnt() const;

    MCoord GetSize() const { return mSize; }
    MCoord GetNullPnt() const { return mNullPnt; }
    size_t GetDims() const { return mDims; }

    void Fill(FieldElement);

    void Resize(MCoord & newSize, MCoord & leftUpperCorner);

    bool IsElementInField(const MCoord &) const;
    int MonteCarlo(int stepMax);
    
private:
    void toDAT(const char * fileName) const;
    void toDLA(const char * fileName) const;
    void toTXT(const char * fileName) const;
    void fromDAT(const char * fileName);
    void fromDLA(const char * fileName);
    void fromTXT(const char * fileName);
    bool is_overlapped(const MCoord & m1, double r1, double ixc, double iyc, double izc, double r2);
    const double EPS = -1e-4;
    MCoord mNullPnt;
    MCoord mSize;
    FieldElement * mCells;
    size_t mDims;
    const double r = 0.5385;
    double cellSize;
    
protected:
    Coordinate CoordToAbs(const MCoord &) const;
    void CreateCache(const MCoord &);
};

//Q_DECLARE_METATYPE(CellsField);
        
#endif	/* MULTIDLA_CELLSFIELD_H */
