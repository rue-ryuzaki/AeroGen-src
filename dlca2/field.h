#ifndef AEROGEN_DLCA2_FIELD_H
#define AEROGEN_DLCA2_FIELD_H

#include <vector>

#include "../basefield.h"
#include "../flexible_field.h"
#include "../figure.h"

namespace dlca2 {
class XCell : public Cell
{
public:
    XCell(IFigure* figure,
          const dCoord& coord = dCoord(0.0, 0.0, 0.0),
          const Vector3d& rotate = Vector3d(0.0, 0.0, 0.0),
          const Vector3d& speed = Vector3d(0.0, 0.0, 0.0))
        : Cell(figure, coord, rotate),
          m_speed(speed)
    { }
    ~XCell () { }
    
    const Vector3d& speed() const { return m_speed; }
    void    setSpeed(const Vector3d& vec) { m_speed = vec; }
    void    move(double t, const Sizes& cs)
    {
        m_coord = m_coord + m_speed * t;
        if (m_coord.x < 0) {
            m_coord.x = m_coord.x + cs.x;
        } else if (m_coord.x >= cs.x) {
            m_coord.x = m_coord.x - cs.x;
        }
        if (m_coord.y < 0) {
            m_coord.y = m_coord.y + cs.y;
        } else if (m_coord.y >= cs.y) {
            m_coord.y = m_coord.y - cs.y;
        }
        if (m_coord.z < 0) {
            m_coord.z = m_coord.z + cs.z;
        } else if (m_coord.z >= cs.z) {
            m_coord.z = m_coord.z - cs.z;
        }
    }
    
private:
    Vector3d m_speed; // speed vector
};

class XField : public Field, public FlexibleField<XCell>
{
public:
    XField(const char* fileName, txt_format format);
    XField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = true);
    
    Sizes       sizes()                                          const override;
    std::vector<Cell>  cells()                                   const override;
    const std::vector<std::vector<XCell> >& clusters() const;

    void        initialize(double porosity, double cellsize)           override;
    void        initializeTEST(double porosity, double cellsize);
    void        initializeNT(double porosity, double cellsize);
    uint32_t    monteCarlo(uint32_t stepMax)                     const override;

    void        agregate();
    void        move();
    double      overlapVolume() const;
    
private:
    void        toDAT(const char* fileName)                      const override;
    void        toDLA(const char* fileName)                      const override;
    void        toTXT(const char* fileName)                      const override;
    void        fromDAT(const char* fileName)                          override;
    void        fromDLA(const char* fileName)                          override;
    void        fromTXT(const char* fileName)                          override;

    static double  fr(double ravr);

    bool   isCellOverlapSpheres(const XCell& cell) const;

    //static const uint32_t q = 10;
    //vector<vcell> vcells[q][q];
    double dt = 0.1;
};
} // namespace dlca2

#endif // AEROGEN_DLCA2_FIELD_H
