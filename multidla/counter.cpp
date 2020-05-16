#include "counter.h"

namespace multidla {
Counter::Counter(MCoord sz, MCoord nul_pnt)
    : m_size(sz),
      m_null(nul_pnt),
      m_current(MCoord()),
      m_isNext(true)
{
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
    for (int32_t d = int32_t(m_current.defDims()) - 1; d >= 0; --d) {
        m_current.setCoord(size_t(d), m_current.coord(size_t(d)) + 1);
        if (m_current.coord(size_t(d)) == m_size.coord(size_t(d))) {
            m_current.setCoord(size_t(d), 0);
        } else {
            m_isNext = true;
            break;
        }
    }
}
} // multidla
