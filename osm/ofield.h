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
        : Cell(figure, coord),
          mark(false)
    { }

    virtual ~OCell() { }
    
    bool    mark;
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
    void    setClusters(const std::vector<OCell> & cells);
    void    restoreClusters(const std::vector<ocell> & cells);
    std::vector<Pare> AgregateList(const std::vector<OCell> & cells) const;
    double  getVolumeAG(const std::vector<OCell> & varcells);
    static void    inPareList(std::vector<vui> & agregate, const Pare & pare);
    double  overlapVolume(const std::vector<OCell> & cells) const;
    double  overlapVolumeCells(const OCell & cell1, const OCell & cell2) const;

private:
    void toDAT(const char * fileName) const;
    void toDLA(const char * fileName) const;
    void toTXT(const char * fileName) const;
    void fromDAT(const char * fileName);
    void fromDLA(const char * fileName);
    void fromTXT(const char * fileName);

    void AddCell(const OCell & cell);
    static void CleanEmptyClusters(std::vector<ocell> & cl);
    void CleanClusters();
    void Agregate(std::vector<ocell> & cl);
    void ReBuildGrid();
    void clearCells();

    std::vector<Pare> AgregateList(const std::vector<ocell> & cl) const;
    dCoord  Diff(const dCoord& c1, const dCoord& c2) const;
    bool    is_overlapped(const OCell & cell1, const OCell & cell2) const;
    bool    is_point_overlap_spheres(const OCell & cell) const;
    std::vector<OCell> overlap_spheres(const OCell & cell) const;
    std::vector<OCell> overlap_grid(const OCell& cell) const;
    double leng(const OCell & cell1, const OCell & cell2) const;
    double sqleng(const OCell & cell1, const OCell & cell2) const;
    //std::vector<OCell> cells;
    ocell *** grid;
    Sizes gsizes;
    std::vector<ocell> clusters;
    const double EPS = -1e-4;
};

#endif	/* OSM_OFIELD_H */
