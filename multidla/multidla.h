#ifndef MULTIDLA_H
#define	MULTIDLA_H

#include <QGLWidget>
#include <vector>

#include "cellsfield.h"
#include "counter.h"
#include "../basegenerator.h"

template <class T>
inline T sqr(T x)
{
    return x * x;
}

template <class FLD>
uint32_t cntNeighbors(FLD* fld, const MCoordVec* mapNeigh, const MCoord& currCoord)
{
    uint32_t res = 0;
    MCoord c;
    for (size_t i = 0; i < mapNeigh->size(); ++i) {
        MCoord neighCoord = (*mapNeigh)[i];
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
    void    generate(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                     uint32_t hit, uint32_t cluster, double cellsize);

    double  surfaceArea(double density) const;
    void    density(double density, double& denAero, double& porosity) const;
    
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);
    
private:
    uint32_t random(uint32_t max) const; // returns integer random value [0 .. max)
    MCoord  toroidizeCoords(const MCoord& coords, const MCoord& dim);
    MCoordVec* createNeighborsMap(uint32_t dimensions) const;
    void    fillDims(MCoordVec* mapSteps, uint32_t currDim, MCoord& otherDims,
                     uint32_t dims, uint32_t step);
    MCoordVec* createStepMap(uint32_t dims, uint32_t step);
    MCoord  makeStep(const MCoord& currCoord, MCoordVec* mapSteps);
    MCoord  randomPntInFld(MCoord fldSize) const;
    MCoord  randomPntOnLiveCircle(uint32_t radius) const;
    bool    isPntOutOfRadius(const MCoord& pnt, uint32_t radius) const;

    uint32_t deathRadius(uint32_t liveRadius);
    void    cMultiDLA(CellsField* m_fld, double targetPorosity, uint32_t initN,
                      uint32_t step = 1, uint32_t hitCnt = 1);
    MCoord  freeRandomPntInField(CellsField* m_fld);
    double  vAdd(CellsField* m_fld, const MCoord& curr) const;
    FieldElement deepestMark(std::map<FieldElement, FieldElement>& substitute, FieldElement mark);
    CellsField* markClusters(const CellsField* m_fld);
    std::vector<MCoordVec>* extractClusters(CellsField* MarkedFld);
    MCoordVec* moveCluster(MCoordVec* cluster, MCoordVec* directions);
    MCoordVec* createDirections();
    bool    isClusterInField(MCoordVec* cluster, CellsField* m_fld) const;
    void    setClusterVal(MCoordVec* cluster, CellsField* m_fld, FieldElement value);
    void    removeCluster(MCoordVec* cluster, CellsField* m_fld);
    void    restoreCluster(MCoordVec* cluster, CellsField* m_fld);
    bool    isAggregation(MCoordVec* cluster, CellsField* m_fld, MCoordVec* directions) const;
    void    clusterAggregation(CellsField* m_fld, size_t cluster_cnt = 1);
    void    printField(CellsField* m_fld) const;
    void    testMarkClusters();
    MCoord  randomPoint(const MCoord& sz) const;
    void    changeLabels(CellsField* m_fld, double fraction, FieldElement lbl);

    CellsField* m_fld = nullptr;
};

#endif	// MULTIDLA_H
