#include "dlca.h"

#include <iostream>
#include <string>

DLCA::DLCA(QObject* parent) : Generator(parent)
{
}

DLCA::~DLCA()
{
    delete fld;
}

CField* DLCA::GetField() const
{
    return fld;
}

double DLCA::SurfaceArea(double density) const
{
    double result = 0.0;
    if (this->finished) {
        // calc
        double volume = 0.0;
        double square = 0.0;
        for (const vcell& vc : fld->getClusters()) {
            for (const CCell& cell : vc) {
                volume += cell.getFigure()->getVolume();
                square += cell.getFigure()->getArea();
            }
        }
        volume -= fld->overlapVolume();
        // -> Monte-Carlo
        int stepMax = 5000;
        int positive = fld->MonteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void DLCA::Density(double density, double& denAero, double& porosity) const
{
    if (finished) {
        // calc
        double volume = 0.0;
        int sx = fld->getSizes().x;
        int sy = fld->getSizes().y;
        int sz = fld->getSizes().z;
        for (const vcell& vc : fld->getClusters()) {
            for (const CCell& cell : vc) {
                volume += cell.getFigure()->getVolume();
            }
        }
        volume -= fld->overlapVolume();
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void DLCA::Generate(const Sizes& sizes, double por, int initial, int step, int hit,
                    size_t cluster, double cellsize)
{
    finished = false;
    
    if (calculated) {
        // clean up
        if (fld != nullptr) {
            delete fld;
        }
        fld = nullptr;
        calculated = false;
    }
    
    fld = new CField(sizes);
    calculated = true;
    std::cout << "start init field!" << std::endl;
    // init field
    fld->Initialize(por, cellsize);
    std::cout << "end init field!" << std::endl;

    size_t target_cluster_cnt = cluster;
    
    int iter = 0;
    int iterstep = 5;
    int maxSize = 0;
    {
        fld->Agregate();
        std::vector<vcell> clusters = fld->getClusters();
        maxSize = clusters.size();
        if (maxSize < 1) maxSize = 1;
    }
    
    // agregation
    while (true) {
        if (cancel) {
            break;
        }
        // move cells
        fld->Move();
        fld->Agregate();
        uint clusters_size = fld->getClusters().size();
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
    
    if (cancel) {
        QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
        std::cout << "Canceled!" << std::endl;
        cancel = false;
        return;
    }
    
    finished = true;
    
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 100));
    QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
}

void DLCA::Save(const char* fileName, txt_format format) const
{
    fld->toFile(fileName, format);
}

void DLCA::Load(const char* fileName, txt_format format)
{
    if (fld != nullptr) {
        delete fld;
    }
    fld = new CField(fileName, format);
    finished = true;
}

