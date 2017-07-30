#ifndef BASECELL_H
#define BASECELL_H

#include "basecoord.h"

class Cell
{
public:
    Cell(IFigure* figure,
         dCoord coord = dCoord(0.0, 0.0, 0.0),
         Vector3d rotate = Vector3d(0.0, 0.0, 0.0))
        : figure(figure),
          coord(coord),
          rotate(rotate)
    { }

    ~Cell()
    {
        //delete figure;
        //figure = 0;
    }
    
    IFigure* getFigure() const { return figure; }
    inline  dCoord getCoord() const { return coord; }
    inline  Vector3d getRotate() const { return rotate; }
    void    setCoord(dCoord coord)  { this->coord = coord; }
    void    setRotate(Vector3d rot) { this->rotate = rot; }
    
protected:
    IFigure*    figure;
    dCoord      coord;
    Vector3d    rotate;
};

#endif // BASECELL_H
