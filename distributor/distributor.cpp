#include "distributor.h"

#include <iostream>

Distributor::Distributor(QObject* parent)
    : m_mainwindow(parent),
      m_distr(),
      m_cancel(false)
{
}

void Distributor::calculate(Field* fld, double d, double dFrom, double dTo, double dStep)
{
    m_cancel = false;
#ifndef _WIN32
    uint32_t t0 = clock();
#endif
    DevField* dFld = DevField::loadFromField(fld, d);
    m_distr = distribution(dFld, dFrom, dTo, dStep);
#ifndef _WIN32
    std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    //printDistribution(distr);
    delete dFld;
    dFld = 0;
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

void Distributor::printDistribution(const std::vector<distrib>& distr) const
{
    for (const distrib& d : distr) {
        std::cout << d.r << " " << d.vol << std::endl;
    }
}

std::vector<distrib> Distributor::distribution(DevField* dFld, double dFrom,
                                                  double dTo, double dStep) const
{
    std::vector<distrib> result;
    for (double d = dFrom; d <= dTo; d+= dStep) {
        if (m_cancel) {
            break;
        }
        double r = 0.5 * d;
        result.push_back(distrib(r, dFld->volume(r)));
        std::cout << d << " " << result.back().vol << std::endl;
    }
    return result;
}
