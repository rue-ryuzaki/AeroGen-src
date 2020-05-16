#include "mxdla.h"

#include <iostream>

MxDLA::MxDLA(QObject *parent)
    : Generator(parent)
{
}

MxDLA::~MxDLA()
{
    if (m_fld) {
        delete m_fld;
        m_fld = nullptr;
    }
}

MxField* MxDLA::field() const
{
    return m_fld;
}

void MxDLA::generate(const Sizes& sizes, const RunParams& params)
{
    m_finished = false;
    QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
    if (params.porosity <= 0.0 || params.porosity >= 1.0) {
        std::cout << "Wrong porosity!" << std::endl;
        return;
    }
    // clean up
    if (m_fld) {
        delete m_fld;
    }
#ifndef _WIN32
    uint32_t t0 = uint32_t(clock());
#endif
    m_fld = new MxField(sizes);
    std::cout << "start init field!" << std::endl;
    // init field
    m_fld->initialize(params.porosity, params.cellSize);
    m_fld->initDla(params.porosity, params.init, params.step, params.hit);
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

double MxDLA::surfaceArea(double density, uint32_t steps) const
{
    double result = 0.0;
    if (this->m_finished) {
#ifndef _WIN32
        uint32_t t0 = uint32_t(clock());
#endif
        // calc
        double volume = 0.0;
        double square = 0.0;
        const auto cells = m_fld->cells();
        for (const auto& cell : cells) {
            volume += cell.figure()->volume();
            square += cell.figure()->area();
        }
        // -> Monte-Carlo
        uint32_t positive = m_fld->monteCarlo(steps);
        result = 1000000 * square * positive / (steps * density * volume);
#ifndef _WIN32
        std::cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек." << std::endl;
#endif
    }
    return result;
}

void MxDLA::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        const auto cells = m_fld->cells();
        for (const auto& cell : cells) {
            volume += cell.figure()->volume();
        }
//        Sizes xx = m_fld->sizes();
//        uint32_t xxa = xx.volume();
        double aeroVolume = volume / m_fld->sizes().volume();
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void MxDLA::save(const char* fileName, txt_format format) const
{
    m_fld->toFile(fileName, format);
}

void MxDLA::load(const char* fileName, txt_format format)
{
    if (m_fld) {
        delete m_fld;
    }
    m_fld = new MxField(fileName, format);
    m_finished = true;
}
