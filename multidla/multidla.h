#ifndef MULTIDLA_H
#define	MULTIDLA_H

#include <QGLWidget>
#include "cellsfield.h"
#include "counter.h"
#include "../basegenerator.h"

template <typename T>
inline T sqr(T x) {
    return x * x;
}

template <class FLD>
int CntNeighbors(FLD * fld, const MCoordVec * mapNeigh, const MCoord & currCoord) {
    int res = 0;

    size_t currNeigh;
    MCoord c;
    for (currNeigh = 0; currNeigh < mapNeigh->size(); ++currNeigh) {
        MCoord neighCoord = (*mapNeigh)[currNeigh];
        c = MCoord(currCoord + neighCoord);
        try {
            if (fld->IsSet(c)) {
                ++res;
            }
        } catch (MOutOfBoundError e) { }
    }
    return res;
}

class MultiDLA : public Generator {
public:
    MultiDLA(QObject * parent) : Generator(parent) { };
    virtual ~MultiDLA() { };

    void Generate(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize);
    double SurfaceArea(double density);
    void Density(double density, double & denAero, double & porosity);
    CellsField * GetField() const { return fld; }
    
    void Save(const char * fileName, txt_format format) const { fld->toFile(fileName, format); }
    void Save(string fileName, txt_format format) const { fld->toFile(fileName.c_str(), format); }
    
    void Load(const char * fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new CellsField(fileName, format);
        finished = true;
    }
    void Load(string fileName, txt_format format) {
        if (fld != nullptr) {
            delete fld;
        }
        fld = new CellsField(fileName.c_str(), format);
        finished = true;
    }
    
private:
    int random(int max); // returns integer random value [0 .. max)
    MCoord ToroidizeCoords(const MCoord & coords, const MCoord & dim) { 
        return (coords + dim) % dim;
    }
    MCoordVec * CreateNeighborsMap(int dimensions);
    void FillDims(MCoordVec * mapSteps, int currDim, MCoord & otherDims, int dims, int step);
    MCoordVec * CreateStepMap(int dims, int step);
    MCoord MakeStep(const MCoord & currCoord, MCoordVec * mapSteps);
    MCoord RandomPntInFld(MCoord fldSize);
    MCoord RandomPntOnLiveCircle(int radius);
    bool IsPntOutOfRadius(const MCoord & pnt, int radius);
    double SqrFromR(int r) { return M_PI * sqr(r); }
    double RFromSqr(double sq) { return sqrt(sq / M_PI); }
    int cube(int val) { return pow((double)val, 3.0); }
    double cube(double val) const { return pow(val, 3.0); }
    double VolFromR(int r) { return (4.0 / 3) * M_PI * cube(r); }
    double VfromR(double r) const { return (4.0 / 3) * M_PI * cube(r); }
    double SfromR(double r) const { return 4.0 * M_PI * r * r; }
    double RFromVol(double vol) { return pow((3 * vol)/(4 * M_PI), 1.0 / 3.0); }
    int GetDeathRadius(int liveRadius);
    void cMultiDLA(CellsField *fld, double targetPorosity, int initN, int step = 1, int hitCnt = 1);
    MCoord FreeRandomPntInField(CellsField * fld);
    double V_add(CellsField * fld, const MCoord & curr);
    FieldElement GetDeepestMark(map<FieldElement, FieldElement> & substitute, FieldElement mark);
    CellsField * MarkClusters(const CellsField * fld);
    vector<MCoordVec> * ExtractClusters(CellsField * MarkedFld);
    MCoordVec * MoveCluster(MCoordVec * cluster, MCoordVec * directions);
    MCoordVec * CreateDirections();
    bool IsClusterInField(MCoordVec * cluster, CellsField * fld);
    void SetClusterVal(MCoordVec * cluster, CellsField * fld, FieldElement value);
    void RemoveCluster(MCoordVec * cluster, CellsField * fld) {
        SetClusterVal(cluster, fld, FREE_CELL);
    }
    void RestoreCluster(MCoordVec * cluster, CellsField * fld) {
        SetClusterVal(cluster, fld, OCUPIED_CELL);
    }
    bool IsAggregation(MCoordVec * cluster, CellsField * fld, MCoordVec * directions);
    void ClusterAggregation(CellsField * fld, size_t cluster_cnt = 1);
    void PrintField(CellsField * fld);
    void TestMarkClusters();
    //void PrintUsageString(char *argv[]) {
    //    cout << "Usage: " << argv[0] << " ConfigFile.yml\n";
    //}
    MCoord RandomPoint(const MCoord & sz);
    void ChangeLabels(CellsField * fld, double fraction, FieldElement lbl);

    CellsField * fld = nullptr;
};

#endif	/* MULTIDLA_H */
