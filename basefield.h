#ifndef AEROGEN_BASEFIELD_H
#define AEROGEN_BASEFIELD_H

#include <cstdint>
#include <vector>

#include "basecell.h"

#define NitroDiameter 0.34

class Field
{
public:
    Field(const char* /*fileName*/, txt_format /*format*/) : m_sizes() { }

    explicit Field(const Sizes& sizes = Sizes(50, 50, 50)) : m_sizes(sizes) { }
    virtual ~Field() { }
    
    virtual Sizes             sizes() const = 0;
    virtual std::vector<Cell> cells() const = 0;

    virtual void initialize(double porosity, double cellsize) = 0;
    virtual uint32_t monteCarlo(uint32_t stepMax) const = 0;

    void toFile(const char* fileName, txt_format format) const;

protected:
    virtual void toDAT(const char* fileName) const = 0;
    virtual void toDLA(const char* fileName) const = 0;
    virtual void toTXT(const char* fileName) const = 0;
    virtual void fromDAT(const char* fileName) = 0;
    virtual void fromDLA(const char* fileName) = 0;
    virtual void fromTXT(const char* fileName) = 0;

    double overlapVolumeSphSph(const Cell* cell1, const Cell* cell2) const;
    double overlapVolumeSphCyl(const Cell* cell1, const Cell* cell2) const;
    double overlapVolumeCylCyl(const Cell* cell1, const Cell* cell2) const;
    bool   isOverlapSphSph(const Cell* cell1, const Cell* cell2) const;
    bool   isOverlapSphCyl(const Cell* cell1, const Cell* cell2) const;
    bool   isOverlapCylCyl(const Cell* cell1, const Cell* cell2) const;
    bool   isOverlapCylPoint(const dCoord& base1, const dCoord& base2,
        double r1, const dCoord& other, const Vector3d& area, double r2) const;
    bool   isOverlapped(const Cell* cell1, const Cell* cell2) const;

    dCoord diff(const dCoord& c1, const dCoord& c2) const;
    double leng(const Cell* cell1, const Cell* cell2) const;

    const double EPS = -1e-4;
    Sizes   m_sizes;
};

#endif // AEROGEN_BASEFIELD_H
