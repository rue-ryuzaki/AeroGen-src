#include "distributor.h"

#include <iostream>

Distributor::Distributor(QObject* parent)
    : m_mainwindow(parent),
      m_distr(),
      m_cancel(false)
{
}

void Distributor::calculate(Field* fld, double d, double from, double to, double step, bool isToroid)
{
    m_cancel = false;
#ifndef _WIN32
    uint32_t t0 = uint32_t(clock());
#endif
    DevField* dFld = DevField::loadFromField(fld, d, isToroid);
    calcDistr(dFld, from, to, step);
#ifndef _WIN32
    std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    delete dFld;
    dFld = nullptr;
    QMetaObject::invokeMethod(m_mainwindow, "closeWaitDialog", Qt::QueuedConnection);
    if (m_cancel) {
        std::cout << "Canceled!" << std::endl;
        m_cancel = false;
        return;
    }
    QMetaObject::invokeMethod(m_mainwindow, "distrFinished", Qt::QueuedConnection);
}

void Distributor::cancel()
{
    m_cancel = true;
}

const std::vector<distrib>& Distributor::distribution() const
{
    return m_distr;
}

void Distributor::calcDistr(DevField* fld, double from, double to, double step)
{
    m_distr.clear();
    for (double d = from; d <= to; d+= step) {
        if (m_cancel) {
            break;
        }
        double r = 0.5 * d;
        m_distr.push_back(distrib(r, fld->volume(r)));
        std::cout << d << " " << m_distr.back().vol << std::endl;
    }
}
