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
    double      isToroid;
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
    uint32_t    method;
    std::string sizes;
    double      porosity;
    uint32_t    init;
    uint32_t    step;
    uint32_t    hit;
    uint32_t    cluster;
    double      cellSize;
    double      isToroid;

    RunParams getRunParams() const
    {
        RunParams result;
        result.porosity = porosity / 100.0;
        result.init     = init;
        result.step     = step;
        result.hit      = hit;
        result.cluster  = cluster;
        result.cellSize = cellSize;
        result.isToroid = isToroid;
        return result;
    }
};

#endif // AEROGEN_PARAMS_H
