#ifndef PARAMS_H
#define	PARAMS_H

struct Parameters {
    int     method;
    string  sizes;
    double  porosity;
    int     init;
    int     step;
    int     hit;
    int     cluster;
    double  cellSize;
};

#endif	/* PARAMS_H */
