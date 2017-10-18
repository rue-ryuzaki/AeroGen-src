#ifndef MULTIDLA_COORD_H
#define	MULTIDLA_COORD_H

#include <cstdint>
#include <exception>
#include <iostream>
#include <vector>
#include <QMutex>

#include "sortedvector.h"

typedef int16_t Coordinate;
#define MMAX_DIMS 3

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
        m_instanceLock.lock();
        if (m_instances != 0) {
            m_instanceLock.unlock();
            return;
        }
        if (2 <= dims || dims <= 3) {
            m_defDims = dims;
        }
        m_instanceLock.unlock();
    }

    friend std::ostream& operator <<(std::ostream& stream, const MCoord& temp);
    
private:
    bool checkBounds(size_t) const;

    Coordinate m_CV[MMAX_DIMS];
    size_t m_dims;

    static size_t   m_defDims;
    static int32_t  m_instances;
    static QMutex   m_instanceLock;
};

typedef std::vector<MCoord> MCoordVec;
typedef sorted_vector<MCoord> SortedCoordVec;

#endif	// MULTIDLA_COORD_H
