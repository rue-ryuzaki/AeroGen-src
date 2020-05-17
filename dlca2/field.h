#ifndef AEROGEN_DLCA2_FIELD_H
#define AEROGEN_DLCA2_FIELD_H

#include <vector>

#include "../basefield.h"
#include "../figure.h"

namespace dlca2 {
class Cluster
{
public:
    typedef Cell value_type;

    Cluster(const std::vector<Cell>& cells = std::vector<Cell>(),
            const Vector3d& speed = Vector3d())
        : m_cells(cells),
          m_speed(speed)
    {
    }

    inline const Vector3d& speed()                 const { return m_speed; }
    inline void            setSpeed(const Vector3d& vec) { m_speed = vec; }
    void    move(double t, const Sizes& cs)
    {
        for (auto& cell : m_cells) {
            dCoord coord = cell.coord() + m_speed * t;
            if (coord.x < 0) {
                coord.x = coord.x + cs.x;
            } else if (coord.x >= cs.x) {
                coord.x = coord.x - cs.x;
            }
            if (coord.y < 0) {
                coord.y = coord.y + cs.y;
            } else if (coord.y >= cs.y) {
                coord.y = coord.y - cs.y;
            }
            if (coord.z < 0) {
                coord.z = coord.z + cs.z;
            } else if (coord.z >= cs.z) {
                coord.z = coord.z - cs.z;
            }
            cell.setCoord(coord);
        }
    }

    inline void     clear()                   { m_cells.clear(); }
    inline bool     empty()             const { return m_cells.empty(); }
    inline size_t   size()              const { return m_cells.size(); }
    inline void     reserve(size_t s)         { m_cells.reserve(s); }
    inline void     push_back(const Cell& c)  { m_cells.push_back(c); }

    inline       Cell& operator [](size_t i)       { return m_cells[i]; }
    inline const Cell& operator [](size_t i) const { return m_cells[i]; }
    inline       Cell&          at(size_t i)       { return m_cells.at(i); }
    inline const Cell&          at(size_t i) const { return m_cells.at(i); }

    inline std::vector<Cell>::iterator       begin()        { return m_cells.begin(); }
    inline std::vector<Cell>::iterator       end()          { return m_cells.end(); }
    inline std::vector<Cell>::const_iterator begin()  const { return m_cells.begin(); }
    inline std::vector<Cell>::const_iterator end()    const { return m_cells.end(); }
    inline std::vector<Cell>::const_iterator cbegin() const { return m_cells.cbegin(); }
    inline std::vector<Cell>::const_iterator cend()   const { return m_cells.cend(); }

private:
    std::vector<Cell>   m_cells;
    Vector3d            m_speed;
};

class XField : public Field
{
public:
    XField(const char* fileName, txt_format format);
    XField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = true);
    
    Sizes       sizes()                                          const override;
    std::vector<Cell>  cells()                                   const override;
    const std::vector<Cluster>& clusters() const;

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

    bool   isCellOverlapSpheres(const Cell& cell) const;

    //static const uint32_t q = 10;
    //vector<vcell> vcells[q][q];
    std::vector<Cluster> m_clusters;
    double dt = 0.1;
};
} // namespace dlca2

#endif // AEROGEN_DLCA2_FIELD_H
