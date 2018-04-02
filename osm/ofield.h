#ifndef AEROGEN_OSM_OFIELD_H
#define AEROGEN_OSM_OFIELD_H

#include <vector>

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
    OField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = true);
    
    Sizes       sizes()                                         const override;
    std::vector<Cell> cells()                                   const override;
    void        initialize(double porosity, double cellsize)          override;
    uint32_t    monteCarlo(uint32_t stepMax)                    const override;

    const std::vector<std::vector<OCell> >& clusters() const;
    void        agregate();
    void        setClusters(const std::vector<OCell>& cells);
    void        restoreClusters(const std::vector<std::vector<OCell> >& cells);
    std::vector<Pare> agregateList(const std::vector<OCell>& cells) const;
    double      getVolumeAG(const std::vector<OCell>& cells)   const;
    double      overlapVolume(const std::vector<OCell>& cells) const;
    double      overlapVolumeCells(const OCell& cell1, const OCell& cell2) const;

private:
    void        toDAT(const char* fileName)                     const override;
    void        toDLA(const char* fileName)                     const override;
    void        toTXT(const char* fileName)                     const override;
    void        fromDAT(const char* fileName)                         override;
    void        fromDLA(const char* fileName)                         override;
    void        fromTXT(const char* fileName)                         override;

    void        cleanClusters();
    std::vector<Pare> agregateList(const std::vector<ocell>& cl) const;
    bool        isPointOverlapSpheres(const OCell& cell) const;
    std::vector<OCell> overlapSpheres(const OCell& cell) const;
    std::vector<OCell> overlapGrid(std::vector<std::vector<std::vector<ocell> > >& grid,
                                    const OCell& cell, const Sizes& gsizes) const;
};

#endif // AEROGEN_OSM_OFIELD_H
