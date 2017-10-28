#ifndef FLEXIBLE_FIELD_H
#define FLEXIBLE_FIELD_H

#include <algorithm>
#include <cstdint>
#include <vector>

#include "baseformats.h"

template <class T>
class FlexibleField
{
public:
    FlexibleField()
        : m_clusters()
    { }
    virtual ~FlexibleField() { }

protected:
    std::vector<std::vector<T> > m_clusters;
    const double EPS = -1e-4;
};

#endif // FLEXIBLE_FIELD_H
