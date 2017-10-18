#include "mxdla.h"

#include <iostream>
#include <string>

void MxDLA::generate(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                     uint32_t hit, uint32_t cluster, double cellsize)
{
    m_finished = false;
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
    if (m_calculated) {
        // clean up
        if (m_fld) {
            delete m_fld;
        }
        m_fld = nullptr;
        m_calculated = false;
    }

    m_fld = new MxField(sizes);
    m_calculated = true;
    std::cout << "start init field!" << std::endl;
    // init field
    m_fld->initialize(por, cellsize);
    std::cout << "end init field!" << std::endl;

    size_t target_cluster_cnt = cluster;

    uint32_t iter = 0;
    uint32_t iterstep = 5;
    uint32_t maxSize = 1;

    // agregation
    while (true) {
        if (m_cancel) {
            break;
        }
        //
        uint32_t clusters_size = 0;
        //std::vector<vcell> clusters = fld->getClusters();

        //check!
        std::cout << "New iter. Clusters: " << clusters_size << std::endl;

        if (clusters_size <= target_cluster_cnt) {
            QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 100));
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
            QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusters_size + target_cluster_cnt)) / int(maxSize))));
            iterstep = 5 * pow(double(maxSize) / clusters_size, 0.25);
        }
    }

    if (m_cancel) {
        QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
        std::cout << "Canceled!" << std::endl;
        m_cancel = false;
        return;
    }

    m_finished = true;
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 100));
    QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
}

double MxDLA::surfaceArea(double density) const
{
    double result = 0.0;
    if (this->m_finished) {
        // calc
        double volume = 0.0;
        double square = 0.0;
        /*for (const ocell& vc : fld->getClusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.getRadius());
                square += SfromR(cell.getRadius());
            }
        }*/
        // -> Monte-Carlo
        uint32_t stepMax = 5000;
        uint32_t positive = m_fld->monteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void MxDLA::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        uint32_t sx = m_fld->sizes().x;
        uint32_t sy = m_fld->sizes().y;
        uint32_t sz = m_fld->sizes().z;
        /*for (const ocell& vc : fld->getClusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.getRadius());
            }
        }*/
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
}
