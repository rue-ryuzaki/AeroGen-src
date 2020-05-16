#ifndef AEROGEN_MULTIDLA_COUNTER_H
#define AEROGEN_MULTIDLA_COUNTER_H

#include "coord.h"

namespace multidla {
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
} // multidla

#endif // AEROGEN_MULTIDLA_COUNTER_H
