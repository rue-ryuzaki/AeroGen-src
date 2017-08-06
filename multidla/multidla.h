#ifndef MULTIDLA_H
#define	MULTIDLA_H

#include <QGLWidget>
#include <vector>

#include "cellsfield.h"
#include "counter.h"
#include "../basegenerator.h"

template <typename T>
inline T sqr(T x)
{
    return x * x;
}

template <class FLD>
int cntNeighbors(FLD* fld, const MCoordVec* mapNeigh, const MCoord& currCoord)
{
    int res = 0;

    size_t currNeigh;
    MCoord c;
    for (currNeigh = 0; currNeigh < mapNeigh->size(); ++currNeigh) {
        MCoord neighCoord = (*mapNeigh)[currNeigh];
        c = MCoord(currCoord + neighCoord);
        try {
            if (fld->isSet(c)) {
                ++res;
            }
        } catch (MOutOfBoundError& e) { }
    }
    return res;
}

class MultiDLA : public Generator
{
public:
    MultiDLA(QObject* parent);
    virtual ~MultiDLA();

    CellsField* field() const;
    void    generate(const Sizes& sizes, double por, int initial, int step, int hit,
                  size_t cluster, double cellsize);

    double  surfaceArea(double density) const;
    void    density(double density, double& denAero, double& porosity) const;
    
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);
    
private:
    int     random(int max); // returns integer random value [0 .. max)
    MCoord  toroidizeCoords(const MCoord& coords, const MCoord& dim);
    MCoordVec* createNeighborsMap(int dimensions) const;
    void    fillDims(MCoordVec* mapSteps, int currDim, MCoord& otherDims, int dims, int step);
    MCoordVec* createStepMap(int dims, int step);
    MCoord  makeStep(const MCoord& currCoord, MCoordVec* mapSteps);
    MCoord  randomPntInFld(MCoord fldSize);
    MCoord  randomPntOnLiveCircle(int radius);
    bool    isPntOutOfRadius(const MCoord& pnt, int radius);

    int     deathRadius(int liveRadius);
    void    cMultiDLA(CellsField* m_fld, double targetPorosity, int initN, int step = 1, int hitCnt = 1);
    MCoord  freeRandomPntInField(CellsField* m_fld);
    double  vAdd(CellsField* m_fld, const MCoord& curr) const;
    FieldElement deepestMark(std::map<FieldElement, FieldElement>& substitute, FieldElement mark);
    CellsField* markClusters(const CellsField* m_fld);
    std::vector<MCoordVec>* extractClusters(CellsField* MarkedFld);
    MCoordVec* moveCluster(MCoordVec* cluster, MCoordVec* directions);
    MCoordVec* createDirections();
    bool    isClusterInField(MCoordVec* cluster, CellsField* m_fld);
    void    setClusterVal(MCoordVec* cluster, CellsField* m_fld, FieldElement value);
    void    removeCluster(MCoordVec* cluster, CellsField* m_fld);
    void    restoreCluster(MCoordVec* cluster, CellsField* m_fld);
    bool    isAggregation(MCoordVec* cluster, CellsField* m_fld, MCoordVec* directions);
    void    clusterAggregation(CellsField* m_fld, size_t cluster_cnt = 1);
    void    printField(CellsField* m_fld) const;
    void    testMarkClusters();
    //void PrintUsageString(char* argv[]) {
    //    cout << "Usage: " << argv[0] << " ConfigFile.yml\n";
    //}
    MCoord  randomPoint(const MCoord& sz);
    void    changeLabels(CellsField* m_fld, double fraction, FieldElement lbl);

    CellsField* m_fld = nullptr;
};

#endif	// MULTIDLA_H
