#ifndef AEROGEN_MULTIDLA_COORD_H
#define AEROGEN_MULTIDLA_COORD_H

#include <cstdint>
#include <exception>
#include <iostream>
#include <mutex>
#include <vector>

#include "sortedvector.h"

typedef int32_t Coordinate;
#define MMAX_DIMS 3

namespace multidla {
class MOutOfBoundError: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Coordinate index out of bounds";
    }
};

//typedef Coordinate CoordinateVector[MAX_DIMS];

class MCoord
{
public:
    MCoord(Coordinate*);
    MCoord(const MCoord&);
    MCoord(Coordinate = 0, Coordinate = 0, Coordinate = 0);
    MCoord(const std::vector<Coordinate>&);

    virtual ~MCoord();

    Coordinate coord(size_t) const;
    void setCoord(size_t, Coordinate);
    void setPosition(Coordinate, Coordinate, Coordinate = 0);

    MCoord operator+ (const MCoord&) const;
    MCoord operator% (const MCoord&) const;
    MCoord operator- (const MCoord&) const;

    MCoord operator+ (const Coordinate) const;
    MCoord operator% (const Coordinate) const;
    MCoord operator- (const Coordinate) const;
    MCoord operator/ (const Coordinate) const;

    bool operator== (const MCoord&) const;
    bool operator<  (const MCoord&) const;
    bool operator<= (const MCoord&) const;
    bool operator>  (const MCoord&) const;
    bool operator>= (const MCoord&) const;
    bool operator!= (const MCoord&) const;

    MCoord& operator= (const MCoord& rhs);

    static size_t defDims() { return m_defDims; }
    static void setDefDims(size_t dims)
    {
        if (m_instances != 0) {
            return;
        }
        m_instanceLock.lock();
        if (2 <= dims && dims <= 3) {
            m_defDims = dims;
        }
        m_instanceLock.unlock();
    }

    friend std::ostream& operator <<(std::ostream& stream, const MCoord& temp);
    
private:
    bool checkBounds(size_t) const;

    Coordinate  m_CV[MMAX_DIMS];
    size_t      m_dims;

    static size_t   m_defDims;
    static int32_t  m_instances;
    static std::mutex m_instanceLock;
};

typedef std::vector<MCoord> MCoordVec;
typedef sorted_vector<MCoord> SortedCoordVec;
} // multidla

#endif // AEROGEN_MULTIDLA_COORD_H
