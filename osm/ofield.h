#ifndef OSM_OFIELD_H
#define	OSM_OFIELD_H

#include <vector>
#include <QGLWidget>

#include "../basefield.h"
#include "../flexible_field.h"
#include "otypes.h"

class OCell : public Cell
{
public:
    OCell(IFigure* figure, const dCoord& coord = dCoord(0.0, 0.0, 0.0))
        : Cell(figure, coord),
          mark(false)
    { }

    virtual ~OCell() { }
    
    bool    mark;
};

typedef std::vector<OCell> ocell;

class OField : public Field, public FlexibleField<OCell>
{
public:
    OField(const char* fileName, txt_format format);
    OField(const Sizes& sizes = Sizes(50, 50, 50));
    
    Sizes       sizes() const;
    std::vector<Cell> cells() const;
    const std::vector<std::vector<OCell> >& clusters() const;

    void        initialize(double porosity, double cellsize);
    uint32_t    monteCarlo(uint32_t stepMax) const;
    
    void        agregate();
    void        setClusters(const std::vector<OCell>& cells);
    void        restoreClusters(const std::vector<std::vector<OCell> >& cells);
    std::vector<Pare> agregateList(const std::vector<OCell>& cells) const;
    double      getVolumeAG(const std::vector<OCell>& cells) const;
    double      overlapVolume(const std::vector<OCell>& cells) const;
    double      overlapVolumeCells(const OCell& cell1, const OCell& cell2) const;

private:
    void        toDAT(const char* fileName) const;
    void        toDLA(const char* fileName) const;
    void        toTXT(const char* fileName) const;
    void        fromDAT(const char* fileName);
    void        fromDLA(const char* fileName);
    void        fromTXT(const char* fileName);

    void        cleanClusters();
    std::vector<Pare> agregateList(const std::vector<ocell>& cl) const;
    bool        isPointOverlapSpheres(const OCell& cell) const;
    std::vector<OCell> overlapSpheres(const OCell& cell) const;
    std::vector<OCell> overlapGrid(std::vector<std::vector<std::vector<ocell> > >& grid,
                                    const OCell& cell, const Sizes& gsizes) const;
};

#endif	// OSM_OFIELD_H
