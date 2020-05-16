#include "xdla.h"

#include <iostream>
#include <string>

xDLA::xDLA(QObject *parent)
    : Generator(parent)
{
}

xDLA::~xDLA()
{
    if (m_fld) {
        delete m_fld;
        m_fld = nullptr;
    }
}

xField* xDLA::field() const
{
    return m_fld;
}

void xDLA::generate(const Sizes& sizes, const RunParams& params)
{
    m_finished = false;
    QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
    // clean up
    if (m_fld) {
        delete m_fld;
    }
#ifndef _WIN32
    uint32_t t0 = uint32_t(clock());
#endif
    m_fld = new xField(sizes, params.isToroid);
    std::cout << "start init field!" << std::endl;
    // init field
    m_fld->initialize(params.porosity, params.cellSize);
    std::cout << "end init field!" << std::endl;

    uint32_t iter = 0;
    uint32_t iterstep = 10;
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

        if (clusters_size <= params.cluster) {
            break;
        }

        ++iter;
        if (iter % iterstep == 0) {
            iter = 0;
            QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
            QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusters_size + params.cluster)) / int(maxSize))));
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

double xDLA::surfaceArea(double density, uint32_t steps) const
{
    double result = 0.0;
    if (this->m_finished) {
#ifndef _WIN32
        uint32_t t0 = uint32_t(clock());
#endif
        // calc
        double volume = 0.0;
        double square = 0.0;
        /*for (const auto& vc : fld->getClusters()) {
            for (const auto& cell : vc) {
                volume += VfromR(cell.getRadius());
                square += SfromR(cell.getRadius());
            }
            volume -= fld->overlapVolume(vc);
        }*/
        // -> Monte-Carlo
        uint32_t positive = m_fld->monteCarlo(steps);
        result = 1000000 * square * positive / (steps * density * volume);
#ifndef _WIN32
        std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    }
    return result;
}

void xDLA::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        /*for (const auto& vc : fld->getClusters()) {
            for (const auto& cell : vc) {
                volume += VfromR(cell.getRadius());
            }
            volume -= fld->overlapVolume(vc);
        }*/
        double aeroVolume = volume / m_fld->sizes().volume();
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void xDLA::save(const char* fileName, txt_format format) const
{
    m_fld->toFile(fileName, format);
}

void xDLA::load(const char* fileName, txt_format format)
{
    if (m_fld) {
        delete m_fld;
    }
    m_fld = new xField(fileName, format);
    m_finished = true;
}
