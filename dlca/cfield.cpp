#include "cfield.h"

#include <algorithm>
#include <fstream>
#include <iostream>

CField::CField(const char* fileName, txt_format format)
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

CField::CField(const Sizes& sizes)
    : Field(sizes),
      FlexibleField()
{
}

Sizes CField::sizes() const
{
    return m_sizes;
}

std::vector<Cell> CField::cells() const
{
    std::vector<Cell> result;
    for (const std::vector<CCell>& vc : m_clusters) {
        result.reserve(result.size() + vc.size());
        result.insert(result.end(), vc.begin(), vc.end());
    }
    return result;
}

const std::vector<std::vector<CCell> >& CField::clusters() const
{
    return m_clusters;
}

void CField::initialize(double porosity, double cellsize)
{
    clearCells();
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
        if (!isPointOverlapSpheres(CCell(sph, dCoord(x, y, z)))) {
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
            
            std::vector<CCell> vc = { CCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)) };
            m_clusters.push_back(vc);
            vol += sph->volume();
        } else {
            delete sph;
        }
    }
}

void CField::initializeTEST(double /*porosity*/, double cellsize)
{
    double vmax = 0.5 * cellsize;
    clearCells();
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
        std::vector<CCell> vc = { CCell(new FCylinder(ravr, 10.0 * ravr),
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
        if (!isPointOverlapSpheres(CCell(sph, dCoord(x, y, z)))) {
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
            
            std::vector<CCell> vc = { CCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)) };
            m_clusters.push_back(vc);
            vol += sph->volume();
        } else {
            delete sph;
        }
    }
}

void CField::initializeNT(double porosity, double cellsize)
{
    double vmax = 0.5 * cellsize;
    clearCells();
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
        if (!isPointOverlapSpheres(CCell(sph, dCoord(x, y, z)))) {
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
            
            std::vector<CCell> vc = { CCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)) };
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

uint32_t CField::monteCarlo(uint32_t stepMax) const
{
    uint32_t positive = 0;

    double rmin = NitroDiameter / 2.0;

    for (uint32_t i = 0; i < stepMax;) {
        uint32_t rcluster = rand() % uint32_t(m_clusters.size());
        uint32_t rcell = rand() % uint32_t(m_clusters[rcluster].size());
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

        CCell cell(new FSphere(rmin), dCoord(ixc, iyc, izc));

        bool overlap = false;
        for (size_t ic = 0; ic < m_clusters.size(); ++ic) {
            for (size_t icc = 0; icc < m_clusters[ic].size(); ++icc) {
                if (icc != rcell || ic != rcluster) {
                    if (isOverlapped(m_clusters[ic][icc], cell)) {
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

void CField::agregate()
{
    // agregate list
    std::vector<Pare> pares;

    for (uint32_t i = 0; i < uint32_t(m_clusters.size()); ++i) {
        for (const CCell& cell1 : m_clusters[i]) {
            for (uint32_t j = (i + 1); j < uint32_t(m_clusters.size()); ++j) {
                for (const CCell& cell2 : m_clusters[j]) {
                    if (isOverlapped(cell1, cell2)) {
                        pares.push_back(Pare(i, j));
                        break;
                    }
                }
            }
        }
    }

    std::vector<std::vector<uint32_t> > agregate;

    for (const Pare& p : pares) {
        inPareList(agregate, p);
    }

    // check more then 2 cluster agregation!

    for (const std::vector<uint32_t>& vu : agregate) {
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
        for (CCell& cell : m_clusters[vu[imax]]) {
            cell.setVector(v);
        }
    }

    // clean empty clusters
    cleanEmptyClusters(m_clusters);
    //std::cout << " >> " << agregate.size() << " >> " << clusters.size() << std::endl;
}

void CField::move()
{
    for (std::vector<CCell>& vc : m_clusters) {
        for (CCell& cell : vc) {
            cell.move(dt, m_sizes);
        }
    }
}

double CField::overlapVolume() const
{
    double volume = 0.0;
    for (const std::vector<CCell>& vc : m_clusters) {
        for (size_t i = 0; i < vc.size(); ++i) {
            for (size_t j = (i + 1); j < vc.size(); ++j) {
                FigureType t1 = vc[i].figure()->type();
                FigureType t2 = vc[j].figure()->type();

                if (t1 == fig_sphere && t2 == fig_sphere) {
                    volume += overlapVolumeSphereSphere(vc[i], vc[j]);
                } else if (t1 == fig_sphere && t2 == fig_cylinder) {
                    volume += overlapVolumeSphereCylinder(vc[i], vc[j]);
                } else if (t2 == fig_sphere && t1 == fig_cylinder) {
                    volume += overlapVolumeSphereCylinder(vc[j], vc[i]);
                } else if (t1 == fig_cylinder && t2 == fig_cylinder) {
                    volume += overlapVolumeCylinderCylinder(vc[i], vc[j]);
                }
            }
        }
    }

    return volume;
}

void CField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    fwrite(&m_sizes.x, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.y, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.z, sizeof(uint32_t), 1, out);
    for (const std::vector<CCell>& vc : m_clusters) {
        for (const CCell& cell : vc) {
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

void CField::toDLA(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        std::cout << m_sizes.x << "\t" << m_sizes.y << "\t" << m_sizes.z << std::endl;
        for (const std::vector<CCell>& vc : m_clusters) {
            for (const CCell& cell : vc) {
                std::cout << cell.coord().x << "\t" << cell.coord().y << "\t"
                          << cell.coord().z << "\t" << cell.figure()->radius() << std::endl;
            }
        }
        file.close();
    }
}

void CField::toTXT(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        for (const std::vector<CCell>& vc : m_clusters) {
            for (const CCell& cell : vc) {
                std::cout << cell.coord().x << "\t" << cell.coord().y << "\t"
                          << cell.coord().z << "\t" << cell.figure()->radius() << std::endl;
            }
        }
        file.close();
    }
}

void CField::fromDAT(const char* fileName)
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
        std::vector<CCell> vc = { CCell(new FSphere(f[i + 3]), dCoord(f[i], f[i + 1], f[i + 2])) };
        m_clusters.push_back(vc);
    }

    fclose(loadFile);
    agregate();
}

void CField::fromDLA(const char* fileName)
{
    FILE* in = fopen(fileName, "r");
    uint32_t dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    m_sizes = Sizes(dx, dy, dz);
    double fx, fy, fz, fr;
    // load structure
    while (fscanf(in, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        std::vector<CCell> vc = { CCell(new FSphere(fr), dCoord(fx, fy, fz)) };
        m_clusters.push_back(vc);
    }
    fclose(in);
    agregate();
}

void CField::fromTXT(const char* fileName)
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
        std::vector<CCell> vc = { CCell(new FSphere(fr), dCoord(fx, fy, fz)) };
        m_clusters.push_back(vc);
    }
    fclose(in2);
    agregate();
}

double CField::fr(double ravr)
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

void CField::clearCells()
{
    m_clusters.clear();
}

double CField::overlapVolumeSphereSphere(const CCell& cell1, const CCell& cell2) const
{
    dCoord diff = cell2.coord() - cell1.coord();
    double r = std::min(square(diff.x), square(m_sizes.x - abs(diff.x)));
    r += std::min(square(diff.y), square(m_sizes.y - abs(diff.y)));
    r += std::min(square(diff.z), square(m_sizes.z - abs(diff.z)));

    double r1 = cell1.figure()->radius();
    double r2 = cell2.figure()->radius();
    double r_sum = square(r1 + r2);

    if ((r_sum - r) > EPS) {
        double d = sqrt(r);
        return 2.0 * M_PI * ((cube(r2) - cube(d - r1)) / 3.0 - ((quad(r2) - quad(d - r1)) / 4.0
                + ((square(d) - square(r1)) * (square(r2) - square(d - r1))) / 2.0) / (2.0 * d));
    }
    return 0.0;
}

double CField::overlapVolumeSphereCylinder(const CCell& /*cell1*/, const CCell& /*cell2*/) const
{
    double volume = 0.0;

    return volume;
}

double CField::overlapVolumeCylinderCylinder(const CCell& /*cell1*/, const CCell& /*cell2*/) const
{
    double volume = 0.0;

    return volume;
}

bool CField::isOverlapSphereSphere(const CCell& cell1, const CCell& cell2) const
{
    dCoord dif = diff(cell1.coord(), cell2.coord());
    double r = square(dif.x);
    r += square(dif.y);
    r += square(dif.z);
    double r_sum = square(cell1.figure()->radius() + cell2.figure()->radius());
    return (r_sum - r) > EPS;
}

bool CField::isOverlapSphereCylinder(const CCell& cell1, const CCell& cell2) const
{
    dCoord c1 = cell1.coord();
    dCoord c2 = cell2.coord();
    dCoord dif = diff(c1, c2);
    dCoord c2d = c1 + dif;
    double r = square(dif.x);
    r += square(dif.y);
    r += square(dif.z);
    double r1 = cell1.figure()->radius();
    double r2 = cell2.figure()->radius();
    double h2 = static_cast<FCylinder*>(cell2.figure())->height() * 0.5;
    double r_sum = square(r1 + sqrt(square(r2) + square(h2)));
    // 1
    if (r > r_sum) { 
        return false;
    }
    // 2 Transform
    Vector3d transf = cell2.rotate();
    dCoord c2A(0.0, 0.0, h2);
    Vector3d N = Vector3d::Oy();
    c2A.rotate(transf.x, Vector3d::Ox());
    N.rotate  (transf.x, Vector3d::Ox());
    c2A.rotate(transf.y, N);
    dCoord c2B = c2d - c2A;
    c2A = c2d + c2A;
    Vector3d A1 = c1  - c2A;
    Vector3d B1 = c1  - c2B;
    Vector3d AB = c2B - c2A;
    double base = 2.0 * h2;//sqrt(AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);
    double baseA1 = A1.length();
    double baseB1 = B1.length();
    //double cosA = dCoord::cosA(c1, c2A, c2B);
    //double cosB = dCoord::cosA(c1, c2B, c2A);
    double cosA =  (A1.x * AB.x + A1.y * AB.y + A1.z * AB.z) / (baseA1 * base);
    double cosB = -(B1.x * AB.x + B1.y * AB.y + B1.z * AB.z) / (baseB1 * base);
    if (cosA >= 0 && cosB >= 0) {
        //double S = 0.5 * base * sqrt(1 - cosA * cosA) * baseA1;
        //double SS= 0.5 * base * (r1 + r2);
        return ((r1 + r2) > sqrt(1.0 - cosA * cosA) * baseA1);
    }
    if (cosA < 0) {
        double H = abs(cosA) * baseA1;
        if (r1 < H) {
            return false;
        }
        if ((H * H + r2 * r2) > (baseA1 * baseA1)) {
            // тупой угол - высота снаружи
            return r1 > H;
        } else {
            // острый - высота внутри
            double A = sqrt(r2 * r2 + baseA1 * baseA1 - 2.0 * r2 * baseA1 * cosA);
            return r1 > A;
        }
    } else {
        double H = abs(cosB) * baseB1;
        if (r1 < H) {
            return false;
        }
        if ((H * H + r2 * r2) > (baseB1 * baseB1)) {
            // тупой угол - высота снаружи
            return r1 > H;
        } else {
            // острый - высота внутри
            double B = sqrt(r2 * r2 + baseB1 * baseB1 - 2.0 * r2 * baseB1 * cosB);
            return r1 > B;
        }
    }
    /*CCoord cX(-transf.x, 0.0, 0.0);
    CCoord cY(0.0, -transf.y, 0.0);
    CCoord cZ(0.0, 0.0, -transf.z);
    diff.Rotated(cZ);
    diff.Rotated(cY);
    diff.Rotated(cX);
    
    //diff.Rotated(CCoord::Negative(transf));
    // 3
    bool result = false;
    result = (((r1 + r2) > sqrt(square(diff.x) + square(diff.y))) && ((r1 + h2) > abs(diff.z)));
    if (abs(diff.z) <= h2) {
        return !((square(diff.x) + square(diff.y)) - square(r1 + r2) > EPS);
    } else {
        return ((r1 + h2) - abs(diff.z) > EPS);
    }*/
}

bool CField::isOverlapCylinderCylinder(const CCell& cell1, const CCell& cell2) const
{
    dCoord c1 = cell1.coord();
    dCoord c2 = cell2.coord();
    dCoord dif = diff(c1, c2);
    dCoord c2d = c1 + dif;
    double r1 = cell1.figure()->radius();
    double h1 = static_cast<FCylinder*>(cell1.figure())->height() * 0.5;
    double r2 = cell2.figure()->radius();
    double h2 = static_cast<FCylinder*>(cell2.figure())->height() * 0.5;
    // 2
    Vector3d transf1 = cell1.rotate();
    Vector3d transf2 = cell2.rotate();
    
    dCoord c1A(0.0, 0.0, h1);
    Vector3d Ox1 = Vector3d::Ox();//(1.0, 0.0, 0.0);
    Vector3d Oy1 = Vector3d::Oy();//(0.0, 1.0, 0.0);
    Vector3d Oz1 = Vector3d::Oz();//(0.0, 0.0, 1.0);
    c1A.rotate(transf1.x, Ox1);
    Oy1.rotate(transf1.x, Ox1);
    Oz1.rotate(transf1.x, Ox1);
    c1A.rotate(transf1.y, Oy1);
    Ox1.rotate(transf1.y, Oy1);
    Oz1.rotate(transf1.y, Oy1);
    // Ox1, Oy1, Oz1 - оси цилиндра1 после поворота
    
    dCoord c1B = c1 - c1A;
    c1A = c1 + c1A;
    
    dCoord c2A(0.0, 0.0, h2);
    Vector3d Ox2 = Vector3d::Ox();
    Vector3d Oy2 = Vector3d::Oy();
    Vector3d Oz2 = Vector3d::Oz();
    c2A.rotate(transf2.x, Ox2);
    Oy2.rotate(transf2.x, Ox2);
    Oz2.rotate(transf2.x, Ox2);
    c2A.rotate(transf2.y, Oy2);
    Ox2.rotate(transf2.y, Oy2);
    Oz2.rotate(transf2.y, Oy2);
    // Ox2, Oy2, Oz2 - оси цилиндра2 после поворота
    
    dCoord c2B = c2d - c2A;
    c2A = c2d + c2A;
    
    Vector3d norm1 = dCoord::normal(Ox1, Oy1);
    Vector3d norm2 = dCoord::normal(Ox2, Oy2);
    
    if (isOverlapCylindersPoint(c1A, c1B, r1, c2A, norm2, r2)) {
        return true;
    }
    if (isOverlapCylindersPoint(c1A, c1B, r1, c2B, norm2, r2)) {
        return true;
    }
    if (isOverlapCylindersPoint(c2A, c2B, r2, c1A, norm1, r1)) {
        return true;
    }
    if (isOverlapCylindersPoint(c2A, c2B, r2, c1B, norm1, r1)) {
        return true;
    }
    //return false;
    Vector3d L1 = c1B - c1A;
    Vector3d L2 = c2B - c2A;
    // c1A - точка на плоскости
    Vector3d S = dCoord::normal(L1, L2);
    S.normalize();
    double D  = -dCoord::termwiseMultiplySum(c1A, S);
    double Smin = (dCoord::termwiseMultiplySum(c2A, S) + D) / S.length();
    
    dCoord EA = c1A + S * Smin;

    double t1, t2;
    //dCoord overpoint =
    dCoord::overlap(L1, L2, EA, c2A, t1, t2);
    if (t2 >= 0 && t2 <= 1 && t1 >= 0 && t1 <= 1) {
        return abs(Smin) <= (r1 + r2);
    }
    return false;
}

bool CField::isOverlapCylindersPoint(const dCoord& base1, const dCoord& base2,
                                     double r1, const dCoord& other,
                                     const Vector3d& area, double r2) const
{
    Vector3d C1 = base1 - base2;
    Vector3d L1 = dCoord::negative(C1);
    
    // плоскость c2A c2B c1A
    Vector3d C2AC1A = other - base1;
    Vector3d SC2ABC1A = dCoord::normal(C1, C2AC1A);
    SC2ABC1A.normalize();
    // плоскость у основания c2A
    Vector3d SC2AOC2AB = dCoord::normal(SC2ABC1A, area);
    SC2AOC2AB.normalize();
    double t1, t2;
    dCoord op1 = dCoord::overlap(L1, SC2AOC2AB, base1, other, t1, t2);
    double cosA = dCoord::cosA(base1, op1, other);
    
    if (1.0 - cosA < 0.1) {
        Vector3d V = other - base1;
        return (V.length() <= r2);
    }
    
    if (1.0 + cosA < 0.1) {
        Vector3d V = other - base2;
        return (V.length() <= r2);
    }
    Vector3d bnorm = base1 - base2;
    bnorm.normalize();
    double hip = r1 / sqrt(1.0 - cosA * cosA);
    double kat = hip * cosA;
    
    Vector3d op11 = op1 + bnorm * kat;
    
    Vector3d c2Aop1 = op11 - other;
    Vector3d c1Aop1 = op11 - base1;
    Vector3d c1Bop1 = op11 - base2;
    
    // проверка
    if (hip + r2 >= c2Aop1.length()
            && (c1Aop1.x * c1Bop1.x <= 0
                && c1Aop1.y * c1Bop1.y <= 0 && c1Aop1.z * c1Bop1.z <= 0)) {
        return true;
    }
    return false;
}

bool CField::isOverlapped(const CCell& cell1, const CCell& cell2) const
{
    FigureType t1 = cell1.figure()->type();
    FigureType t2 = cell2.figure()->type();
    if (t1 == fig_sphere && t2 == fig_sphere) {
        return isOverlapSphereSphere(cell1, cell2);
    }
    if (t1 == fig_sphere && t2 == fig_cylinder) {
        return isOverlapSphereCylinder(cell1, cell2);
    }
    if (t2 == fig_sphere && t1 == fig_cylinder) {
        return isOverlapSphereCylinder(cell2, cell1);
    }
    if (t1 == fig_cylinder && t2 == fig_cylinder) {
        return isOverlapCylinderCylinder(cell1, cell2);
    }
    return false; // error
}

bool CField::isPointOverlapSpheres(const CCell& cell) const
{
    for (const std::vector<CCell>& vc : m_clusters) {
        for (const CCell& c : vc) {
            if (isOverlapped(c, cell)) {
                return true;
            }
        }
    }
    return false;
}

dCoord CField::diff(const dCoord& c1, const dCoord& c2) const
{
    dCoord d = c2 - c1;
    dCoord diff;
    if (abs(d.x) < m_sizes.x - abs(d.x)) {
        diff.x = d.x;
    } else {
        diff.x = -(m_sizes.x - abs(d.x));
    }
    if (abs(d.y) < m_sizes.y - abs(d.y)) {
        diff.y = d.y;
    } else {
        diff.y = -(m_sizes.y - abs(d.y));
    }
    if (abs(d.z) < m_sizes.z - abs(d.z)) {
        diff.z = d.z;
    } else {
        diff.z = -(m_sizes.z - abs(d.z));
    }
    return diff;
}

double CField::leng(const CCell& cell1, const CCell& cell2)
{
    dCoord diff = cell1.coord() - cell2.coord();
    double r = std::min(square(diff.x), square(m_sizes.x - abs(diff.x)));
    r += std::min(square(diff.y), square(m_sizes.y - abs(diff.y)));
    r += std::min(square(diff.z), square(m_sizes.z - abs(diff.z)));
    return sqrt(r);
}

double CField::quad(double x)
{
    return pow(x, 4.0);
}
