#include "ccoord.h"

CCoord CCoord::operator* (const double & v) const {
    CCoord res;
    res.x = this->x * v;
    res.y = this->y * v;
    res.z = this->z * v;
    return res;
}

CCoord CCoord::operator* (const unsigned int & v) const {
    CCoord res;
    res.x = this->x * v;
    res.y = this->y * v;
    res.z = this->z * v;
    return res;
}

CCoord CCoord::operator/ (const unsigned int & v) const {
    CCoord res;
    res.x = this->x / v;
    res.y = this->y / v;
    res.z = this->z / v;
    return res;
}

CCoord CCoord::operator+ (const CCoord &rhs) const {
    CCoord res;
    res.x = this->x + rhs.x;
    res.y = this->y + rhs.y;
    res.z = this->z + rhs.z;
    return res;
}

CCoord CCoord::operator- (const CCoord & rhs) const {
    CCoord res;
    res.x = this->x - rhs.x;
    res.y = this->y - rhs.y;
    res.z = this->z - rhs.z;
    return res;
}

CCoord CCoord::operator+ (const Sizes &rhs) const {
    CCoord res;
    res.x = this->x + rhs.x;
    res.y = this->y + rhs.y;
    res.z = this->z + rhs.z;
    return res;
}

CCoord CCoord::operator- (const Sizes & rhs) const {
    CCoord res;
    res.x = this->x - rhs.x;
    res.y = this->y - rhs.y;
    res.z = this->z - rhs.z;
    return res;
}

CCoord & CCoord::operator= (const CCoord & rhs) {
    if (this != &rhs) {
        this->x = rhs.x;
        this->y = rhs.y;
        this->z = rhs.z;
    }
    return *this;
}
