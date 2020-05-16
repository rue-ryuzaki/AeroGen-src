#ifndef AEROGEN_OSM_FIELD_H
#define AEROGEN_OSM_FIELD_H

#include <vector>

#include "../basefield.h"
#include "../flexible_field.h"
#include "types.h"

namespace osm {
class XCell : public Cell
{
public:
    XCell(IFigure* figure, const dCoord& coord = dCoord(0.0, 0.0, 0.0))
        : Cell(figure, coord),
          mark(false)
    { }

    virtual ~XCell() { }
    
    bool    mark;
};

class XField : public Field, public FlexibleField<XCell>
{
public:
    XField(const char* fileName, txt_format format);
    XField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = true);
    
    Sizes       sizes()                                          const override;
    std::vector<Cell> cells()                                    const override;
    void        initialize(double porosity, double cellsize)           override;
    uint32_t    monteCarlo(uint32_t stepMax)                     const override;

    const std::vector<std::vector<XCell> >& clusters() const;
    void        agregate();
    void        setClusters(const std::vector<XCell>& cells);
    void        restoreClusters(const std::vector<std::vector<XCell> >& cells);
    std::vector<Pare> agregateList(const std::vector<XCell>& cells) const;
    double      getVolumeAG(const std::vector<XCell>& cells)   const;
    double      overlapVolume(const std::vector<XCell>& cells) const;
    double      overlapVolumeCells(const XCell& cell1, const XCell& cell2) const;

private:
    void        toDAT(const char* fileName)                      const override;
    void        toDLA(const char* fileName)                      const override;
    void        toTXT(const char* fileName)                      const override;
    void        fromDAT(const char* fileName)                          override;
    void        fromDLA(const char* fileName)                          override;
    void        fromTXT(const char* fileName)                          override;

    void        cleanClusters();
    std::vector<Pare> agregateList(const std::vector<std::vector<XCell> >& cl) const;
    bool        isPointOverlapSpheres(const XCell& cell) const;
    std::vector<XCell> overlapSpheres(const XCell& cell) const;
    std::vector<XCell> overlapGrid(std::vector<std::vector<std::vector<std::vector<XCell> > > >& grid,
                                   const XCell& cell, const Sizes& gsizes) const;
};
} // osm

#endif // AEROGEN_OSM_FIELD_H
