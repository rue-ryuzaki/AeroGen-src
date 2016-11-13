#include "mcoord.h"

#include <iostream>

MCoord::MCoord(Coordinate X, Coordinate Y, Coordinate Z)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    mDims = MCoord::mDefDims;

    SetPosition(X,Y,Z);
}

MCoord::MCoord(Coordinate * cv)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    mDims = MCoord::mDefDims;

    for (size_t i=0; i < MCoord::mDefDims; ++i) {
        mCV[i] = cv[i];
    }
}

MCoord::MCoord(const std::vector<Coordinate> & cv)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    mDims = MCoord::mDefDims;

    for (size_t i=0; i < MCoord::mDefDims; ++i) {
        mCV[i] = cv[i];
    }
}

MCoord::MCoord(const MCoord & c)
{
    MCoord::instanceLock.lock();
    MCoord::instances += 1;
    MCoord::instanceLock.unlock();

    mDims = MCoord::mDefDims;

    //mDims = c.mDims;
    for (size_t i = 0; i < MCoord::mDefDims; ++i) {
        mCV[i] = c.GetCoord(i);
    }
}

MCoord::~MCoord()
{
    MCoord::instanceLock.lock();
    MCoord::instances -= 1;
    MCoord::instanceLock.unlock();
}

bool MCoord::CheckBounds(size_t index) const
{
    return (index < MCoord::mDefDims);
}

Coordinate MCoord::GetCoord(size_t num) const
{
    if (this->CheckBounds(num)) {
        return mCV[num];
    }
    throw MOutOfBoundError();
}

void MCoord::SetCoord(size_t num, Coordinate val)
{
    if (this->CheckBounds(num)) {
        this->mCV[num] = val;
        return;
    }
    throw MOutOfBoundError();
}

void MCoord::SetPosition(Coordinate X, Coordinate Y, Coordinate Z)
{
    mCV[0] = X;
    mCV[1] = Y;
    mCV[2] = Z;
}

MCoord MCoord::operator+ (const MCoord &rhs) const
{
    MCoord res;
    for (size_t i = 0; i < MCoord::mDefDims; ++i)
        res.SetCoord(i, this->GetCoord(i) + rhs.GetCoord(i));
    return res;
}

MCoord MCoord::operator% (const MCoord & rhs) const
{
    MCoord res;
    for (size_t i = 0; i < MCoord::mDefDims; ++i)
        res.SetCoord(i, this->GetCoord(i) % ((rhs.GetCoord(i) != 0) ? rhs.GetCoord(i) : 1));
    return res;
}

MCoord MCoord::operator- (const MCoord & rhs) const
{
    MCoord res;
    for (size_t i = 0; i < MCoord::mDefDims; ++i)
        res.SetCoord(i, this->GetCoord(i) - rhs.GetCoord(i));
    return res;
}

MCoord MCoord::operator/ (const Coordinate divide) const
{
    MCoord res(*this);

    for (size_t i = 0; i < MCoord::mDefDims; ++i)
        res.SetCoord(i, this->GetCoord(i) / divide);

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

bool MCoord::operator== (const MCoord & rhs) const
{
    if (mDims != rhs.mDims) return false;
    bool res = true;
    for (size_t i = 0; (i < MCoord::mDefDims) && res; ++i)
        res = res && (rhs.GetCoord(i) == this->GetCoord(i));
    return res;
}

bool MCoord::operator< (const MCoord & rhs) const
{
    bool res = false;
    for (size_t i = 0; i < MCoord::mDefDims; ++i) {
        if (this->GetCoord(i) < rhs.GetCoord(i)) {
            // less case - return true
            res = true;
            break;
        }
        if (this->GetCoord(i) == rhs.GetCoord(i))
            // equal case - continue check
            continue;
        // greater case - return false
        break;
    }
    return res;
}

bool MCoord::operator<= (const MCoord & rhs) const
{
    bool res = (*this < rhs || *this == rhs);
    return res;
}

bool MCoord::operator> (const MCoord & rhs) const
{
    bool res = !(*this <= rhs);
    return res;
}

bool MCoord::operator>= (const MCoord & rhs) const
{
    bool res = !(*this < rhs);
    return res;
}

bool MCoord::operator!= (const MCoord & rhs) const
{
    bool res = !(*this == rhs);
    return res;
}

MCoord & MCoord::operator= (const MCoord & rhs)
{
    if (this != &rhs) {
        for (size_t i = 0; i < MCoord::mDefDims; ++i) {
            this->mCV[i] = rhs.mCV[i];
        }
    }
    return *this;
}

size_t MCoord::mDefDims = 3;

int MCoord::instances = 0;

QMutex MCoord::instanceLock;

std::ostream& operator <<(std::ostream& stream, const MCoord& c)
{
    //stream << "(";
    stream << c.GetCoord(0);
    for (size_t d = 1; d < MCoord::mDefDims; ++d) {
        stream << "\t" << c.GetCoord(d);
    }
    //stream << ")";
    return stream;
}
