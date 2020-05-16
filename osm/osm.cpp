#include "osm.h"

#include <iostream>

OSM::OSM(QObject* parent)
    : Generator(parent)
{
}

OSM::~OSM()
{
    if (m_fld) {
        delete m_fld;
        m_fld = nullptr;
    }
}

osm::XField* OSM::field() const
{
    return m_fld;
}

void OSM::generate(const Sizes& sizes, const RunParams& params)
{
    m_finished = false;
    QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 0));
    // clean up
    if (m_fld) {
        delete m_fld;
    }
    m_fld = new osm::XField(sizes, params.isToroid);
#ifndef _WIN32
    uint32_t t0 = uint32_t(clock());
#endif
    // part 1
    std::cout << "start init field!" << std::endl;

    double Emin = 0.3;

    m_fld->initialize(Emin, params.cellSize);
    QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "end init field!" << std::endl;

    // part 2
    bool success = true;

    double allVol = (1.0 - params.porosity) * sizes.volume();

    uint32_t iterstep = 100;
    const auto& oldclusters = m_fld->clusters();
    for (uint32_t t = 0; t < 10;) {
        success = true;
        if (m_cancel) {
            success = false;
            break;
        }
        std::vector<osm::XCell> cells;
        for (const auto& vc : oldclusters) {
            cells.reserve(cells.size() + vc.size());
            cells.insert(cells.end(), vc.begin(), vc.end());
        }
        uint32_t count = uint32_t(cells.size());
        std::cout << "Current: " << ++t << std::endl;
        std::vector<Pare> pares = m_fld->agregateList(cells);

        uint32_t bad = 0;
        uint32_t Kmax = 200;
        std::vector<sPar> spars;
        for (uint32_t i = 0; i < uint32_t(cells.size()); ++i) {
            spars.push_back(sPar(i));
            cells[i].mark = false;
        }
        for (const auto& p : pares) {
            ++spars[p.a].count;
            ++spars[p.b].count;
        }
        double currVol = m_fld->getVolumeAG(cells);
        double maxVol = currVol;
        uint32_t iter = 0;
        while (allVol < currVol) {
            if (m_cancel) {
                success = false;
                break;
            }
            uint32_t idx = uint32_t(rand() % count);
            std::vector<Pare> prs;
            double deltaVol = cells[idx].figure()->volume();
            std::vector<uint32_t> srs;
            for (const auto& p : pares) {
                if (p.a == idx) {
                    srs.push_back(p.b);
                    deltaVol -= m_fld->overlapVolumeCells(cells[p.a], cells[p.b]);
                } else if (p.b == idx) {
                    srs.push_back(p.a);
                    deltaVol -= m_fld->overlapVolumeCells(cells[p.a], cells[p.b]);
                } else {
                    prs.push_back(p);
                }
            }
            bool checkSPAR = true;
            for (const uint32_t& ui : srs) {
                if (spars[ui].count < 2) {
                    checkSPAR = false;
                    break;
                }
            }
            if (checkSPAR) {
                if (prs.size() < pares.size()) {
                    if (!cells[idx].mark) {
                        std::vector<std::vector<uint32_t> > agregate;
                        for (const auto& p : prs) {
                            inPareList(agregate, p);
                        }
                        if (agregate.size() > params.cluster) {
                            ++bad;
                        } else {
                            bad = 0;
                            cells[idx].mark = true;
                            pares.clear();
                            pares.reserve(prs.size());
                            pares.insert(pares.end(), prs.begin(), prs.end());

                            for (const uint32_t& ui : srs) {
                                --spars[ui].count;
                            }
                            currVol -= deltaVol;
                            ++iter;
                            if (iter % iterstep == 0) {
                                iter = 0;
                                m_fld->setClusters(cells);
                                reBuild(count, pares, spars, cells);
                                QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                                    Q_ARG(int, std::min(100, int(100 * (1 - (currVol - allVol) / (maxVol - allVol))))));
                                QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
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
            m_fld->setClusters(cells);
            m_fld->agregate();
            break;
        } else {
            m_fld->restoreClusters(oldclusters);
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
    if (!success) {
        std::cout << "Not success!" << std::endl;
        return;
    }
    m_finished = true;
    QMetaObject::invokeMethod(m_mainwindow, "setProgress", Qt::QueuedConnection,
                Q_ARG(int, 100));
    QMetaObject::invokeMethod(m_mainwindow, "restructGL", Qt::QueuedConnection);
    std::cout << "Done" << std::endl;
}

double OSM::surfaceArea(double density, uint32_t steps) const
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
            volume -= m_fld->overlapVolume(vc);
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

void OSM::density(double density, double& denAero, double& porosity) const
{
    if (m_finished) {
        // calc
        double volume = 0.0;
        for (const auto& vc : m_fld->clusters()) {
            for (const auto& cell : vc) {
                volume += cell.figure()->volume();
            }
            volume -= m_fld->overlapVolume(vc);
        }
        double aeroVolume = volume / m_fld->sizes().volume();
        porosity = 1.0 - aeroVolume;
        denAero = density * aeroVolume;
    }
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
    m_fld = new osm::XField(fileName, format);
    m_finished = true;
}

void OSM::reBuild(uint32_t& count, std::vector<Pare>& pares,
                  std::vector<sPar>& spars, std::vector<osm::XCell>& cells)
{
    // rebuild varcells
    cells.erase(std::remove_if(cells.begin(), cells.end(),
                               [] (osm::XCell& k) { return k.mark; }),
                cells.end());
    
    count = uint32_t(cells.size());
    pares = m_fld->agregateList(cells);
    
    spars.clear();
    spars.reserve(cells.size());
    for (uint32_t i = 0; i < uint32_t(cells.size()); ++i) {
        spars.push_back(sPar(i));
    }
    for (const auto& p : pares) {
        ++spars[p.a].count;
        ++spars[p.b].count;
    }
}
