#ifndef MULTIDLA_COUNTER_H
#define	MULTIDLA_COUNTER_H

#include "mcoord.h"

class Counter
{
public:
    Counter(MCoord sz, MCoord = MCoord());
    MCoord  Current() const;
    bool    IsNext()  const;
    void    Next();
    
private:
    MCoord  mSize;
    MCoord  mNull;
    MCoord  mCurrent;
    bool    mIsNext;
};

#endif	/* MULTIDLA_COUNTER_H */

