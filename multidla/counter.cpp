#include "counter.h"

Counter::Counter(MCoord sz, MCoord nul_pnt)
    : m_size(sz),
      m_null(nul_pnt)
{
    m_isNext = true;
    m_current = MCoord();
}

MCoord Counter::current() const
{
    return m_current + m_null;
}

bool Counter::isNext() const
{
    return m_isNext;
}

void Counter::next()
{
    m_isNext = false;
    for (int d = m_current.defDims() - 1; d >= 0; --d) {
        m_current.setCoord(d, m_current.coord(d) + 1);
        if (m_current.coord(d) == m_size.coord(d)) {
            m_current.setCoord(d, 0);
        } else {
            m_isNext = true;
            break;
        }
    }
}
