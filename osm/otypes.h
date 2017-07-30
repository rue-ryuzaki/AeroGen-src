#ifndef OSM_TYPES_H
#define	OSM_TYPES_H

struct float4
{
    float4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f)
            : x(x),
              y(y),
              z(z),
              w(w)
    { }

    float x;
    float y;
    float z;
    float w;
};

typedef float4 sph;

#endif	// OSM_TYPES_H
