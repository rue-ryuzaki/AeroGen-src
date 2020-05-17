#include "dlca.h"

#include <iostream>

DLCA::DLCA(QObject* parent)
    : Generator(parent)
{
}

DLCA::~DLCA()
{
    if (m_fld) {
        delete m_fld;
        m_fld = nullptr;
    }
}

dlca::XField* DLCA::field() const
{
    return m_fld;
}

void DLCA::generate(const Sizes& sizes, const RunParams& params)
{
    m_finished = false;
    QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
    // clean up
    if (m_fld) {
        delete m_fld;
    }
    m_fld = new dlca::XField(sizes, params. isToroid);
    std::cout << "start init field!" << std::endl;
    // init field
    m_fld->initialize(params.porosity, params.cellSize);
    std::cout << "end init field!" << std::endl;

    uint32_t iter = 0;
    uint32_t iterstep = 10;
    m_fld->agregate();
    size_t maxSize = m_fld->clusters().size();
    if (maxSize < 1) {
        maxSize = 1;
    }
    double speed = m_fld->clusters().front().front().vector().length();
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
        size_t clusterSize = m_fld->clusters().size();
        //std::vector<vcell> clusters = fld->getClusters();

        //check!
        std::cout << "New iter. Clusters: " << clusterSize << std::endl;

        if (clusterSize <= params.cluster) {
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusterSize + params.cluster)) / int(maxSize))));
            iterstep = uint32_t(5.0 * pow(double(maxSize) / double(clusterSize), 0.25));
        }
    }std::cout << speed << " > " << m_fld->clusters().front().front().vector().length() << std::endl;

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

double DLCA::surfaceArea(double density, uint32_t steps) const
{
    double result = 0.0;
    if (this->m_finished) {
#ifndef _WIN32
        uint32_t t0 = uint32_t(clock());
#endif
        // calc
        double volume = 0.0;
        double square = 0.0;
        for (const auto& vc : m_fld->clusters()) {
            for (const auto& cell : vc) {
                volume += cell.figure()->volume();
                square += cell.figure()->area();
            }
        }
        volume -= m_fld->overlapVolume();
        // -> Monte-Carlo
        uint32_t positive = m_fld->monteCarlo(steps);
        result = 1000000 * square * positive / (steps * density * volume);
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
        for (const auto& vc : m_fld->clusters()) {
            for (const auto& cell : vc) {
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
    m_fld = new dlca::XField(fileName, format);
    m_finished = true;
}

