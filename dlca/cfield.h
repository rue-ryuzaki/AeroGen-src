#ifndef DLCA_FIELD_H
#define	DLCA_FIELD_H

#include <cmath>
#include <vector>

#include "../basefield.h"
#include "../figure.h"

class CCell : public Cell
{
public:
    CCell(IFigure* figure,
          dCoord coord = dCoord(0.0, 0.0, 0.0),
          Vector3d rotate = Vector3d(0.0, 0.0, 0.0),
          Vector3d vec = Vector3d(0.0, 0.0, 0.0))
        : Cell(figure, coord, rotate),
          m_vec(vec)
    { }
    ~CCell () { }
    
    inline  Vector3d vector() const { return m_vec; }
    void    setVector(const Vector3d& vec) { m_vec = vec; }
    void    move(double t, const Sizes& cs)
    {
        m_coord = m_coord + m_vec * t;
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
    Vector3d m_vec; // speed vector
};

typedef std::vector<CCell> vcell;
//#define DIA 10
//typedef vcell vcellD[DIA][DIA];

class CField : public Field
{
public:
    CField(const char* fileName, txt_format format);
    CField(Sizes m_sizes = Sizes(50, 50, 50));
    
    Sizes   sizes() const;
    std::vector<Cell>  cells() const;
    const std::vector<vcell>& clusters() const;

    void    initialize(double porosity, double cellsize);
    void    initializeTEST(double porosity, double cellsize);
    void    initializeNT(double porosity, double cellsize);
    int     monteCarlo(int stepMax);
    //vector<vcell>[q][q] getCells() const { return vcells; }

    void    agregate();
    void    move();
    double  overlapVolume() const;
    
private:
    void    toDAT(const char* fileName) const;
    void    toDLA(const char* fileName) const;
    void    toTXT(const char* fileName) const;
    void    fromDAT(const char* fileName);
    void    fromDLA(const char* fileName);
    void    fromTXT(const char* fileName);

    static double  fr(double ravr);
    void   clearCells();

    double overlapVolumeSphereSphere(const CCell& cell1, const CCell& cell2) const;
    double overlapVolumeSphereCylinder(const CCell& cell1, const CCell& cell2) const;
    double overlapVolumeCylinderCylinder(const CCell& cell1, const CCell& cell2) const;
    bool   isOverlapSphereSphere(const CCell& cell1, const CCell& cell2) const;
    bool   isOverlapSphereCylinder(const CCell& cell1, const CCell& cell2) const;
    bool   isOverlapCylinderCylinder(const CCell& cell1, const CCell& cell2) const;
    bool   isOverlapCylindersPoint(const dCoord& base1, const dCoord& base2,
        double r1, const dCoord& other, const Vector3d& area, double r2) const;
    bool   isOverlapped(const CCell& cell1, const CCell& cell2) const;
    bool   isPointOverlapSpheres(const CCell& cell) const;

    dCoord diff(const dCoord& c1, const dCoord& c2) const;
    static void   inPareList(std::vector<vui>& agregate, const Pare& pare);
    double leng(const CCell& cell1, const CCell& cell2);
    static double quad(double x);
    
    std::vector<vcell> m_clusters;
    //static const int q = 10;
    //vector<vcell> vcells[q][q];
    double dt = 0.1;
    const double EPS = -1e-4;
};

#endif	// DLCA_FIELD_H
