#ifndef BASECELL_H
#define BASECELL_H

#include "basecoord.h"

class Cell
{
public:
    Cell(IFigure* figure, const dCoord& coord = dCoord(0.0, 0.0, 0.0),
         const Vector3d& rotate = Vector3d(0.0, 0.0, 0.0))
        : m_figure(figure),
          m_coord(coord),
          m_rotate(rotate)
    { }

    virtual ~Cell()
    { }
    
    inline  IFigure* figure() const { return m_figure; }
    const   dCoord&   coord()  const { return m_coord; }
    const   Vector3d& rotate() const { return m_rotate; }
    void    setCoord(const dCoord& coord)  { m_coord = coord; }
    void    setRotate(const Vector3d& rot) { m_rotate = rot; }
    
protected:
    IFigure*    m_figure;
    dCoord      m_coord;
    Vector3d    m_rotate;
};

inline bool isCellsOverlapSphSph(const Cell* cell1, const Cell* cell2)
{
    dCoord diff = cell2->coord() - cell1->coord();
    double r = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    double r_sum = (cell1->figure()->radius() + cell2->figure()->radius())
            * (cell1->figure()->radius() + cell2->figure()->radius());
    return r_sum > r;
}

inline bool isCellsOverlapSphCyl(const Cell* /*cell1*/, const Cell* /*cell2*/)
{
    return false;
}

inline bool isCellsOverlapCylCyl(const Cell* /*cell1*/, const Cell* /*cell2*/)
{
    return false;
}

inline bool isCellsOverlaped(const Cell* cell1, const Cell* cell2)
{
    FigureType t1 = cell1->figure()->type();
    FigureType t2 = cell2->figure()->type();
    if (t1 == fig_sphere && t2 == fig_sphere) {
        return isCellsOverlapSphSph(cell1, cell2);
    }
    if (t1 == fig_sphere && t2 == fig_cylinder) {
        return isCellsOverlapSphCyl(cell1, cell2);
    }
    if (t2 == fig_sphere && t1 == fig_cylinder) {
        return isCellsOverlapSphCyl(cell2, cell1);
    }
    if (t1 == fig_cylinder && t2 == fig_cylinder) {
        return isCellsOverlapCylCyl(cell1, cell2);
    }
    return false;
}

#endif // BASECELL_H
