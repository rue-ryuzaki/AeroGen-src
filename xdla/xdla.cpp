#include "xdla.h"

#include <iostream>
#include <string>

double xDLA::SurfaceArea(double density) const
{
    double result = 0.0;
    if (this->finished) {
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
        int stepMax = 5000;
        int positive = fld->MonteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void xDLA::Density(double density, double& denAero, double& porosity) const
{
    if (finished) {
        // calc
        double volume = 0.0;
        int sx = fld->getSizes().x;
        int sy = fld->getSizes().y;
        int sz = fld->getSizes().z;
        /*for (const ocell& vc : fld->getClusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.getRadius());
            }
            volume -= fld->overlapVolume(vc);
        }*/
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void xDLA::Generate(const Sizes& sizes, double por, int initial, int step, int hit,
                    size_t cluster, double cellsize)
{
    finished = false;
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
    if (calculated) {
        // clean up
        if (fld) {
            delete fld;
        }
        fld = nullptr;
        calculated = false;
    }
    
    fld = new xField(sizes);
    calculated = true;
    std::cout << "start init field!" << std::endl;
    // init field
    fld->Initialize(por, cellsize);
    std::cout << "end init field!" << std::endl;

    size_t target_cluster_cnt = cluster;
    
    int iter = 0;
    int iterstep = 5;
    int maxSize = 1;
    
    // agregation
    while (true) {
        if (cancel) {
            break;
        }
        //
        uint clusters_size = 0;
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

