#include "dlca.h"

#include <iostream>
#include <string>

DLCA::DLCA(QObject* parent)
    : Generator(parent)
{
}

DLCA::~DLCA()
{
    delete m_fld;
    m_fld = nullptr;
}

CField* DLCA::field() const
{
    return m_fld;
}

void DLCA::generate(const Sizes& sizes, double por, uint32_t /*initial*/, uint32_t /*step*/,
                    uint32_t /*hit*/, uint32_t cluster, double cellsize)
{
    m_finished = false;

    if (m_calculated) {
        // clean up
        if (m_fld) {
            delete m_fld;
        }
        m_fld = nullptr;
        m_calculated = false;
    }

    m_fld = new CField(sizes);
    m_calculated = true;
    std::cout << "start init field!" << std::endl;
    // init field
    m_fld->initialize(por, cellsize);
    std::cout << "end init field!" << std::endl;

    size_t target_cluster_cnt = cluster;

    uint32_t iter = 0;
    uint32_t iterstep = 10;
    size_t maxSize = 0;
    {
        m_fld->agregate();
        std::vector<std::vector<CCell> > clusters = m_fld->clusters();
        maxSize = clusters.size();
        if (maxSize < 1) {
            maxSize = 1;
        }
    }

#ifndef _WIN32
    uint32_t t0 = uint32_t(clock());
#endif
    // agregation
    while (true) {
        if (m_cancel) {
            break;
        }
        // move cells
        m_fld->move();
        m_fld->agregate();
        size_t clusters_size = m_fld->clusters().size();
        //std::vector<vcell> clusters = fld->getClusters();

        //check!
        std::cout << "New iter. Clusters: " << clusters_size << std::endl;

        if (clusters_size <= target_cluster_cnt) {
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusters_size + target_cluster_cnt)) / int(maxSize))));
            iterstep = uint32_t(5.0 * pow(double(maxSize) / double(clusters_size), 0.25));
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

double DLCA::surfaceArea(double density) const
{
    double result = 0.0;
    if (this->m_finished) {
#ifndef _WIN32
        uint32_t t0 = uint32_t(clock());
#endif
        // calc
        double volume = 0.0;
        double square = 0.0;
        for (const std::vector<CCell>& vc : m_fld->clusters()) {
            for (const CCell& cell : vc) {
                volume += cell.figure()->volume();
                square += cell.figure()->area();
            }
        }
        volume -= m_fld->overlapVolume();
        // -> Monte-Carlo
        uint32_t stepMax = 5000;
        uint32_t positive = m_fld->monteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
#ifndef _WIN32
        std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    }
    return result;
}

void DLCA::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        for (const std::vector<CCell>& vc : m_fld->clusters()) {
            for (const CCell& cell : vc) {
                volume += cell.figure()->volume();
            }
        }
        volume -= m_fld->overlapVolume();
        double aeroVolume = volume / m_fld->sizes().volume();
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void DLCA::save(const char* fileName, txt_format format) const
{
    m_fld->toFile(fileName, format);
}

void DLCA::load(const char* fileName, txt_format format)
{
    if (m_fld) {
        delete m_fld;
    }
    m_fld = new CField(fileName, format);
    m_finished = true;
}

