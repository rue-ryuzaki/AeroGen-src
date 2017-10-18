#ifndef PARAMS_H
#define	PARAMS_H

#include <cstdint>
#include <string>

struct Parameters
{
    uint32_t    method;
    std::string sizes;
    double      porosity;
    uint32_t    init;
    uint32_t    step;
    uint32_t    hit;
    uint32_t    cluster;
    double      cellSize;
};

#endif	// PARAMS_H
