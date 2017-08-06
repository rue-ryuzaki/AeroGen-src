#include "dlca.h"

#include <iostream>
#include <string>

DLCA::DLCA(QObject* parent) : Generator(parent)
{
}

DLCA::~DLCA()
{
    delete m_fld;
}

CField* DLCA::field() const
{
    return m_fld;
}

double DLCA::surfaceArea(double density) const
{
    double result = 0.0;
    if (this->m_finished) {
        // calc
        double volume = 0.0;
        double square = 0.0;
        for (const vcell& vc : m_fld->clusters()) {
            for (const CCell& cell : vc) {
                volume += cell.figure()->volume();
                square += cell.figure()->area();
            }
        }
        volume -= m_fld->overlapVolume();
        // -> Monte-Carlo
        int stepMax = 5000;
        int positive = m_fld->monteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void DLCA::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        int sx = m_fld->sizes().x;
        int sy = m_fld->sizes().y;
        int sz = m_fld->sizes().z;
        for (const vcell& vc : m_fld->clusters()) {
            for (const CCell& cell : vc) {
                volume += cell.figure()->volume();
            }
        }
        volume -= m_fld->overlapVolume();
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void DLCA::generate(const Sizes& sizes, double por, int initial, int step, int hit,
                    size_t cluster, double cellsize)
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
    
    int iter = 0;
    int iterstep = 5;
    int maxSize = 0;
    {
        m_fld->agregate();
        std::vector<vcell> clusters = m_fld->clusters();
        maxSize = clusters.size();
        if (maxSize < 1) maxSize = 1;
    }
    
    // agregation
    while (true) {
        if (m_cancel) {
            break;
        }
        // move cells
        m_fld->move();
        m_fld->agregate();
        uint clusters_size = m_fld->clusters().size();
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
                Q_ARG(int, std::min(100, int(100 * (maxSize - clusters_size + target_cluster_cnt)) / maxSize)));
            iterstep = 5 * pow (double(maxSize) / clusters_size, 0.25);
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

