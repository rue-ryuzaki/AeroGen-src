#ifndef AEROGEN_MULTIDLA_MULTIDLA_H
#define AEROGEN_MULTIDLA_MULTIDLA_H

#include <vector>

#include "counter.h"
#include "field.h"
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
    void    clusterAggregation(multidla::XField* fld, size_t clusterCnt = 1);
    void    testMarkClusters();

    multidla::XField* m_fld = nullptr;

    MultiDLA(const MultiDLA&) = delete;
    MultiDLA& operator =(const MultiDLA&) = delete;
};

#endif // AEROGEN_MULTIDLA_MULTIDLA_H
