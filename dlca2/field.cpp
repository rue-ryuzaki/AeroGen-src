#include "field.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace dlca2 {
XField::XField(const char* fileName, txt_format format)
    : Field(fileName, format),
      FlexibleField()
{
    switch (format) {
        case txt_dat :
            fromDAT(fileName);
            break;
        case txt_dla :
            fromDLA(fileName);
            break;
        case txt_txt :
            fromTXT(fileName);
            break;
    }
}

XField::XField(const Sizes& sizes, bool isToroid)
    : Field(sizes, isToroid),
      FlexibleField()
{
}

Sizes XField::sizes() const
{
    return m_sizes;
}

std::vector<Cell> XField::cells() const
{
    std::vector<Cell> result;
    for (const auto& vc : m_clusters) {
        result.reserve(result.size() + vc.size());
        result.insert(result.end(), vc.begin(), vc.end());
    }
    return result;
}

const std::vector<std::vector<XCell> >& XField::clusters() const
{
    return m_clusters;
}

void XField::initialize(double porosity, double cellsize)
{
    m_clusters.clear();
    double vmax = 0.5 * cellsize;
    double ravr = 0.5 * cellsize;
    double vol = 0.0;
    double allvol = (1 - porosity) * m_sizes.volume();
    while (vol < allvol) {
        // random coords
        double x = m_sizes.x * (rand() / double(RAND_MAX));
        double y = m_sizes.y * (rand() / double(RAND_MAX));
        double z = m_sizes.z * (rand() / double(RAND_MAX));
        double r = fr(ravr);
        IFigure* sph = new FSphere(r);
        if (!isCellOverlapSpheres(XCell(sph, dCoord(x, y, z)))) {
            //add sphere
            double vx = vmax * (rand() / double(RAND_MAX));
            double vy = vmax * (rand() / double(RAND_MAX));
            double vz = vmax * (rand() / double(RAND_MAX));
            uint32_t vct = rand() % 8;
            if ((vct / 1) % 2 == 0) {
                vx = -vx;
            }
            if ((vct / 2) % 2 == 0) {
                vy = -vy;
            }
            if ((vct / 4) % 2 == 0) {
                vz = -vz;
            }
            double rotx = 0.0; //360.0 * (rand() / double(RAND_MAX));
            double roty = 0.0; //360.0 * (rand() / double(RAND_MAX));
            double rotz = 0.0; //360.0 * (rand() / double(RAND_MAX));
            
            std::vector<XCell> vc = { XCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)) };
            m_clusters.push_back(vc);
            vol += sph->volume();
        } else {
            delete sph;
        }
    }
}

void XField::initializeTEST(double /*porosity*/, double cellsize)
{
    m_clusters.clear();
    double vmax = 0.5 * cellsize;
    double ravr = 0.5 * cellsize;
    double vol = 0.0;
    //return;
    {
//        double x = m_sizes.x * (rand() / double(RAND_MAX));
//        double y = m_sizes.y * (rand() / double(RAND_MAX));
//        double z = m_sizes.z * (rand() / double(RAND_MAX));
        double rotx = 360.0 * (rand() / double(RAND_MAX));
        double roty = 360.0 * (rand() / double(RAND_MAX));
//        double rotz = 360.0 * (rand() / double(RAND_MAX));
        std::vector<XCell> vc = { XCell(new FCylinder(ravr, 10.0 * ravr),
                                  dCoord(25, 25, 25),
                                  Vector3d(rotx, roty)) };
        m_clusters.push_back(vc);
    }
    //return;
    for (uint32_t io = 0; io < 20;) {
        // random coords
        double x = m_sizes.x * (rand() / double(RAND_MAX));
        double y = m_sizes.y * (rand() / double(RAND_MAX));
        double z = m_sizes.z * (rand() / double(RAND_MAX));
        double r = fr(ravr);
        IFigure* sph;
        //uint32_t ftype = rand() % 2;
        switch (0) {
            case 0 :
                sph = new FSphere(r);
                break;
            case 1 :
                sph = new FCylinder(r, 10.0 * r);
                break;
            case 2 :
                sph = new FSphere(r);//FCube(2.0 * r);
                break;
            default :
                sph = new FSphere(r);
                break;
        }
        if (!isCellOverlapSpheres(XCell(sph, dCoord(x, y, z)))) {
            //add sphere
            ++io;
            double vx = vmax * (rand() / double(RAND_MAX));
            double vy = vmax * (rand() / double(RAND_MAX));
            double vz = vmax * (rand() / double(RAND_MAX));
            uint32_t vct = rand() % 8;
            if ((vct / 1) % 2 == 0) {
                vx = -vx;
            }
            if ((vct / 2) % 2 == 0) {
                vy = -vy;
            }
            if ((vct / 4) % 2 == 0) {
                vz = -vz;
            }
            double rotx = 360.0 * (rand() / double(RAND_MAX));
            double roty = 360.0 * (rand() / double(RAND_MAX));
            double rotz = 0.0;//360 * (rand() / double(RAND_MAX));
            
            std::vector<XCell> vc = { XCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)) };
            m_clusters.push_back(vc);
            vol += sph->volume();
        } else {
            delete sph;
        }
    }
}

void XField::initializeNT(double porosity, double cellsize)
{
    m_clusters.clear();
    double vmax = 0.5 * cellsize;
    double ravr = 0.5 * cellsize;
    double volAG = 0.0;
    double volNT = 0.0;
    double concNT = 0.3;
    double allvol = (1 - porosity) * m_sizes.volume();
    double allvolNT = allvol * concNT;
    double allvolAG = allvol * (1 - concNT);
    while (volAG < allvolAG || volNT < allvolNT) {
        // random coords
        double x = m_sizes.x * (rand() / double(RAND_MAX));
        double y = m_sizes.y * (rand() / double(RAND_MAX));
        double z = m_sizes.z * (rand() / double(RAND_MAX));
        double r = fr(ravr);
        IFigure* sph;
        uint32_t ftype = rand() % 2;
        if (ftype == 0 && volAG >= allvolAG) {
            ftype = 1;
        }
        if (ftype == 1 && volNT >= allvolNT) {
            ftype = 0;
        }
        switch (ftype) {
            case 0 :
                sph = new FSphere(r);
                break;
            case 1 :
                sph = new FCylinder(2.0 * r, 40.0 * r);
                break;
            case 2 :
                sph = new FSphere(r);//FCube(2.0 * r);
                break;
            default :
                sph = new FSphere(r);
                break;
        }
        if (!isCellOverlapSpheres(XCell(sph, dCoord(x, y, z)))) {
            //add sphere
            double vx = vmax * (rand() / double(RAND_MAX));
            double vy = vmax * (rand() / double(RAND_MAX));
            double vz = vmax * (rand() / double(RAND_MAX));
            uint32_t vct = rand() % 8;
            if ((vct / 1) % 2 == 0) {
                vx = -vx;
            }
            if ((vct / 2) % 2 == 0) {
                vy = -vy;
            }
            if ((vct / 4) % 2 == 0) {
                vz = -vz;
            }
            double rotx = 360.0 * (rand() / double(RAND_MAX));
            double roty = 360.0 * (rand() / double(RAND_MAX));
            double rotz = 0.0;//360 * (rand() / double(RAND_MAX));
            
            std::vector<XCell> vc = { XCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)) };
            m_clusters.push_back(vc);
            switch (ftype) {
                case 0 :
                    volAG += sph->volume();
                    break;
                case 1 :
                    volNT += sph->volume();
                    break;
            }
        } else {
            delete sph;
        }
    }
}

uint32_t XField::monteCarlo(uint32_t stepMax) const
{
    uint32_t positive = 0;

    double rmin = NitroDiameter / 2.0;

    for (uint32_t i = 0; i < stepMax;) {
        size_t rcluster = size_t(rand() % int32_t(m_clusters.size()));
        size_t rcell = size_t(rand() % int32_t(m_clusters[rcluster].size()));
        if (m_clusters[rcluster][rcell].figure()->type() == fig_cylinder) {
            continue;
        }
        ++i;
        double xc = m_clusters[rcluster][rcell].coord().x;
        double yc = m_clusters[rcluster][rcell].coord().y;
        double zc = m_clusters[rcluster][rcell].coord().z;
        double rc = m_clusters[rcluster][rcell].figure()->radius();

        //spheric!
        double teta = 2.0 * M_PI * (rand() / double(RAND_MAX));
        double phi  = 2.0 * M_PI * (rand() / double(RAND_MAX));

        double ixc = xc + (rc + rmin) * sin(teta) * cos(phi);
        double iyc = yc + (rc + rmin) * sin(teta) * sin(phi);
        double izc = zc + (rc + rmin) * cos(teta);

        XCell cell(new FSphere(rmin), dCoord(ixc, iyc, izc));

        bool overlap = false;
        for (size_t ic = 0; ic < m_clusters.size(); ++ic) {
            for (size_t icc = 0; icc < m_clusters[ic].size(); ++icc) {
                if (icc != rcell || ic != rcluster) {
                    if (isOverlapped(&m_clusters[ic][icc], &cell)) {
                        overlap = true;
                        break;
                    }
                }
            }
        }
        if (!overlap) {
            ++positive;
        }
    }
    return positive;
}

void XField::agregate()
{
    // agregate list
    std::vector<Pare> pares;

    for (uint32_t i = 0; i < uint32_t(m_clusters.size()); ++i) {
        for (const auto& cell1 : m_clusters[i]) {
            for (uint32_t j = (i + 1); j < uint32_t(m_clusters.size()); ++j) {
                for (const auto& cell2 : m_clusters[j]) {
                    if (isOverlapped(&cell1, &cell2)) {
                        pares.push_back(Pare(i, j));
                        break;
                    }
                }
            }
        }
    }

    std::vector<std::vector<uint32_t> > agregate;

    for (const auto& p : pares) {
        inPareList(agregate, p);
    }

    // check more then 2 cluster agregation!

    for (const auto& vu : agregate) {
        size_t cnt = vu.size();

        uint32_t ms[cnt];
        Vector3d vs[cnt];
        uint32_t summ = 0;
        Vector3d sump;
        uint32_t smax = 0;
        uint32_t imax = 0;
        for (uint32_t i = 0; i < cnt; ++i) {
            ms[i] = uint32_t(m_clusters[vu[i]].size());
            if (smax < ms[i]) {
                smax = ms[i];
                imax = i;
            }
            summ += ms[i];
            vs[i] = m_clusters[vu[i]][0].vector();
            sump = sump + vs[i] * ms[i];
        }
        Vector3d v = sump / summ;

        // agregation in 1 cluster
        for (uint32_t i = 0; i < cnt; ++i) {
            if (i != imax) {
                m_clusters[vu[imax]].reserve(m_clusters[vu[imax]].size() + m_clusters[vu[i]].size());
                copy(m_clusters[vu[i]].begin(), m_clusters[vu[i]].end(), back_inserter(m_clusters[vu[imax]]));
                m_clusters[vu[i]].clear();
            }
        }
        // set new vector
        for (XCell& cell : m_clusters[vu[imax]]) {
            cell.setVector(v);
        }
    }

    // clean empty clusters
    cleanEmptyClusters(m_clusters);
    //std::cout << " >> " << agregate.size() << " >> " << clusters.size() << std::endl;
}

void XField::move()
{
    for (std::vector<XCell>& vc : m_clusters) {
        for (XCell& cell : vc) {
            cell.move(dt, m_sizes);
        }
    }
}

double XField::overlapVolume() const
{
    double volume = 0.0;
    for (const auto& vc : m_clusters) {
        for (size_t i = 0; i < vc.size(); ++i) {
            for (size_t j = (i + 1); j < vc.size(); ++j) {
                FigureType t1 = vc[i].figure()->type();
                FigureType t2 = vc[j].figure()->type();

                if (t1 == fig_sphere && t2 == fig_sphere) {
                    volume += overlapVolumeSphSph(&vc[i], &vc[j]);
                } else if (t1 == fig_sphere && t2 == fig_cylinder) {
                    volume += overlapVolumeSphCyl(&vc[i], &vc[j]);
                } else if (t2 == fig_sphere && t1 == fig_cylinder) {
                    volume += overlapVolumeSphCyl(&vc[j], &vc[i]);
                } else if (t1 == fig_cylinder && t2 == fig_cylinder) {
                    volume += overlapVolumeCylCyl(&vc[i], &vc[j]);
                }
            }
        }
    }

    return volume;
}

void XField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    fwrite(&m_sizes.x, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.y, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.z, sizeof(uint32_t), 1, out);
    for (const auto& vc : m_clusters) {
        for (const auto& cell : vc) {
            double x = cell.coord().x;
            double y = cell.coord().y;
            double z = cell.coord().z;
            double r = cell.figure()->radius();
            fwrite(&x, sizeof(double), 1, out);
            fwrite(&y, sizeof(double), 1, out);
            fwrite(&z, sizeof(double), 1, out);
            fwrite(&r, sizeof(double), 1, out);
        }
    }
    fclose(out);
}

void XField::toDLA(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        std::cout << m_sizes.x << "\t" << m_sizes.y << "\t" << m_sizes.z << std::endl;
        for (const auto& vc : m_clusters) {
            for (const auto& cell : vc) {
                std::cout << cell.coord().x << "\t" << cell.coord().y << "\t"
                          << cell.coord().z << "\t" << cell.figure()->radius() << std::endl;
            }
        }
        file.close();
    }
}

void XField::toTXT(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        for (const auto& vc : m_clusters) {
            for (const auto& cell : vc) {
                std::cout << cell.coord().x << "\t" << cell.coord().y << "\t"
                          << cell.coord().z << "\t" << cell.figure()->radius() << std::endl;
            }
        }
        file.close();
    }
}

void XField::fromDAT(const char* fileName)
{
    FILE* loadFile = fopen(fileName, "rb+");
    //Define file size:
    fseek(loadFile, 0L, SEEK_END);
    uint32_t sc = uint32_t(ftell(loadFile));
    fseek(loadFile, 0L, SEEK_SET);
    uint32_t dx, dy, dz;
    fread(&dx, sizeof(uint32_t), 1, loadFile);
    fread(&dy, sizeof(uint32_t), 1, loadFile);
    fread(&dz, sizeof(uint32_t), 1, loadFile);
    m_sizes = Sizes(dx, dy, dz);
    sc -= uint32_t(sizeof(uint32_t)) * 3;
    uint32_t total = sc / uint32_t(sizeof(double));
    double f[total];
    // load structure
    fread(&f, sizeof(double), total, loadFile);

    for (uint32_t i = 0; i < total; i += 4) {
        std::vector<XCell> vc = { XCell(new FSphere(f[i + 3]), dCoord(f[i], f[i + 1], f[i + 2])) };
        m_clusters.push_back(vc);
    }

    fclose(loadFile);
    agregate();
}

void XField::fromDLA(const char* fileName)
{
    FILE* in = fopen(fileName, "r");
    uint32_t dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    m_sizes = Sizes(dx, dy, dz);
    double fx, fy, fz, fr;
    // load structure
    while (fscanf(in, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        std::vector<XCell> vc = { XCell(new FSphere(fr), dCoord(fx, fy, fz)) };
        m_clusters.push_back(vc);
    }
    fclose(in);
    agregate();
}

void XField::fromTXT(const char* fileName)
{
    uint32_t dx = 0, dy = 0, dz = 0;
    FILE* in1 = fopen(fileName, "r");
    double fx, fy, fz, fr;
    while (fscanf(in1, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        if (dx < fx + fr) {
            dx = uint32_t(fx + fr + 1);
        }
        if (dy < fy + fr) {
            dy = uint32_t(fy + fr + 1);
        }
        if (dz < fz + fr) {
            dz = uint32_t(fz + fr + 1);
        }
    }
    fclose(in1);

    FILE* in2 = fopen(fileName, "r");
    m_sizes = Sizes(dx, dy, dz);
    // load structure
    while (fscanf(in2, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        std::vector<XCell> vc = { XCell(new FSphere(fr), dCoord(fx, fy, fz)) };
        m_clusters.push_back(vc);
    }
    fclose(in2);
    agregate();
}

double XField::fr(double ravr)
{
    // функция распределения частиц по размерам
    double r = ravr;
    uint32_t rnd = rand() % 8;
    switch (rnd) {
        case 2 :
            //r *= 0.8;
            break;
        case 6 :
            //r *= 1.2;
            break;
    }
    return r;
}

bool XField::isCellOverlapSpheres(const XCell& cell) const
{
    for (const auto& vc : m_clusters) {
        for (const auto& c : vc) {
            if (isOverlapped(&c, &cell)) {
                return true;
            }
        }
    }
    return false;
}
} // dlca2
