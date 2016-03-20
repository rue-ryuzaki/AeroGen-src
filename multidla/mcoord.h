#ifndef MULTIDLA_COORD_H
#define	MULTIDLA_COORD_H

#include <vector>
#include <exception>
#include "sortedvector.h"
#include <QMutex>

using namespace std;

typedef int Coordinate;
#define MMAX_DIMS 3

class MOutOfBoundError: public exception {
    virtual const char* what() const throw() {
        return "Coordinate index out of bounds";
    }
};

//typedef Coordinate CoordinateVector[MAX_DIMS];

class MCoord {
public:
    MCoord(Coordinate *);
    MCoord(const MCoord &);
    MCoord(Coordinate = 0, Coordinate = 0, Coordinate = 0);
    MCoord(const vector<Coordinate> &);

    virtual ~MCoord();

    Coordinate GetCoord(size_t) const;
    void SetCoord(size_t, Coordinate);
    void SetPosition(Coordinate, Coordinate, Coordinate = 0);

    MCoord operator+ (const MCoord &) const;
    MCoord operator% (const MCoord &) const;
    MCoord operator- (const MCoord &) const;

    MCoord operator+ (const Coordinate) const;
    MCoord operator% (const Coordinate) const;
    MCoord operator- (const Coordinate) const;
    MCoord operator/ (const Coordinate) const;

    bool operator== (const MCoord &) const;
    bool operator<  (const MCoord &) const;
    bool operator<= (const MCoord &) const;
    bool operator>  (const MCoord &) const;
    bool operator>= (const MCoord &) const;
    bool operator!= (const MCoord &) const;

    MCoord & operator= (const MCoord & rhs);

    static size_t GetDefDims() { return mDefDims; }
    static void SetDefDims(size_t dims) {
        instanceLock.lock();
        if (instances != 0) {
            instanceLock.unlock();
            return;
        }
        if (2 <= dims || dims <= 3) {
            mDefDims = dims;
        }
        instanceLock.unlock();
    }

    friend ostream& operator <<(ostream& stream, const MCoord& temp);
    
private:
    Coordinate mCV[MMAX_DIMS];
    size_t mDims;

    bool CheckBounds(size_t) const;

    static size_t mDefDims;
    static int instances;
    static QMutex instanceLock;
};

typedef vector<MCoord> MCoordVec;
typedef sorted_vector<MCoord> SortedCoordVec;

#endif	/* MULTIDLA_COORD_H */
