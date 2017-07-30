#include "devfield.h"

#include <iostream>
#include <math.h>

//#define FMASK

DevField::DevField(Sizes size, double d)
    : size(size)
{
    div = int(ceil(7 / d));
    field = new int**[size.x * div];
#ifdef FMASK
    mfield = new int**[size.x * div];
#endif
    for (int ix = 0; ix < size.x * div; ++ix) {
        field[ix] = new int*[size.y * div];
#ifdef FMASK
        mfield[ix] = new int*[size.y * div];
#endif
        for (int iy = 0; iy < size.y * div; ++iy) {
            field[ix][iy] = new int[size.z * div];
#ifdef FMASK
            mfield[ix][iy] = new int[size.z * div];
#endif
            for (int iz = 0; iz < size.z * div; ++iz) {
                field[ix][iy][iz] = d_empty;
#ifdef FMASK
                mfield[ix][iy][iz] = min(min(min(ix, size.x * div - ix - 1), 
                        min(iy, size.y * div - iy - 1)), min(iz, size.z * div - iz - 1));
//#else
                //mfield[ix][iy][iz] = -1;
#endif
            }
        }
    }
}

DevField::~DevField()
{
    for (int ix = 0; ix < size.x * div; ++ix) {
        for (int iy = 0; iy < size.y * div; ++iy) {
            delete [] field[ix][iy];
#ifdef FMASK
            delete [] mfield[ix][iy];
#endif
        }
        delete [] field[ix];
#ifdef FMASK
        delete [] mfield[ix];
#endif
    }
    delete [] field;
#ifdef FMASK
    delete [] mfield;
#endif
}

DevField* DevField::LoadFromField(const Field* fld, double d)
{
    DevField* result = new DevField(fld->getSizes(), d);
    for (const Cell& cell : fld->getCells()) {
        dCoord centre = cell.getCoord() * result->div;
        double r = cell.getFigure()->getRadius() * result->div;
        int x1 = std::max(int(centre.x - r), 0);
        double x2 = std::min(centre.x + r, double(fld->getSizes().x) * result->div);
        int y1 = std::max(int(centre.y - r), 0);
        double y2 = std::min(centre.y + r, double(fld->getSizes().y) * result->div);
        int z1 = std::max(int(centre.z - r), 0);
        double z2 = std::min(centre.z + r, double(fld->getSizes().z) * result->div);
        for (int ix = x1; ix < x2; ++ix) {
            for (int iy = y1; iy < y2; ++iy) {
                for (int iz = z1; iz < z2; ++iz) {
                    if (result->overlap(ix, iy, iz, centre, r)) {
                        result->field[ix][iy][iz] = d_solid;
#ifdef FMASK
                        result->mfield[ix][iy][iz] = 0;
#endif
                    }
                }
            }
        }
#ifdef FMASK
        // var 1
        for (int ix = 0; ix < result->size.x * result->div; ++ix) {
            for (int iy = 0; iy < result->size.y * result->div; ++iy) {
                for (int iz = 0; iz < result->size.z * result->div; ++iz) {
                    int l = int(std::max(ceil(result->leng(ix, iy, iz, centre) - r), 0.0)) + 1;
                    if (result->mfield[ix][iy][iz] > l) {
                        result->mfield[ix][iy][iz] = l;
                    }
                }
            }
        }
#endif
    }
#ifdef FMASK
    // var 2
    for (int ix = 0; ix < result->size.x * result->div; ++ix) {
        for (int iy = 0; iy < result->size.y * result->div; ++iy) {
            for (int iz = 0; iz < result->size.z * result->div; ++iz) {
                
            }
        }
    }
#endif
    return result;
}

double DevField::getVolume(double r)
{
    // mask
    maskField(r);
    // end mask
    long volume = maskCountAndClear();
    return double(volume) / (div * div * div);
}

bool DevField::overlap(int x, int y, int z, dCoord& centre, double r)
{
    double rr = (centre.x - x) * (centre.x - x);
    rr += (centre.y - y) * (centre.y - y);
    rr += (centre.z - z) * (centre.z - z);
    return rr <= (r * r);
}

double DevField::leng(int x, int y, int z, dCoord& centre)
{
    double result = (centre.x - x) * (centre.x - x);
    result += (centre.y - y) * (centre.y - y);
    result += (centre.z - z) * (centre.z - z);
    return pow(result, 0.5);
}

int DevField::solidCount() const
{
    int result = 0;
    for (int ix = 0; ix < size.x * div; ++ix) {
        for (int iy = 0; iy < size.y * div; ++iy) {
            for (int iz = 0; iz < size.z * div; ++iz) {
                if (field[ix][iy][iz] == d_solid) {
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
    int cmin = int(r * div);
    int xmax = int((size.x - r) * div);
    int ymax = int((size.y - r) * div);
    int zmax = int((size.z - r) * div);
    for (int ix = cmin; ix <= xmax; ++ix) {
        for (int iy = cmin; iy <= ymax; ++iy) {
            for (int iz = cmin; iz <= zmax; ++iz) {
#ifdef FMASK
                if (cmin <= mfield[ix][iy][iz]) {
#else
                bool ok = true;
                for (const iCoord& sh : shifts) {
                    if (field[ix + sh.x][iy + sh.y][iz + sh.z] == d_solid) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
#endif
                    // set mask
                    for (const iCoord& sh : shifts) {
                        field[ix + sh.x][iy + sh.y][iz + sh.z] = d_mask;
                    }
                }
            }
        }
    }
}

long DevField::maskCountAndClear()
{
    long result = 0;
    for (int ix = 0; ix < size.x * div; ++ix) {
        for (int iy = 0; iy < size.y * div; ++iy) {
            for (int iz = 0; iz < size.z * div; ++iz) {
                if (field[ix][iy][iz] == d_mask) {
                    field[ix][iy][iz] = d_empty;
                    ++result;
                }
            }
        }
    }
    return result;
}

void DevField::clearMask()
{
    for (int ix = 0; ix < size.x * div; ++ix) {
        for (int iy = 0; iy < size.y * div; ++iy) {
            for (int iz = 0; iz < size.z * div; ++iz) {
                if (field[ix][iy][iz] == d_mask) {
                    field[ix][iy][iz] = d_empty;
                }
            }
        }
    }
}

std::vector<iCoord> DevField::createShifts(double r) const
{
    std::vector<iCoord> result;
    dCoord centre(0, 0, 0);
    int cmin = int(-r) * div;
    double cmax = r * div;
    for (int ix = cmin; ix < cmax; ++ix) {
        for (int iy = cmin; iy < cmax; ++iy) {
            for (int iz = cmin; iz < cmax; ++iz) {
                if (overlap(ix, iy, iz, centre, cmax)) {
                    result.push_back(iCoord(ix, iy, iz));
                }
            }
        }
    }
    return result;
}
