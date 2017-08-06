#ifndef MULTIDLA_COUNTER_H
#define	MULTIDLA_COUNTER_H

#include "mcoord.h"

class Counter
{
public:
    Counter(MCoord sz, MCoord = MCoord());
    MCoord  current() const;
    bool    isNext()  const;
    void    next();
    
private:
    MCoord  m_size;
    MCoord  m_null;
    MCoord  m_current;
    bool    m_isNext;
};

#endif	// MULTIDLA_COUNTER_H

