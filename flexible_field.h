#ifndef FLEXIBLE_FIELD_H
#define FLEXIBLE_FIELD_H

#include <algorithm>
#include <cstdint>
#include <vector>

#include "baseformats.h"

inline Sizes gridSizes(const Sizes& sizes, uint32_t gridsize)
{
    Sizes result;
    result.x = ((sizes.x % gridsize) == 0 ? (sizes.x / gridsize) : (sizes.x / gridsize + 1));
    result.y = ((sizes.y % gridsize) == 0 ? (sizes.y / gridsize) : (sizes.y / gridsize + 1));
    result.z = ((sizes.z % gridsize) == 0 ? (sizes.z / gridsize) : (sizes.z / gridsize + 1));
    return result;
}

template <class T>
class FlexibleField
{
public:
    FlexibleField()
        : m_clusters()
    { }
    virtual ~FlexibleField() { }

protected:
    void addCell(std::vector<std::vector<std::vector<std::vector<T> > > >& grid,
                 const T& cell, const Sizes& sizes, const Sizes& gsizes)
    {
        m_clusters.push_back({ cell });
        grid[uint32_t(cell.coord().x * gsizes.x / sizes.x)]
                [uint32_t(cell.coord().y * gsizes.y / sizes.y)]
                [uint32_t(cell.coord().z * gsizes.z / sizes.z)].push_back(cell);
    }

    std::vector<std::vector<std::vector<std::vector<T> > > >
    buildGrid(const Sizes& sizes, const Sizes& gsizes) const
    {
        std::vector<std::vector<std::vector<std::vector<T> > > > grid;
        grid.resize(gsizes.x);
        for (uint32_t x = 0; x < gsizes.x; ++x) {
            grid[x].resize(gsizes.y);
            for (uint32_t y = 0; y < gsizes.y; ++y) {
                grid[x][y].resize(gsizes.z);
            }
        }
        for (const std::vector<T>& vc : m_clusters) {
            for (const T& cell : vc) {
                grid[uint32_t(cell.coord().x * gsizes.x / sizes.x)]
                        [uint32_t(cell.coord().y * gsizes.y / sizes.y)]
                        [uint32_t(cell.coord().z * gsizes.z / sizes.z)].push_back(cell);
            }
        }
        return grid;
    }

    void rebuildGrid(std::vector<std::vector<std::vector<std::vector<T> > > >& grid,
                     const Sizes& sizes, const Sizes& gsizes) const
    {
        for (uint32_t x = 0; x < gsizes.x; ++x) {
            for (uint32_t y = 0; y < gsizes.y; ++y) {
                for (uint32_t z = 0; z < gsizes.z; ++z) {
                    grid[x][y][z].clear();
                }
            }
        }
        for (const std::vector<T>& vc : m_clusters) {
            for (const T& cell : vc) {
                grid[uint32_t(cell.coord().x * gsizes.x / sizes.x)]
                        [uint32_t(cell.coord().y * gsizes.y / sizes.y)]
                        [uint32_t(cell.coord().z * gsizes.z / sizes.z)].push_back(cell);
            }
        }
    }

    std::vector<std::vector<T> > m_clusters;
};

#endif // FLEXIBLE_FIELD_H
