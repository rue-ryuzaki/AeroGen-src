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
    delete m_fld;
}

OField* OSM::field() const
{
    return m_fld;
}

double OSM::surfaceArea(double density) const
{
    double result = 0.0;
    if (this->m_finished) {
        // calc
        double volume = 0.0;
        double square = 0.0;
        for (const ocell& vc : m_fld->clusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.figure()->radius());
                square += SfromR(cell.figure()->radius());
            }
            volume -= m_fld->overlapVolume(vc);
        }
        // -> Monte-Carlo
        int stepMax = 5000;
        int positive = m_fld->monteCarlo(stepMax);
        
        result = 1000000 * square * positive / (stepMax * density * volume);
    }
    return result;
}

void OSM::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        int sx = m_fld->sizes().x;
        int sy = m_fld->sizes().y;
        int sz = m_fld->sizes().z;
        for (const ocell& vc : m_fld->clusters()) {
            for (const OCell& cell : vc) {
                volume += VfromR(cell.figure()->radius());
            }
            volume -= m_fld->overlapVolume(vc);
        }
        double aeroVolume = volume / (sx * sy * sz);
        porosity = 1 - aeroVolume;
        denAero = density * aeroVolume;
    }
}

void OSM::generate(const Sizes& sizes, double por, int initial, int step, int hit,
                   size_t cluster, double cellsize)
{
    m_finished = false;
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
    if (m_calculated) {
        // clean up
        if (m_fld) {
            delete m_fld;
        }
        m_fld = nullptr;
        m_calculated = false;
    }
    
    m_fld = new OField(sizes);
    m_calculated = true;
    
    // part 1
    std::cout << "start init field!" << std::endl;
    
    double Emin = 0.3;

    m_fld->initialize(Emin, cellsize);
    QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "end init field!" << std::endl;
    
    // part 2
    bool success = true;
    
    double allVol = (1 - por) * (sizes.x * sizes.y * sizes.z);
    
    int iterstep = 100;
    std::vector<ocell> oldclusters = m_fld->clusters();
    for (int t = 0; t < 10;) {
        success = true;
        if (m_cancel) {
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
        std::vector<Pare> pares = m_fld->agregateList(varcells);
        
        int bad = 0;
        int Kmax = 200;
        std::vector<sPar> spars;
        for (uint i = 0; i < varcells.size(); ++i) {
            spars.push_back(sPar(i));
            varcells[i].m_mark = false;
        }
        for (const Pare& p : pares) {
            ++spars[p.a].count;
            ++spars[p.b].count;
        }
        double currVol = m_fld->getVolumeAG(varcells);
        double maxVol = currVol;
        int iter = 0;
        while (allVol < currVol) {
            if (m_cancel) {
                success = false;
                break;
            }
            uint idx = rand() % count;
            std::vector<Pare> prs;
            double deltaVol = VfromR(varcells[idx].figure()->radius());
            vui srs;
            for (const Pare& p : pares) {
                if (p.a == idx) {
                    srs.push_back(p.b);
                    deltaVol -= m_fld->overlapVolumeCells(varcells[p.a], varcells[p.b]);
                } else if (p.b == idx) {
                    srs.push_back(p.a);
                    deltaVol -= m_fld->overlapVolumeCells(varcells[p.a], varcells[p.b]);
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
                    if (!varcells[idx].m_mark) {
                        std::vector<vui> agregate;

                        for (const Pare& p : prs) {
                            OField::inPareList(agregate, p);
                        }

                        if (agregate.size() > cluster) {
                            ++bad;
                        } else {
                            bad = 0;
                            varcells[idx].m_mark = true;
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
                                m_fld->setClusters(varcells);
                                reBuild(count, pares, spars, varcells);
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
            m_fld->setClusters(varcells);
            m_fld->agregate();
            break;
        } else {
            m_fld->restoreClusters(oldclusters);
        }
    }
    
    if (m_cancel) {
        QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 0));
        std::cout << "Canceled!" << std::endl;
        m_cancel = false;
        return;
    }
    
    if (!success) {
        std::cout << "Not success!" << std::endl;
        return;
    }
    
    m_finished = true;
    QMetaObject::invokeMethod(mainwindow, "setProgress", Qt::QueuedConnection, 
                Q_ARG(int, 100));
    QMetaObject::invokeMethod(mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
}

void OSM::save(const char* fileName, txt_format format) const
{
    m_fld->toFile(fileName, format);
}

void OSM::load(const char* fileName, txt_format format)
{
    if (m_fld) {
        delete m_fld;
    }
    m_fld = new OField(fileName, format);
    m_finished = true;
}

void OSM::reBuild(uint& count, std::vector<Pare>& pares,
                  std::vector<sPar>& spars, std::vector<OCell>& varcells)
{
    // rebuild varcells
    for (uint i = 0; i < varcells.size(); ) {
        if (varcells[i].m_mark) {
            varcells.erase(varcells.begin() + i);
        } else {
            ++i;
        }
    }
    
    count = varcells.size();
    pares = m_fld->agregateList(varcells);
    
    spars.clear();
    spars.reserve(varcells.size());
    for (uint i = 0; i < varcells.size(); ++i) {
        spars.push_back(sPar(i));
    }
    for (const Pare& p : pares) {
        ++spars[p.a].count;
        ++spars[p.b].count;
    }
}
