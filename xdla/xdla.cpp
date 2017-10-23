#include "xdla.h"

#include <iostream>
#include <string>

void xDLA::generate(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                    uint32_t hit, uint32_t cluster, double cellsize)
{
    m_finished = false;
    QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
    if (m_calculated) {
        // clean up
        if (m_fld) {
            delete m_fld;
        }
        m_fld = nullptr;
        m_calculated = false;
    }

    m_fld = new xField(sizes);
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
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 100));
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusters_size + target_cluster_cnt)) / int(maxSize))));
            iterstep = 5 * pow(double(maxSize) / clusters_size, 0.25);
        }
    }

    if (m_cancel) {
        QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
        std::cout << "Canceled!" << std::endl;
        m_cancel = false;
        return;
    }

    m_finished = true;
    QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 100));
    QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
}

double xDLA::surfaceArea(double density) const
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
            volume -= fld->overlapVolume(vc);
        }*/
        // -> Monte-Carlo
        uint32_t stepMax = 5000;
        uint32_t positive = m_fld->monteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void xDLA::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        /*for (const ocell& vc : fld->getClusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.getRadius());
            }
            volume -= fld->overlapVolume(vc);
        }*/
        double aeroVolume = volume / m_fld->sizes().volume();
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
}
