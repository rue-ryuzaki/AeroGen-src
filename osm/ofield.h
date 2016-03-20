#ifndef OSM_OFIELD_H
#define	OSM_OFIELD_H

#include <QGLWidget>
#include <vector>
#include <cmath>
#include "../basefield.h"
#include "otypes.h"

using namespace std;

class OCell : public Cell {
public:
    OCell(Figure * figure, Coord<double> coord = Coord<double>(0.0, 0.0, 0.0))
            : Cell(figure, coord) { }
    virtual ~OCell () { }
    
    bool mark = false;
};

typedef vector<OCell> ocell;

class OField : public Field {
public:
    OField(const char * fileName, txt_format format);
    OField(Sizes sizes = Sizes(50, 50, 50)) : Field(sizes) { }
    virtual ~OField() { }
    
    Sizes getSizes() const { return sizes; }
    void Initialize(double porosity, double cellsize);
    vector<Cell> getCells() const;
    vector<ocell> getClusters() const { return clusters; }
    void Agregate();
    void setClusters(vector<OCell> & cells);
    void restoreClusters(vector<ocell> & cells);
    vector<Pare> AgregateList(vector<OCell> & cells);
    double getVolumeAG(const vector<OCell> & varcells);
    void inPareList(vector<vui> & agregate, Pare & pare);
    double overlapVolumeCells(const OCell & cell1, const OCell & cell2);
    double overlapVolume(const vector<OCell> & cells);
    int MonteCarlo(int stepMax);
    
private:
    void toDAT(const char * fileName) const;
    void toDLA(const char * fileName) const;
    void toTXT(const char * fileName) const;
    void fromDAT(const char * fileName);
    void fromDLA(const char * fileName);
    void fromTXT(const char * fileName);
    vector<Pare> AgregateList(vector<ocell> & cl);
    void AddCell(const OCell & cell);
    void CleanEmptyClusters(vector<ocell> & cl);
    void CleanClusters();
    void Agregate(vector<ocell> & cl);
    void ReBuildGrid();
    void Puck();
    void clearCells();
    Coord<double> Diff(Coord<double> c1, Coord<double> c2);
    bool is_overlapped(const OCell & cell1, const OCell & cell2);
    bool is_point_overlap_spheres(const OCell & cell);
    vector<OCell> overlap_spheres(const OCell & cell);
    vector<OCell> overlap_grid(const OCell& cell);
    double leng(const OCell & cell1, const OCell & cell2);
    double sqleng(const OCell & cell1, const OCell & cell2);
    //vector<OCell> cells;
    ocell *** grid;
    Sizes gsizes;
    vector<ocell> clusters;
    const double EPS = -1e-4;
};

#endif	/* OSM_OFIELD_H */
