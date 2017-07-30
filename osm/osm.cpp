#include "osm.h"

#include <iostream>
#include <string>
#include <vector>

OSM::OSM(QObject* parent)
    : Generator(parent)
{
}

OSM::~OSM()
{
    delete fld;
}

OField* OSM::GetField() const
{
    return fld;
}

double OSM::SurfaceArea(double density) const
{
    double result = 0.0;
    if (this->finished) {
        // calc
        double volume = 0.0;
        double square = 0.0;
        for (const ocell& vc : fld->getClusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.getFigure()->getRadius());
                square += SfromR(cell.getFigure()->getRadius());
            }
            volume -= fld->overlapVolume(vc);
        }
        // -> Monte-Carlo
        int stepMax = 5000;
        int positive = fld->MonteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void OSM::Density(double density, double& denAero, double& porosity) const
{
    if (finished) {
        // calc
        double volume = 0.0;
        int sx = fld->getSizes().x;
        int sy = fld->getSizes().y;
        int sz = fld->getSizes().z;
        for (const ocell& vc : fld->getClusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.getFigure()->getRadius());
            }
            volume -= fld->overlapVolume(vc);
        }
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void OSM::Generate(const Sizes& sizes, double por, int initial, int step, int hit,
                   size_t cluster, double cellsize)
{
    finished = false;
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
    if (calculated) {
        // clean up
        if (fld != nullptr) {
            delete fld;
        }
        fld = nullptr;
        calculated = false;
    }
    
    fld = new OField(sizes);
    calculated = true;
    
    // part 1
    std::cout << "start init field!" << std::endl;
    
    double Emin = 0.3;

    fld->Initialize(Emin, cellsize);
    QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "end init field!" << std::endl;
    
    // part 2
    bool success = true;
    
    double allVol = (1 - por) * (sizes.x * sizes.y * sizes.z);
    
    int iterstep = 100;
    std::vector<ocell> oldclusters = fld->getClusters();
    for (int t = 0; t < 10;) {
        success = true;
        if (cancel) {
            success = false;
            break;
        }
        std::vector<OCell> varcells;
        for (const ocell& vc : oldclusters) {
            for (const OCell& cell : vc) {
                varcells.push_back(cell);
            }
        }

        uint count = varcells.size();
        std::cout << "Current: " << ++t << std::endl;
        std::vector<Pare> pares = fld->AgregateList(varcells);
        
        int bad = 0;
        int Kmax = 200;
        std::vector<sPar> spars;
        for (uint i = 0; i < varcells.size(); ++i) {
            spars.push_back(sPar(i));
            varcells[i].mark = false;
        }
        for (const Pare& p : pares) {
            ++spars[p.a].count;
            ++spars[p.b].count;
        }
        double currVol = fld->getVolumeAG(varcells);
        double maxVol = currVol;
        int iter = 0;
        while (allVol < currVol) {
            if (cancel) {
                success = false;
                break;
            }
            uint idx = rand() % count;
            std::vector<Pare> prs;
            double deltaVol = VfromR(varcells[idx].getFigure()->getRadius());
            vui srs;
            for (const Pare& p : pares) {
                if (p.a == idx) {
                    srs.push_back(p.b);
                    deltaVol -= fld->overlapVolumeCells(varcells[p.a], varcells[p.b]);
                } else if (p.b == idx) {
                    srs.push_back(p.a);
                    deltaVol -= fld->overlapVolumeCells(varcells[p.a], varcells[p.b]);
                } else {
                    prs.push_back(p);
                }
            }
            
            bool checkSPAR = true;
            
            for (const uint& ui : srs) {
                if (spars[ui].count < 2) {
                    checkSPAR = false;
                }
            }
            if (checkSPAR) {
                if (prs.size() < pares.size()) {
                    if (!varcells[idx].mark) {
                        std::vector<vui> agregate;

                        for (const Pare& p : prs) {
                            OField::inPareList(agregate, p);
                        }

                        if (agregate.size() > cluster) {
                            ++bad;
                        } else {
                            bad = 0;
                            varcells[idx].mark = true;
                            pares.clear();
                            for (const Pare& p : prs) {
                                pares.push_back(p);
                            }
                            
                            for (uint& ui : srs) {
                                --spars[ui].count;
                            }
                            currVol -= deltaVol;
                            QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                                Q_ARG(int, std::min(100, int(100 * (1 - (currVol - allVol) / (maxVol - allVol))))));
                            ++iter;
                            if (iter % iterstep == 0) {
                                iter = 0;
                                fld->setClusters(varcells);
                                ReBuild(count, pares, spars, varcells);
                                QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
                            }
                        }
                    } else {
                        //++bad;
                    }
                } else {
                    //++bad;
                }
            } else {
                //++bad;
            }

            if (bad >= Kmax) {
                std::cout << "Too many bad attempts in remove clusters!" << std::endl;
                success = false;
                break;
            }
        }
        
        if (success) {
            fld->setClusters(varcells);
            fld->Agregate();
            break;
        } else {
            fld->restoreClusters(oldclusters);
        }
    }
    
    if (cancel) {
        QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
        std::cout << "Canceled!" << std::endl;
        cancel = false;
        return;
    }
    
    if (!success) {
        std::cout << "Not success!" << std::endl;
        return;
    }
    
    finished = true;
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 100));
    QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
}

void OSM::Save(const char* fileName, txt_format format) const
{
    fld->toFile(fileName, format);
}

void OSM::Load(const char* fileName, txt_format format)
{
    if (fld != nullptr) {
        delete fld;
    }
    fld = new OField(fileName, format);
    finished = true;
}

void OSM::ReBuild(uint& count, std::vector<Pare>& pares,
                  std::vector<sPar>& spars, std::vector<OCell>& varcells)
{
    // rebuild varcells
    for (uint i = 0; i < varcells.size(); ) {
        if (varcells[i].mark) {
            varcells.erase(varcells.begin() + i);
        } else {
            ++i;
        }
    }
    
    count = varcells.size();
    pares = fld->AgregateList(varcells);
    
    spars.clear();
    for (uint i = 0; i < varcells.size(); ++i) {
        spars.push_back(sPar(i));
    }
    for (const Pare& p : pares) {
        ++spars[p.a].count;
        ++spars[p.b].count;
    }
}
