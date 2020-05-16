#ifndef AEROGEN_MULTIDLA_MULTIDLA_H
#define AEROGEN_MULTIDLA_MULTIDLA_H

#include <vector>

#include "field.h"
#include "counter.h"
#include "../basegenerator.h"

class MultiDLA : public Generator
{
public:
    MultiDLA(QObject* parent);
    virtual ~MultiDLA();

    multidla::XField* field() const;
    void    generate(const Sizes& sizes, const RunParams& params);
    double  surfaceArea(double density, uint32_t steps) const;
    void    density(double density, double& denAero, double& porosity) const;
    void    save(const char* fileName, txt_format format) const;
    void    load(const char* fileName, txt_format format);
    
private:
    uint32_t random(uint32_t max) const; // returns integer random value [0 .. max)
    multidla::MCoord  toroidizeCoords(const multidla::MCoord& coords, const multidla::MCoord& dim);
    multidla::MCoordVec* createNeighborsMap(size_t dimensions) const;
    void    fillDims(multidla::MCoordVec* mapSteps, size_t currDim, multidla::MCoord& otherDims,
                     size_t dims, uint32_t step);
    multidla::MCoordVec* createStepMap(size_t dims, uint32_t step);
    multidla::MCoord  makeStep(const multidla::MCoord& currCoord, multidla::MCoordVec* mapSteps);
    multidla::MCoord  randomPntInFld(multidla::MCoord fldSize) const;
    multidla::MCoord  randomPntOnLiveCircle(uint32_t radius) const;
    bool    isPntOutOfRadius(const multidla::MCoord& pnt, uint32_t radius) const;

    uint32_t deathRadius(uint32_t liveRadius);
    void    cMultiDLA(multidla::XField* fld, double targetPorosity, uint32_t initN,
                      uint32_t step = 1, uint32_t hitCnt = 1);
    multidla::MCoord  freeRandomPntInField(multidla::XField* fld);
    double  vAdd(multidla::XField* fld, const multidla::MCoord& curr) const;
    multidla::FieldElement deepestMark(std::map<multidla::FieldElement, multidla::FieldElement>& substitute, multidla::FieldElement mark);
    multidla::XField* markClusters(const multidla::XField* fld);
    std::vector<multidla::MCoordVec>* extractClusters(multidla::XField* MarkedFld);
    multidla::MCoordVec* moveCluster(multidla::MCoordVec* cluster, multidla::MCoordVec* directions);
    multidla::MCoordVec* createDirections();
    bool    isClusterInField(multidla::MCoordVec* cluster, multidla::XField* fld) const;
    void    setClusterVal(multidla::MCoordVec* cluster, multidla::XField* fld, multidla::FieldElement value);
    void    removeCluster(multidla::MCoordVec* cluster, multidla::XField* fld);
    void    restoreCluster(multidla::MCoordVec* cluster, multidla::XField* fld);
    bool    isAggregation(multidla::MCoordVec* cluster, multidla::XField* fld, multidla::MCoordVec* directions) const;
    void    clusterAggregation(multidla::XField* fld, size_t cluster_cnt = 1);
    void    printField(multidla::XField* fld) const;
    void    testMarkClusters();
    multidla::MCoord  randomPoint(const multidla::MCoord& sz) const;
    void    changeLabels(multidla::XField* fld, double fraction, multidla::FieldElement lbl);

    multidla::XField* m_fld = nullptr;

    MultiDLA(const MultiDLA&) = delete;
    MultiDLA& operator =(const MultiDLA&) = delete;
};

#endif // AEROGEN_MULTIDLA_MULTIDLA_H
