#include "mxdla.h"

#include <iostream>
#include <string>

void MxDLA::generate(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                     uint32_t hit, uint32_t cluster, double cellsize)
{
    if (por <= 0 || por >= 1) {
        std::cout << "Wrong porosity!" << std::endl;
        return;
    }

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

#ifndef _WIN32
    uint32_t t0 = uint32_t(clock());
#endif
    m_fld = new MxField(sizes);
    m_calculated = true;
    std::cout << "start init field!" << std::endl;
    // init field
    m_fld->initialize(por, cellsize);
    m_fld->initDla(por, initial, step, hit);
    std::cout << "end init field!" << std::endl;

    uint32_t iter = 0;
    uint32_t iterstep = 10;
    uint32_t maxSize = 1;

//    uint32_t maxCount = uint32_t(sizes.volume() * (1.0 - por));

//    int birthR = int(fside * 0.45 + 0.001);
//    int deathR = int(fside * 0.49 + 0.001);

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

        if (clusters_size <= cluster) {
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusters_size + cluster)) / int(maxSize))));
            iterstep = uint32_t(5.0 * pow(double(maxSize) / clusters_size, 0.25));
        }
    }

#ifndef _WIN32
    std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
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

double MxDLA::surfaceArea(double density) const
{
    double result = 0.0;
    if (this->m_finished) {
        // calc
        double volume = 0.0;
        double square = 0.0;
        const std::vector<Cell> cells = m_fld->cells();
        for (const Cell& cell : cells) {
            volume += cell.figure()->volume();
            square += cell.figure()->area();
        }
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
        const std::vector<Cell> cells = m_fld->cells();
        for (const Cell& cell : cells) {
            volume += cell.figure()->volume();
        }
        double aeroVolume = volume / m_fld->sizes().volume();
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
}
