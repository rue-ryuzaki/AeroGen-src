#include "devfield.h"

#include <iostream>
#include <limits>

static bool overlap(int32_t x, int32_t y, int32_t z, const dCoord& centre, double r)
{
    double rr = (centre.x - x) * (centre.x - x);
    rr += (centre.y - y) * (centre.y - y);
    rr += (centre.z - z) * (centre.z - z);
    return rr <= (r * r);
}

//static double leng(int32_t x, int32_t y, int32_t z, const dCoord& centre)
//{
//    double result = (centre.x - x) * (centre.x - x);
//    result += (centre.y - y) * (centre.y - y);
//    result += (centre.z - z) * (centre.z - z);
//    return std::sqrt(result);
//}

// -----------------------------------------------------------------------------
DevField::~DevField()
{
}

DevField* DevField::loadFromField(const Field* fld, double d, bool isToroid)
{
    DevField* result = new DevField(fld->sizes(), d, isToroid);
    std::vector<Cell> cells = fld->cells();
    int32_t xmax, ymax, zmax, xmin, ymin, zmin;
    xmax = 0, ymax = 0, zmax = 0;
    xmin = ymin = zmin = std::numeric_limits<int32_t>::max();
    for (const auto& cell : cells) {
        dCoord centre = cell.coord() * result->m_div;
        double r = cell.figure()->radius() * result->m_div;
        int32_t x1 = std::max(int32_t(centre.x - r), 0);
        double x2 = std::min(centre.x + r, double(fld->sizes().x * result->m_div));
        int32_t y1 = std::max(int32_t(centre.y - r), 0);
        double y2 = std::min(centre.y + r, double(fld->sizes().y * result->m_div));
        int32_t z1 = std::max(int32_t(centre.z - r), 0);
        double z2 = std::min(centre.z + r, double(fld->sizes().z * result->m_div));
        for (int32_t ix = x1; ix < x2; ++ix) {
            for (int32_t iy = y1; iy < y2; ++iy) {
                for (int32_t iz = z1; iz < z2; ++iz) {
                    if (overlap(ix, iy, iz, centre, r)) {
                        if (ix < xmin) {
                            xmin = ix;
                        }
                        if (iy < ymin) {
                            ymin = iy;
                        }
                        if (iz < zmin) {
                            zmin = iz;
                        }
                        if (ix > xmax) {
                            xmax = ix;
                        }
                        if (iy > ymax) {
                            ymax = iy;
                        }
                        if (iz > zmax) {
                            zmax = iz;
                        }
                        result->m_field[size_t(ix)][size_t(iy)][size_t(iz)] = d_solid;
#ifdef FMASK
                        result->m_mask[ix][iy][iz] = 0;
#endif // FMASK
                    }
                }
            }
        }
#ifdef FMASK
        // var 1
        for (uint32_t ix = 0; ix < result->m_field.size(); ++ix) {
            for (uint32_t iy = 0; iy < result->m_field[ix].size(); ++iy) {
                for (uint32_t iz = 0; iz < result->m_field[ix][iy].size(); ++iz) {
                    uint16_t l = uint16_t(std::max(ceil(result->leng(ix, iy, iz, centre) - r), 0.0)) + 1;
                    if (result->m_mask[ix][iy][iz] > l) {
                        result->m_mask[ix][iy][iz] = l;
                    }
                }
            }
        }
#endif // FMASK
    }
#ifdef FMASK
    // var 2
//    for (uint32_t ix = 0; ix < result->m_field.size(); ++ix) {
//        for (uint32_t iy = 0; iy < result->m_field[ix].size(); ++iy) {
//            for (uint32_t iz = 0; iz < result->m_field[ix][iy].size(); ++iz) {
                
//            }
//        }
//    }
#endif // FMASK
    if (xmax > xmin && ymax > ymin && zmax > zmin) {
        for (size_t ix = 0; ix < size_t(xmax - xmin); ++ix) {
            for (size_t iy = 0; iy < size_t(ymax - ymin); ++iy) {
                for (size_t iz = 0; iz < size_t(zmax - zmin); ++iz) {
                    result->m_field[ix][iy][iz] = result->m_field[ix + size_t(xmin)][iy + size_t(ymin)][iz + size_t(zmin)];
                }
            }
        }
        result->m_field.resize(size_t(xmax - xmin));
        for (size_t ix = 0; ix < result->m_field.size(); ++ix) {
            result->m_field[ix].resize(size_t(ymax - ymin));
            for (size_t iy = 0; iy < result->m_field[ix].size(); ++iy) {
                result->m_field[ix][iy].resize(size_t(zmax - zmin));
            }
        }
    }
    std::cout << "Loaded" << std::endl;
    return result;
}

double DevField::volume(double r)
{
    // mask
    maskField(r);
    // end mask
    uint32_t volume = maskCountAndClear();
    return double(volume) / cube(m_div);
}

DevField::DevField(const Sizes& size, double /*d*/, bool isToroid)
    : m_div(1),//uint16_t(ceil(7 / d))),
      m_field(),
#ifdef FMASK
      m_mask(),
#endif // FMASK
      m_isToroid(isToroid)
{
    m_field.resize(size.x * m_div);
#ifdef FMASK
    m_mask.resize(size.x * m_div);
#endif // FMASK
    for (size_t ix = 0; ix < m_field.size(); ++ix) {
        m_field[ix].resize(size.y * m_div);
#ifdef FMASK
        m_mask[ix].resize(size.y * m_div);
#endif // FMASK
        for (size_t iy = 0; iy < m_field[ix].size(); ++iy) {
            m_field[ix][iy].resize(size.z * m_div, d_empty);
#ifdef FMASK
            m_mask[ix][iy].resize(size.z * m_div);
            for (size_t iz = 0; iz < m_field[ix][iy].size(); ++iz) {
                m_mask[ix][iy][iz] = std::min(std::min(std::min(ix, size.x * m_div - ix - 1),
                        std::min(iy, size.y * m_div - iy - 1)), std::min(iz, size.z * m_div - iz - 1));
//#else
                //m_mask[ix][iy][iz] = -1;
            }
#endif // FMASK
        }
    }
}

uint32_t DevField::solidCount() const
{
    uint32_t result = 0;
    for (size_t ix = 0; ix < m_field.size(); ++ix) {
        for (size_t iy = 0; iy < m_field[ix].size(); ++iy) {
            for (size_t iz = 0; iz < m_field[ix][iy].size(); ++iz) {
                if (m_field[ix][iy][iz] == d_solid) {
                    ++result;
                }
            }
        }
    }
    return result;
}

void DevField::maskField(double r)
{
    const std::vector<iCoord > shifts = createShifts(r);
    const int32_t cmin = int32_t(r * m_div);
    const int32_t xmax = int32_t(m_field.size()) - cmin;
    const int32_t ymax = int32_t(m_field[0].size()) - cmin;
    const int32_t zmax = int32_t(m_field[0][0].size()) - cmin;
    for (int32_t ix = cmin; ix <= xmax; ++ix) {
        for (int32_t iy = cmin; iy <= ymax; ++iy) {
            for (int32_t iz = cmin; iz <= zmax; ++iz) {
#ifdef FMASK
                if (cmin <= m_mask[ix][iy][iz]) {
#else
                bool ok = true;
                for (const auto& sh : shifts) {
                    if (m_field[size_t(ix + sh.x)][size_t(iy + sh.y)][size_t(iz + sh.z)] == d_solid) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
#endif // FMASK
                    // set mask
                    for (const auto& sh : shifts) {
                        m_field[size_t(ix + sh.x)][size_t(iy + sh.y)][size_t(iz + sh.z)] = d_mask;
                    }
                }
            }
        }
    }
}

uint32_t DevField::maskCountAndClear()
{
    uint32_t result = 0;
    for (size_t ix = 0; ix < m_field.size(); ++ix) {
        for (size_t iy = 0; iy < m_field[ix].size(); ++iy) {
            for (size_t iz = 0; iz < m_field[ix][iy].size(); ++iz) {
                if (m_field[ix][iy][iz] == d_mask) {
                    m_field[ix][iy][iz] = d_empty;
                    ++result;
                }
            }
        }
    }
    return result;
}

void DevField::clearMask()
{
    for (size_t ix = 0; ix < m_field.size(); ++ix) {
        for (size_t iy = 0; iy < m_field[ix].size(); ++iy) {
            for (size_t iz = 0; iz < m_field[ix][iy].size(); ++iz) {
                if (m_field[ix][iy][iz] == d_mask) {
                    m_field[ix][iy][iz] = d_empty;
                }
            }
        }
    }
}

std::vector<iCoord> DevField::createShifts(double r) const
{
    std::vector<iCoord> result;
    dCoord centre(0, 0, 0);
    int32_t cmin = int32_t(-r) * m_div;
    double cmax = r * double(m_div);
    for (int32_t ix = cmin; ix < cmax; ++ix) {
        for (int32_t iy = cmin; iy < cmax; ++iy) {
            for (int32_t iz = cmin; iz < cmax; ++iz) {
                if (overlap(ix, iy, iz, centre, cmax)) {
                    result.push_back(iCoord(ix, iy, iz));
                }
            }
        }
    }
    return result;
}
