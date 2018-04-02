#ifndef AEROGEN_PARAMS_H
#define AEROGEN_PARAMS_H

#include <cstdint>
#include <string>

struct RunParams
{
    double      porosity;
    uint32_t    init;
    uint32_t    step;
    uint32_t    hit;
    uint32_t    cluster;
    double      cellSize;
    bool        isToroid;
};

struct Parameters
{
    Parameters()
        : method(),
          sizes(),
          porosity(),
          init(),
          step(),
          hit(),
          cluster(),
          cellSize(),
          isToroid(true)
    { }
    int32_t     method;
    std::string sizes;
    double      porosity;
    int32_t     init;
    int32_t     step;
    int32_t     hit;
    int32_t     cluster;
    double      cellSize;
    bool        isToroid;

    RunParams getRunParams() const
    {
        RunParams result;
        result.porosity = porosity / 100.0;
        result.init     = uint32_t(init);
        result.step     = uint32_t(step);
        result.hit      = uint32_t(hit);
        result.cluster  = uint32_t(cluster);
        result.cellSize = cellSize;
        result.isToroid = isToroid;
        return result;
    }
};

#endif // AEROGEN_PARAMS_H
