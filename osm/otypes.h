#ifndef AEROGEN_OSM_OTYPES_H
#define AEROGEN_OSM_OTYPES_H

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

#endif // AEROGEN_OSM_OTYPES_H
