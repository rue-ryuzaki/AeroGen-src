#ifndef OSM_OFIELD_H
#define	OSM_OFIELD_H

#include <QGLWidget>
#include <vector>
#include <cmath>

#include "../basefield.h"
#include "otypes.h"

class OCell : public Cell
{
public:
    OCell(Figure * figure, dCoord coord = dCoord(0.0, 0.0, 0.0))
        : Cell(figure, coord)
    { }

    virtual ~OCell() { }
    
    bool mark = false;
};

typedef std::vector<OCell> ocell;

class OField : public Field
{
public:
    OField(const char * fileName, txt_format format);
    OField(Sizes sizes = Sizes(50, 50, 50));
    virtual ~OField();
    
    Sizes   getSizes() const;
    std::vector<Cell> getCells() const;
    std::vector<ocell> getClusters() const;

    void    Initialize(double porosity, double cellsize);
    int     MonteCarlo(int stepMax);
    
    void    Agregate();
    void    setClusters(std::vector<OCell> & cells);
    void    restoreClusters(std::vector<ocell> & cells);
    std::vector<Pare> AgregateList(std::vector<OCell> & cells);
    double  getVolumeAG(const std::vector<OCell> & varcells);
    void    inPareList(std::vector<vui> & agregate, Pare & pare);
    double  overlapVolume(const std::vector<OCell> & cells);
    double  overlapVolumeCells(const OCell & cell1, const OCell & cell2);

private:
    void toDAT(const char * fileName) const;
    void toDLA(const char * fileName) const;
    void toTXT(const char * fileName) const;
    void fromDAT(const char * fileName);
    void fromDLA(const char * fileName);
    void fromTXT(const char * fileName);

    void AddCell(const OCell & cell);
    void CleanEmptyClusters(std::vector<ocell> & cl);
    void CleanClusters();
    void Agregate(std::vector<ocell> & cl);
    void ReBuildGrid();
    void clearCells();

    std::vector<Pare> AgregateList(std::vector<ocell> & cl);
    dCoord  Diff(dCoord c1, dCoord c2);
    bool    is_overlapped(const OCell & cell1, const OCell & cell2);
    bool    is_point_overlap_spheres(const OCell & cell);
    std::vector<OCell> overlap_spheres(const OCell & cell);
    std::vector<OCell> overlap_grid(const OCell& cell);
    double leng(const OCell & cell1, const OCell & cell2);
    double sqleng(const OCell & cell1, const OCell & cell2);
    //std::vector<OCell> cells;
    ocell *** grid;
    Sizes gsizes;
    std::vector<ocell> clusters;
    const double EPS = -1e-4;
};

#endif	/* OSM_OFIELD_H */
