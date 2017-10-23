#ifndef BASECELL_H
#define BASECELL_H

#include "basecoord.h"

class Cell
{
public:
    Cell(IFigure* figure, dCoord coord = dCoord(0.0, 0.0, 0.0),
         Vector3d rotate = Vector3d(0.0, 0.0, 0.0))
        : m_figure(figure),
          m_coord(coord),
          m_rotate(rotate)
    { }

    virtual ~Cell()
    { }
    
    inline  IFigure* figure() const { return m_figure; }
    inline  dCoord   coord()  const { return m_coord; }
    inline  Vector3d rotate() const { return m_rotate; }
    void    setCoord(dCoord coord)  { m_coord = coord; }
    void    setRotate(Vector3d rot) { m_rotate = rot; }
    
protected:
    IFigure*    m_figure;
    dCoord      m_coord;
    Vector3d    m_rotate;
};

#endif // BASECELL_H
