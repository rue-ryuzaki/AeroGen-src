#ifndef PARAMS_H
#define	PARAMS_H

#include <string>

struct Parameters
{
    int         method;
    std::string sizes;
    double      porosity;
    int         init;
    int         step;
    int         hit;
    int         cluster;
    double      cellSize;
};

#endif	// PARAMS_H
