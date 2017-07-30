#include "distributor.h"

#include <iostream>
#include <math.h>
#include <vector>

Distributor::Distributor(QObject* parent)
    : mainwindow(parent)
{
}

Distributor::~Distributor()
{
}

void Distributor::Calculation(Field* fld, double d, double dFrom, double dTo, double dStep)
{
    cancel = false;
#ifndef _WIN32
    unsigned t0 = clock();
#endif
    DevField* dFld = DevField::LoadFromField(fld, d);
    distr = getDistribution(dFld, dFrom, dTo, dStep);
#ifndef _WIN32
    std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    //printDistribution(distr);
    delete dFld;
    dFld = 0;
    QMetaObject::invokeMethod(mainwindow, "closeWaitDialog", Qt::QueuedConnection);
    if (cancel) {
        std::cout << "Canceled!" << std::endl;
        cancel = false;
        return;
    }
    QMetaObject::invokeMethod(mainwindow, "distrFinished", Qt::QueuedConnection);
}

void Distributor::Cancel()
{
    cancel = true;
}

std::vector<distrib> Distributor::getDistr() const
{
    return distr;
}

void Distributor::printDistribution(const std::vector<distrib>& distr) const
{
    for (const distrib& d : distr) {
        std::cout << d.r << " " << d.vol << std::endl;
    }
}

std::vector<distrib> Distributor::getDistribution(DevField* dFld, double dFrom, double dTo, double dStep) const
{
    std::vector<distrib> result;
    for (double d = dFrom; d <= dTo; d+= dStep) {
        if (cancel) {
            break;
        }
        double r = 0.5 * d;
        result.push_back(distrib(r, dFld->getVolume(r)));
        std::cout << d << " " << result.back().vol << std::endl;
    }
    return result;
}
