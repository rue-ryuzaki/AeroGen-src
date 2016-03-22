#ifndef DLCA_FIELD_H
#define	DLCA_FIELD_H

#include <vector>
#include <cmath>
#include "../basefield.h"
#include "../figure.h"

using namespace std;

class CCell : public Cell {
public:
    CCell(Figure * figure, Coord<double> coord = Coord<double>(0.0, 0.0, 0.0),
            Vector3d rotate = Vector3d(0.0, 0.0, 0.0), Vector3d vec = Vector3d(0.0, 0.0, 0.0))
            : Cell(figure, coord, rotate), vec(vec) { }
    ~CCell () { }
    
    inline Vector3d getVector() const { return vec; }
    void setVector(Vector3d vec) { this->vec = vec; }
    void move(double t, Sizes & cs) {
        coord = coord + vec * t;
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
    }
    
private:
    Vector3d vec; // speed vector
};

typedef vector<CCell> vcell;
//#define DIA 10
//typedef vcell vcellD[DIA][DIA];

class CField : public Field {
public:
    CField(const char * fileName, txt_format format);
    CField(Sizes sizes = Sizes(50, 50, 50));
    virtual ~CField();
    
    Sizes getSizes() const;
    vector<Cell> getCells() const;
    vector<vcell> getClusters() const;

    void Initialize(double porosity, double cellsize);
    void InitializeTEST(double porosity, double cellsize);
    void InitializeNT(double porosity, double cellsize);
    int MonteCarlo(int stepMax);
    //vector<vcell>[q][q] getCells() const { return vcells; }

    void Agregate();
    void Move();
    double overlapVolume();
    
private:
    void toDAT(const char * fileName) const;
    void toDLA(const char * fileName) const;
    void toTXT(const char * fileName) const;
    void fromDAT(const char * fileName);
    void fromDLA(const char * fileName);
    void fromTXT(const char * fileName);

    double fr(double ravr);
    void clearCells();

    double overlapVolume_sphere_sphere(const CCell & cell1, const CCell & cell2);
    double overlapVolume_sphere_cylinder(const CCell & cell1, const CCell & cell2);
    double overlapVolume_cylinder_cylinder(const CCell & cell1, const CCell & cell2);
    bool is_overlap_sphere_sphere(const CCell & cell1, const CCell & cell2);
    bool is_overlap_sphere_cylinder(const CCell & cell1, const CCell & cell2);
    bool is_overlap_cylinder_cylinder(const CCell & cell1, const CCell & cell2);
    bool is_overlap_cylinders_point(const Coord<double>& base1, const Coord<double>& base2,
        double r1, const Coord<double>& other, const Vector3d& area, double r2);
    bool is_overlapped(const CCell & cell1, const CCell & cell2);
    bool is_point_overlap_spheres(const CCell & cell);

    Coord<double> Diff(const Coord<double> & c1, const Coord<double> & c2);
    void inPareList(vector<vui> & agregate, Pare & pare);
    double leng(const CCell & cell1, const CCell & cell2);
    double quad(double x) const;
    
    vector<vcell> clusters;
    //static const int q = 10;
    //vector<vcell> vcells[q][q];
    double dt = 0.1;
    const double EPS = -1e-4;
};

#endif	/* DLCA_FIELD_H */
