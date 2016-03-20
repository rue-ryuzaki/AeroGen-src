#include "distributor.h"
#include <math.h>

void Distributor::Calculation(Field * fld, double d, double dFrom, double dTo, double dStep) {
    cancel = false;
#ifndef _WIN32
    unsigned t0 = clock();
#endif
    DevField * dFld = DevField::LoadFromField(fld, d);
    distr = getDistribution(dFld, dFrom, dTo, dStep);
#ifndef _WIN32
    cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек.\n";
#endif
    //printDistribution(distr);
    delete dFld;
    dFld = 0;
    QMetaObject::invokeMethod(mainwindow, "closeWaitDialog", Qt::QueuedConnection);
    if (cancel) {
        cout << "Canceled!\n";
        cancel = false;
        return;
    }
    QMetaObject::invokeMethod(mainwindow, "distrFinished", Qt::QueuedConnection);
}

void Distributor::printDistribution(const vector<distrib> & distr) {
    for (const distrib & d : distr) {
        cout << d.r << " " << d.vol << "\n";
    }
}

vector<distrib> Distributor::getDistribution(DevField* dFld, double dFrom, double dTo, double dStep) {
    vector<distrib> result;
    for (double d = dFrom; d <= dTo; d+= dStep) {
        if (cancel) {
            break;
        }
        double r = 0.5 * d;
        result.push_back(distrib(r, dFld->getVolume(r)));
        cout << d << " " << result.back().vol << "\n";
    }
    return result;
}
