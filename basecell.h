#ifndef AEROGEN_BASECELL_H
#define AEROGEN_BASECELL_H

#include "basecoord.h"

template <class T>
inline T sqr(T t)
{
    return t * t;
}
template <class T>
inline T cube(T t)
{
    return t * t * t;
}
template <class T>
inline T quad(T t)
{
    return t * t * t * t;
}
template <class T>
inline double VfromR(T t)
{
    return (4.0 / 3.0) * M_PI * double(cube(t));
}
inline double RfromV(double v) { return pow((3.0 * v) / (4.0 * M_PI), 1.0 / 3.0); }
inline double SfromR(double r) { return 4.0 * M_PI * r * r; }
inline double RfromS(double s) { return std::sqrt(0.25 * s / M_PI); }
inline double VfromD(double d) { return M_PI * cube(d) / 6.0; }
inline double Dmin(double d, double psi) { return d * std::sqrt(1.0 - psi * psi); }
// 2 dims
inline double SfromR2D(double r) { return M_PI * r * r; }
inline double RfromS2D(double s) { return std::sqrt(s / M_PI); }

class Cell
{
public:
    Cell(IFigure* figure, const dCoord& coord = dCoord(0.0, 0.0, 0.0),
         const Vector3d& rotate = Vector3d(0.0, 0.0, 0.0))
        : m_figure(figure),
          m_coord(coord),
          m_rotate(rotate)
    { }

    virtual ~Cell() { }
    
    inline  IFigure*  figure() const { return m_figure; }
    const   dCoord&   coord()  const { return m_coord; }
    const   Vector3d& rotate() const { return m_rotate; }
    void    setCoord(const dCoord& coord) { m_coord = coord; }
    void    setRotate(const Vector3d& vec) { m_rotate = vec; }
    
protected:
    IFigure*    m_figure;
    dCoord      m_coord;
    Vector3d    m_rotate;
};

inline double cellsOverlapVolumeSphSph(const Cell* cell1, const Cell* cell2)
{
    dCoord diff = cell2->coord() - cell1->coord();
    double r = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    double r1 = cell1->figure()->radius();
    double r2 = cell2->figure()->radius();
    if (sqr(r1 + r2) > r) {
        double d = std::sqrt(r);
        return 2.0 * M_PI * ((cube(r2) - cube(d - r1)) / 3.0 - ((quad(r2) - quad(d - r1)) / 4.0
                + ((sqr(d) - sqr(r1)) * (sqr(r2) - sqr(d - r1))) / 2.0) / (2.0 * d));
    }
    return 0.0;
}

inline double cellsOverlapVolumeSphCyl(const Cell* /*cell1*/, const Cell* /*cell2*/)
{
    return 0.0;
}

inline double cellsOverlapVolumeCylCyl(const Cell* /*cell1*/, const Cell* /*cell2*/)
{
    return 0.0;
}

inline bool isCellsOverlapSphSph(const Cell* cell1, const Cell* cell2)
{
    dCoord diff = cell2->coord() - cell1->coord();
    double r = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    double r_sum = (cell1->figure()->radius() + cell2->figure()->radius())
            * (cell1->figure()->radius() + cell2->figure()->radius());
    return r_sum > r;
}

inline bool isCellsOverlapSphCyl(const Cell* cell1, const Cell* cell2)
{
    const dCoord& c1 = cell1->coord();
    const dCoord& c2 = cell2->coord();
    dCoord diff = c2 - c1;
    dCoord c2d = c1 + diff;
    double r = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    double r1 = cell1->figure()->radius();
    double r2 = cell2->figure()->radius();
    double h2 = static_cast<FCylinder*>(cell2->figure())->height() * 0.5;
    double r_sum = sqr(r1 + std::sqrt(sqr(r2) + sqr(h2)));
    // 1
    if (r > r_sum) {
        return false;
    }
    // 2 Transform
    const Vector3d& transf = cell2->rotate();
    dCoord c2A(0.0, 0.0, h2);
    Vector3d N = Vector3d::Oy();
    c2A.rotate(transf.x, Vector3d::Ox());
    N.rotate  (transf.x, Vector3d::Ox());
    c2A.rotate(transf.y, N);
    dCoord c2B = c2d - c2A;
    c2A = c2d + c2A;
    Vector3d A1 = c1  - c2A;
    Vector3d B1 = c1  - c2B;
    Vector3d AB = c2B - c2A;
    double base = 2.0 * h2;//std::sqrt(AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);
    double baseA1 = A1.length();
    double baseB1 = B1.length();
    //double cosA = dCoord::cosA(c1, c2A, c2B);
    //double cosB = dCoord::cosA(c1, c2B, c2A);
    double cosA =  (A1.x * AB.x + A1.y * AB.y + A1.z * AB.z) / (baseA1 * base);
    double cosB = -(B1.x * AB.x + B1.y * AB.y + B1.z * AB.z) / (baseB1 * base);
    if (cosA >= 0 && cosB >= 0) {
        //double S = 0.5 * base * std::sqrt(1 - cosA * cosA) * baseA1;
        //double SS= 0.5 * base * (r1 + r2);
        return ((r1 + r2) > std::sqrt(1.0 - cosA * cosA) * baseA1);
    }
    if (cosA < 0) {
        double H = abs(cosA) * baseA1;
        if (r1 < H) {
            return false;
        }
        if ((H * H + r2 * r2) > (baseA1 * baseA1)) {
            // тупой угол - высота снаружи
            return r1 > H;
        } else {
            // острый - высота внутри
            double A = std::sqrt(r2 * r2 + baseA1 * baseA1 - 2.0 * r2 * baseA1 * cosA);
            return r1 > A;
        }
    } else {
        double H = abs(cosB) * baseB1;
        if (r1 < H) {
            return false;
        }
        if ((H * H + r2 * r2) > (baseB1 * baseB1)) {
            // тупой угол - высота снаружи
            return r1 > H;
        } else {
            // острый - высота внутри
            double B = std::sqrt(r2 * r2 + baseB1 * baseB1 - 2.0 * r2 * baseB1 * cosB);
            return r1 > B;
        }
    }
}

inline bool isCellsOverlapCylCyl(const Cell* /*cell1*/, const Cell* /*cell2*/)
{
    return false;
}

inline bool isCellsOverlaped(const Cell* cell1, const Cell* cell2)
{
    FigureType t1 = cell1->figure()->type();
    FigureType t2 = cell2->figure()->type();
    if (t1 == fig_sphere && t2 == fig_sphere) {
        return isCellsOverlapSphSph(cell1, cell2);
    }
    if (t1 == fig_sphere && t2 == fig_cylinder) {
        return isCellsOverlapSphCyl(cell1, cell2);
    }
    if (t2 == fig_sphere && t1 == fig_cylinder) {
        return isCellsOverlapSphCyl(cell2, cell1);
    }
    if (t1 == fig_cylinder && t2 == fig_cylinder) {
        return isCellsOverlapCylCyl(cell1, cell2);
    }
    return false;
}

#endif // AEROGEN_BASECELL_H
