#ifndef BASECELL_H
#define BASECELL_H

#include "basecoord.h"

class Cell {
public:
    Cell(Figure * figure, dCoord coord = dCoord(0.0, 0.0, 0.0), Vector3d rotate = Vector3d(0.0, 0.0, 0.0)) :
        figure(figure), coord(coord), rotate(rotate) { }
    ~Cell() {
        //delete figure;
        //figure = 0;
    }
    
    Figure * getFigure() const { return figure; }
    inline  dCoord getCoord() const { return coord; }
    inline  Vector3d getRotate() const { return rotate; }
    void    setCoord(dCoord coord)  { this->coord = coord; }
    void    setRotate(Vector3d rot) { this->rotate = rot; }
    
protected:
    Figure *    figure;
    dCoord      coord;
    Vector3d    rotate;
};

#endif /* BASECELL_H */
