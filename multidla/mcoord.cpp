#include "mcoord.h"

#include <iostream>

MCoord::MCoord(Coordinate X, Coordinate Y, Coordinate Z)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    m_dims = MCoord::m_defDims;

    setPosition(X,Y,Z);
}

MCoord::MCoord(Coordinate* cv)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    m_dims = MCoord::m_defDims;

    for (size_t i = 0; i < MCoord::m_defDims; ++i) {
        mCV[i] = cv[i];
    }
}

MCoord::MCoord(const std::vector<Coordinate>& cv)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    m_dims = MCoord::m_defDims;

    for (size_t i = 0; i < MCoord::m_defDims; ++i) {
        mCV[i] = cv[i];
    }
}

MCoord::MCoord(const MCoord& c)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    m_dims = MCoord::m_defDims;

    //mDims = c.mDims;
    for (size_t i = 0; i < MCoord::m_defDims; ++i) {
        mCV[i] = c.coord(i);
    }
}

MCoord::~MCoord()
{
    MCoord::instanceLock.lock();
    MCoord::instances -= 1;
    MCoord::instanceLock.unlock();
}

bool MCoord::checkBounds(size_t index) const
{
    return (index < MCoord::m_defDims);
}

Coordinate MCoord::coord(size_t num) const
{
    if (this->checkBounds(num)) {
        return mCV[num];
    }
    throw MOutOfBoundError();
}

void MCoord::setCoord(size_t num, Coordinate val)
{
    if (this->checkBounds(num)) {
        this->mCV[num] = val;
        return;
    }
    throw MOutOfBoundError();
}

void MCoord::setPosition(Coordinate X, Coordinate Y, Coordinate Z)
{
    mCV[0] = X;
    mCV[1] = Y;
    mCV[2] = Z;
}

MCoord MCoord::operator+ (const MCoord& rhs) const
{
    MCoord res;
    for (size_t i = 0; i < MCoord::m_defDims; ++i)
        res.setCoord(i, this->coord(i) + rhs.coord(i));
    return res;
}

MCoord MCoord::operator% (const MCoord& rhs) const
{
    MCoord res;
    for (size_t i = 0; i < MCoord::m_defDims; ++i)
        res.setCoord(i, this->coord(i) % ((rhs.coord(i) != 0) ? rhs.coord(i) : 1));
    return res;
}

MCoord MCoord::operator- (const MCoord& rhs) const
{
    MCoord res;
    for (size_t i = 0; i < MCoord::m_defDims; ++i)
        res.setCoord(i, this->coord(i) - rhs.coord(i));
    return res;
}

MCoord MCoord::operator/ (const Coordinate divide) const
{
    MCoord res(*this);

    for (size_t i = 0; i < MCoord::m_defDims; ++i)
        res.setCoord(i, this->coord(i) / divide);

    return res;
}

MCoord MCoord::operator+ (const Coordinate rhs) const
{
    MCoord tmp(rhs,rhs,rhs);
    MCoord res = *this + tmp;
    return res;
}

MCoord MCoord::operator% (const Coordinate rhs) const
{
    MCoord tmp(rhs,rhs,rhs);
    MCoord res = *this % tmp;
    return res;
}

MCoord MCoord::operator- (const Coordinate rhs) const
{
    MCoord tmp(rhs, rhs, rhs);
    MCoord res = *this - tmp;
    return res;
}

bool MCoord::operator== (const MCoord& rhs) const
{
    if (m_dims != rhs.m_dims) return false;
    bool res = true;
    for (size_t i = 0; (i < MCoord::m_defDims) && res; ++i)
        res = res && (rhs.coord(i) == this->coord(i));
    return res;
}

bool MCoord::operator< (const MCoord& rhs) const
{
    bool res = false;
    for (size_t i = 0; i < MCoord::m_defDims; ++i) {
        if (this->coord(i) < rhs.coord(i)) {
            // less case - return true
            res = true;
            break;
        }
        if (this->coord(i) == rhs.coord(i))
            // equal case - continue check
            continue;
        // greater case - return false
        break;
    }
    return res;
}

bool MCoord::operator<= (const MCoord& rhs) const
{
    bool res = (*this < rhs || *this == rhs);
    return res;
}

bool MCoord::operator> (const MCoord& rhs) const
{
    bool res = !(*this <= rhs);
    return res;
}

bool MCoord::operator>= (const MCoord& rhs) const
{
    bool res = !(*this < rhs);
    return res;
}

bool MCoord::operator!= (const MCoord& rhs) const
{
    bool res = !(*this == rhs);
    return res;
}

MCoord& MCoord::operator= (const MCoord& rhs)
{
    if (this != &rhs) {
        for (size_t i = 0; i < MCoord::m_defDims; ++i) {
            this->mCV[i] = rhs.mCV[i];
        }
    }
    return *this;
}

size_t MCoord::m_defDims = 3;

int MCoord::instances = 0;

QMutex MCoord::instanceLock;

std::ostream& operator <<(std::ostream& stream, const MCoord& c)
{
    //stream << "(";
    stream << c.coord(0);
    for (size_t d = 1; d < MCoord::m_defDims; ++d) {
        stream << "\t" << c.coord(d);
    }
    //stream << ")";
    return stream;
}
