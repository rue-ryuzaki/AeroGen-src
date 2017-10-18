#include "devfield.h"

#include <iostream>

//#define FMASK

DevField::DevField(Sizes size, double d)
    : m_size(size)
{
    m_div = uint8_t(ceil(7 / d));
    m_field = new uint8_t**[size.x * m_div];
#ifdef FMASK
    m_mask = new int8_t**[size.x * div];
#endif
    for (uint32_t ix = 0; ix < size.x * m_div; ++ix) {
        m_field[ix] = new uint8_t*[size.y * m_div];
#ifdef FMASK
        m_mask[ix] = new int8_t*[size.y * div];
#endif
        for (uint32_t iy = 0; iy < size.y * m_div; ++iy) {
            m_field[ix][iy] = new uint8_t[size.z * m_div];
#ifdef FMASK
            m_mask[ix][iy] = new int8_t[size.z * div];
#endif
            for (uint32_t iz = 0; iz < size.z * m_div; ++iz) {
                m_field[ix][iy][iz] = d_empty;
#ifdef FMASK
                m_mask[ix][iy][iz] = min(min(min(ix, size.x * div - ix - 1),
                        min(iy, size.y * div - iy - 1)), min(iz, size.z * div - iz - 1));
//#else
                //m_mask[ix][iy][iz] = -1;
#endif
            }
        }
    }
}

DevField::~DevField()
{
    for (uint32_t ix = 0; ix < m_size.x * m_div; ++ix) {
        for (uint32_t iy = 0; iy < m_size.y * m_div; ++iy) {
            delete [] m_field[ix][iy];
#ifdef FMASK
            delete [] m_mask[ix][iy];
#endif
        }
        delete [] m_field[ix];
#ifdef FMASK
        delete [] m_mask[ix];
#endif
    }
    delete [] m_field;
#ifdef FMASK
    delete [] m_mask;
#endif
}

DevField* DevField::loadFromField(const Field* fld, double d)
{
    DevField* result = new DevField(fld->sizes(), d);
    for (const Cell& cell : fld->cells()) {
        dCoord centre = cell.coord() * result->m_div;
        double r = cell.figure()->radius() * result->m_div;
        int32_t x1 = std::max(int32_t(centre.x - r), 0);
        double x2 = std::min(centre.x + r, double(fld->sizes().x) * result->m_div);
        int32_t y1 = std::max(int32_t(centre.y - r), 0);
        double y2 = std::min(centre.y + r, double(fld->sizes().y) * result->m_div);
        int32_t z1 = std::max(int32_t(centre.z - r), 0);
        double z2 = std::min(centre.z + r, double(fld->sizes().z) * result->m_div);
        for (int32_t ix = x1; ix < x2; ++ix) {
            for (int32_t iy = y1; iy < y2; ++iy) {
                for (int32_t iz = z1; iz < z2; ++iz) {
                    if (result->overlap(ix, iy, iz, centre, r)) {
                        result->m_field[ix][iy][iz] = d_solid;
#ifdef FMASK
                        result->m_mask[ix][iy][iz] = 0;
#endif
                    }
                }
            }
        }
#ifdef FMASK
        // var 1
        for (uint32_t ix = 0; ix < result->size.x * result->div; ++ix) {
            for (uint32_t iy = 0; iy < result->size.y * result->div; ++iy) {
                for (uint32_t iz = 0; iz < result->size.z * result->div; ++iz) {
                    uint32_t l = uint32_t(std::max(ceil(result->leng(ix, iy, iz, centre) - r), 0.0)) + 1;
                    if (result->m_mask[ix][iy][iz] > l) {
                        result->m_mask[ix][iy][iz] = l;
                    }
                }
            }
        }
#endif
    }
#ifdef FMASK
    // var 2
    for (uint32_t ix = 0; ix < result->size.x * result->div; ++ix) {
        for (uint32_t iy = 0; iy < result->size.y * result->div; ++iy) {
            for (uint32_t iz = 0; iz < result->size.z * result->div; ++iz) {
                
            }
        }
    }
#endif
    return result;
}

double DevField::volume(double r)
{
    // mask
    maskField(r);
    // end mask
    uint32_t volume = maskCountAndClear();
    return double(volume) / (m_div * m_div * m_div);
}

bool DevField::overlap(int32_t x, int32_t y, int32_t z, dCoord& centre, double r)
{
    double rr = (centre.x - x) * (centre.x - x);
    rr += (centre.y - y) * (centre.y - y);
    rr += (centre.z - z) * (centre.z - z);
    return rr <= (r * r);
}

double DevField::leng(int32_t x, int32_t y, int32_t z, dCoord& centre)
{
    double result = (centre.x - x) * (centre.x - x);
    result += (centre.y - y) * (centre.y - y);
    result += (centre.z - z) * (centre.z - z);
    return sqrt(result);
}

uint32_t DevField::solidCount() const
{
    uint32_t result = 0;
    for (uint32_t ix = 0; ix < m_size.x * m_div; ++ix) {
        for (uint32_t iy = 0; iy < m_size.y * m_div; ++iy) {
            for (uint32_t iz = 0; iz < m_size.z * m_div; ++iz) {
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
    std::vector<iCoord > shifts = createShifts(r);
    int32_t cmin = int32_t(r * m_div);
    int32_t xmax = int32_t((m_size.x - r) * m_div);
    int32_t ymax = int32_t((m_size.y - r) * m_div);
    int32_t zmax = int32_t((m_size.z - r) * m_div);
    for (int32_t ix = cmin; ix <= xmax; ++ix) {
        for (int32_t iy = cmin; iy <= ymax; ++iy) {
            for (int32_t iz = cmin; iz <= zmax; ++iz) {
#ifdef FMASK
                if (cmin <= m_mask[ix][iy][iz]) {
#else
                bool ok = true;
                for (const iCoord& sh : shifts) {
                    if (m_field[ix + sh.x][iy + sh.y][iz + sh.z] == d_solid) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
#endif
                    // set mask
                    for (const iCoord& sh : shifts) {
                        m_field[ix + sh.x][iy + sh.y][iz + sh.z] = d_mask;
                    }
                }
            }
        }
    }
}

uint32_t DevField::maskCountAndClear()
{
    uint32_t result = 0;
    for (uint32_t ix = 0; ix < m_size.x * m_div; ++ix) {
        for (uint32_t iy = 0; iy < m_size.y * m_div; ++iy) {
            for (uint32_t iz = 0; iz < m_size.z * m_div; ++iz) {
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
    for (uint32_t ix = 0; ix < m_size.x * m_div; ++ix) {
        for (uint32_t iy = 0; iy < m_size.y * m_div; ++iy) {
            for (uint32_t iz = 0; iz < m_size.z * m_div; ++iz) {
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
    double cmax = r * m_div;
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
