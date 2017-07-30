#include "cfield.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

CField::CField(const char* fileName, txt_format format)
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

CField::CField(Sizes sizes)
    : Field(sizes)
{
}

CField::~CField()
{
}

Sizes CField::getSizes() const
{
    return sizes;
}

std::vector<Cell> CField::getCells() const
{
    std::vector<Cell> result;
    for (const vcell& vc : clusters) {
        for (const CCell& c : vc) {
            result.push_back(Cell(c));
        }
    }
    return result;
}

std::vector<vcell> CField::getClusters() const
{
    return clusters;
}

void CField::Initialize(double porosity, double cellsize)
{
    double vmax = 0.5 * cellsize;
    clearCells();
    double ravr = 0.5 * cellsize;
    double vol = 0.0;
    double allvol = (1 - porosity) * (sizes.x * sizes.y * sizes.z);
    while (vol < allvol) {
        // random coords
        double x = sizes.x * (rand() / double(RAND_MAX));
        double y = sizes.y * (rand() / double(RAND_MAX));
        double z = sizes.z * (rand() / double(RAND_MAX));
        double r = fr(ravr);
        IFigure* sph = new FSphere(r);
        if (!is_point_overlap_spheres(CCell(sph, dCoord(x, y, z)))) {
            //add sphere
            double vx = vmax * (rand() / double(RAND_MAX));
            double vy = vmax * (rand() / double(RAND_MAX));
            double vz = vmax * (rand() / double(RAND_MAX));
            int vct = rand() % 8;
            if ((vct / 1) % 2 == 0) {
                vx = -vx;
            }
            if ((vct / 2) % 2 == 0) {
                vy = -vy;
            }
            if ((vct / 4) % 2 == 0) {
                vz = -vz;
            }
            double rotx = 0.0; //360 * (rand() / double(RAND_MAX));
            double roty = 0.0; //360 * (rand() / double(RAND_MAX));
            double rotz = 0.0; //360 * (rand() / double(RAND_MAX));
            
            vcell vc;
            vc.push_back(CCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)));
            clusters.push_back(vc);
            vol += sph->getVolume();
        } else {
            delete sph;
        }
    }
}

void CField::InitializeTEST(double porosity, double cellsize)
{
    double vmax = 0.5 * cellsize;
    clearCells();
    double ravr = 0.5 * cellsize;
    double vol = 0.0;
    //return;
    {
        double x = sizes.x * (rand() / double(RAND_MAX));
        double y = sizes.y * (rand() / double(RAND_MAX));
        double z = sizes.z * (rand() / double(RAND_MAX));
        IFigure* sph = new FCylinder(ravr, 10 * ravr);
        vcell vc;
        double rotx = 360 * (rand() / double(RAND_MAX));
        double roty = 360 * (rand() / double(RAND_MAX));
        double rotz = 360 * (rand() / double(RAND_MAX));
        vc.push_back(CCell(sph, dCoord(25, 25, 25), Vector3d(rotx, roty)));
        clusters.push_back(vc);
    }
    //return;
    for (int io = 0; io < 20;) {
        // random coords
        double x = sizes.x * (rand() / double(RAND_MAX));
        double y = sizes.y * (rand() / double(RAND_MAX));
        double z = sizes.z * (rand() / double(RAND_MAX));
        double r = fr(ravr);
        IFigure* sph;
        //int ftype = rand() % 2;
        switch (0) {
            case 0 :
                sph = new FSphere(r);
                break;
            case 1 :
                sph = new FCylinder(r, 10 * r);
                break;
            case 2 :
                sph = new FSphere(r);//FCube(2 * r);
                break;
            default :
                sph = new FSphere(r);
                break;
        }
        if (!is_point_overlap_spheres(CCell(sph, dCoord(x, y, z)))) {
            //add sphere
            ++io;
            double vx = vmax * (rand() / double(RAND_MAX));
            double vy = vmax * (rand() / double(RAND_MAX));
            double vz = vmax * (rand() / double(RAND_MAX));
            int vct = rand() % 8;
            if ((vct / 1) % 2 == 0) {
                vx = -vx;
            }
            if ((vct / 2) % 2 == 0) {
                vy = -vy;
            }
            if ((vct / 4) % 2 == 0) {
                vz = -vz;
            }
            double rotx = 360 * (rand() / double(RAND_MAX));
            double roty = 360 * (rand() / double(RAND_MAX));
            double rotz = 0;//360 * (rand() / double(RAND_MAX));
            
            vcell vc;
            vc.push_back(CCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)));
            clusters.push_back(vc);
            vol += sph->getVolume();
        } else {
            delete sph;
        }
    }
}

void CField::InitializeNT(double porosity, double cellsize)
{
    double vmax = 0.5 * cellsize;
    clearCells();
    double ravr = 0.5 * cellsize;
    double volAG = 0.0;
    double volNT = 0.0;
    double concNT = 0.3;
    double allvol = (1 - porosity) * (sizes.x * sizes.y * sizes.z);
    double allvolNT = allvol * concNT;
    double allvolAG = allvol * (1 - concNT);
    while (volAG < allvolAG || volNT < allvolNT) {
        // random coords
        double x = sizes.x * (rand() / double(RAND_MAX));
        double y = sizes.y * (rand() / double(RAND_MAX));
        double z = sizes.z * (rand() / double(RAND_MAX));
        double r = fr(ravr);
        IFigure* sph;
        int ftype = rand() % 2;
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
                sph = new FCylinder(2 * r, 40 * r);
                break;
            case 2 :
                sph = new FSphere(r);//FCube(2 * r);
                break;
            default :
                sph = new FSphere(r);
                break;
        }
        if (!is_point_overlap_spheres(CCell(sph, dCoord(x, y, z)))) {
            //add sphere
            double vx = vmax * (rand() / double(RAND_MAX));
            double vy = vmax * (rand() / double(RAND_MAX));
            double vz = vmax * (rand() / double(RAND_MAX));
            int vct = rand() % 8;
            if ((vct / 1) % 2 == 0) {
                vx = -vx;
            }
            if ((vct / 2) % 2 == 0) {
                vy = -vy;
            }
            if ((vct / 4) % 2 == 0) {
                vz = -vz;
            }
            double rotx = 360 * (rand() / double(RAND_MAX));
            double roty = 360 * (rand() / double(RAND_MAX));
            double rotz = 0.0;//360 * (rand() / double(RAND_MAX));
            
            vcell vc;
            vc.push_back(CCell(sph, dCoord(x, y, z), Vector3d(rotx, roty, rotz), Vector3d(vx, vy, vz)));
            clusters.push_back(vc);
            switch (ftype) {
                case 0 :
                    volAG += sph->getVolume();
                    break;
                case 1 :
                    volNT += sph->getVolume();
                    break;
            }
        } else {
            delete sph;
        }
    }
}

int CField::MonteCarlo(int stepMax)
{
    int positive = 0;

    double rmin = NitroDiameter / 2;

    for (int i = 0; i < stepMax;) {
        uint rcluster = rand() % clusters.size();
        uint rcell = rand() % clusters[rcluster].size();
        if (clusters[rcluster][rcell].getFigure()->getType() == fig_cylinder) {
            continue;
        }
        ++i;
        double xc = clusters[rcluster][rcell].getCoord().x;
        double yc = clusters[rcluster][rcell].getCoord().y;
        double zc = clusters[rcluster][rcell].getCoord().z;
        double rc = clusters[rcluster][rcell].getFigure()->getRadius();

        //spheric!
        double teta = 2 * M_PI * (rand() / double(RAND_MAX));
        double phi  = 2 * M_PI * (rand() / double(RAND_MAX));

        double ixc = xc + (rc + rmin) * sin(teta) * cos(phi);
        double iyc = yc + (rc + rmin) * sin(teta) * sin(phi);
        double izc = zc + (rc + rmin) * cos(teta);
        FSphere* sph = new FSphere(rmin);
        CCell cell(sph, dCoord(ixc, iyc, izc));

        bool overlap = false;
        for (uint ic = 0; ic < clusters.size(); ++ic) {
            for (uint icc = 0; icc < clusters[ic].size(); ++icc) {
                if (overlap) {
                    break;
                }
                if (icc != rcell || ic != rcluster) {
                    if (is_overlapped(clusters[ic][icc], cell)) {
                        overlap = true;
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

void CField::Agregate()
{
    // agregate list
    std::vector<Pare> pares;

    for (uint i = 0; i < clusters.size(); ++i) {
        for (const CCell& cell1 : clusters[i]) {
            for (uint j = (i + 1); j < clusters.size(); ++j) {
                bool overlap = false;
                for (const CCell& cell2 : clusters[j]) {
                    if (overlap) {
                        break;
                    }
                    if (is_overlapped(cell1, cell2)) {
                        overlap = true;
                        pares.push_back(Pare(i, j));
                    }
                }
            }
        }
    }

    std::vector<vui> agregate;

    for (const Pare& p : pares) {
        inPareList(agregate, p);
    }

    // check more then 2 cluster agregation!

    for (const vui& vu : agregate) {
        uint cnt = vu.size();

        uint ms[cnt];
        Vector3d vs[cnt];
        uint summ = 0;
        Vector3d sump;
        uint smax = 0;
        uint imax = 0;
        for (uint i = 0; i < cnt; ++i) {
            ms[i] = clusters[vu[i]].size();
            if (smax < ms[i]) {
                smax = ms[i];
                imax = i;
            }
            summ += ms[i];
            vs[i] = clusters[vu[i]][0].getVector();
            sump = sump + vs[i] * ms[i];
        }
        Vector3d v = sump / summ;

        // agregation in 1 cluster
        for (uint i = 0; i < cnt; ++i) {
            if (i != imax) {
                /*clusters[vu[imax]].reserve(clusters[vu[imax]].size() + clusters[vu[i]].size());
                clusters[vu[imax]].insert(clusters[vu[imax]].end(), clusters[vu[i]].begin(), clusters[vu[i]].end());
                clusters[vu[i]].clear();*/
                /*copy(clusters[vu[i]].begin(), clusters[vu[i]].end(), back_inserter(clusters[vu[imax]]));
                clusters[vu[i]].clear();*/
                while (clusters[vu[i]].size() > 0) {
                    clusters[vu[imax]].push_back(clusters[vu[i]].back());
                    clusters[vu[i]].pop_back();
                }
            }
        }
        // set new vector
        for (CCell& cell : clusters[vu[imax]]) {
            cell.setVector(v);
        }
    }

    // clean empty clusters
    for (uint i = 0; i < clusters.size();) {
        if (clusters[i].empty()) {
            clusters.erase(clusters.begin() + i);
        } else {
             ++i;
        }
    }
    //std::cout << " >> " << agregate.size() << " >> " << clusters.size() << std::endl;
}

void CField::Move()
{
    for (vcell& vc : clusters) {
        for (CCell& cell : vc) {
            cell.move(dt, sizes);
        }
    }
}

double CField::overlapVolume()
{
    double volume = 0.0;
    for (const vcell& vc : clusters) {
        for (uint i = 0; i < vc.size(); ++i) {
            for (uint j = (i + 1); j < vc.size(); ++j) {
                FigureType t1 = vc[i].getFigure()->getType();
                FigureType t2 = vc[j].getFigure()->getType();

                if (t1 == fig_sphere && t2 == fig_sphere) {
                    volume += overlapVolume_sphere_sphere(vc[i], vc[j]);
                }

                if (t1 == fig_sphere && t2 == fig_cylinder) {
                    volume += overlapVolume_sphere_cylinder(vc[i], vc[j]);
                }

                if (t2 == fig_sphere && t1 == fig_cylinder) {
                    volume += overlapVolume_sphere_cylinder(vc[j], vc[i]);
                }

                if (t1 == fig_cylinder && t2 == fig_cylinder) {
                    volume += overlapVolume_cylinder_cylinder(vc[i], vc[j]);
                }
            }
        }
    }

    return volume;
}

void CField::toDLA(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    fprintf(out, "%d\t%d\t%d\n", sizes.x, sizes.y, sizes.z);

    for (const vcell& vc : clusters) {
        for (const CCell& cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getFigure()->getRadius());
        }
    }
    fclose(out);
}

void CField::toTXT(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    for (const vcell& vc : clusters) {
        for (const CCell& cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getFigure()->getRadius());
        }
    }
    fclose(out);
}

void CField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    fwrite(&sizes.x, sizeof(int), 1, out);
    fwrite(&sizes.y, sizeof(int), 1, out);
    fwrite(&sizes.z, sizeof(int), 1, out);
    for (const vcell& vc : clusters) {
        for (const CCell& cell : vc) {
            double x = cell.getCoord().x;
            double y = cell.getCoord().y;
            double z = cell.getCoord().z;
            double r = cell.getFigure()->getRadius();
            fwrite(&x, sizeof(double), 1, out);
            fwrite(&y, sizeof(double), 1, out);
            fwrite(&z, sizeof(double), 1, out);
            fwrite(&r, sizeof(double), 1, out);
        }
    }
    fclose(out);
}

void CField::fromDLA(const char* fileName)
{
    FILE* in = fopen(fileName, "r");
    int dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    sizes = Sizes(dx, dy, dz);
    double fx, fy, fz, fr;
    // load structure
    while (fscanf(in, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        vcell vc;
        FSphere* sph = new FSphere(fr);
        vc.push_back(CCell(sph, dCoord(fx, fy, fz)));
        clusters.push_back(vc);
    }
    fclose(in);
    Agregate();
}

void CField::fromTXT(const char* fileName)
{
    int dx = 0, dy = 0, dz = 0;
    FILE* in1 = fopen(fileName, "r");
    double fx, fy, fz, fr;
    while (fscanf(in1, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        if (dx < fx + fr) dx = int(fx + fr + 1);
        if (dy < fy + fr) dy = int(fy + fr + 1);
        if (dz < fz + fr) dz = int(fz + fr + 1);
    }
    fclose(in1);

    FILE* in2 = fopen(fileName, "r");
    sizes = Sizes(dx, dy, dz);
    // load structure
    while (fscanf(in2, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        vcell vc;
        FSphere* sph = new FSphere(fr);
        vc.push_back(CCell(sph, dCoord(fx, fy, fz)));
        clusters.push_back(vc);
    }
    fclose(in2);
    Agregate();
}

void CField::fromDAT(const char* fileName)
{
    FILE* loadFile = fopen(fileName, "rb+");
    //Define file size:
    fseek(loadFile, 0L, SEEK_END);
    long sc = ftell(loadFile);
    fseek(loadFile, 0L, SEEK_SET);
    int dx, dy, dz;
    fread(&dx, sizeof(int), 1, loadFile);
    fread(&dy, sizeof(int), 1, loadFile);
    fread(&dz, sizeof(int), 1, loadFile);
    sizes = Sizes(dx, dy, dz);
    sc -= sizeof(int) * 3;
    long total = sc / sizeof(double);
    double f[total];
    // load structure
    fread(&f, sizeof(double), total, loadFile);

    for (int i = 0; i < total; i += 4) {
        vcell vc;
        FSphere* sph = new FSphere(f[i + 3]);
        vc.push_back(CCell(sph, dCoord(f[i], f[i + 1], f[i + 2])));
        clusters.push_back(vc);
    }

    fclose(loadFile);
    Agregate();
}

double CField::fr(double ravr)
{
    // функция распределения частиц по размерам
    double r = ravr;
    int rnd = rand() % 8;
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

void CField::clearCells() {
    clusters.clear();
}

double CField::overlapVolume_sphere_sphere(const CCell& cell1, const CCell& cell2)
{
    dCoord diff = cell2.getCoord() - cell1.getCoord();
    double r = std::min(square(diff.x), square(sizes.x - abs(diff.x)));
    r += std::min(square(diff.y), square(sizes.y - abs(diff.y)));
    r += std::min(square(diff.z), square(sizes.z - abs(diff.z)));

    double r1 = cell1.getFigure()->getRadius();
    double r2 = cell2.getFigure()->getRadius();
    double r_sum = square(r1 + r2);

    if ((r_sum - r) > EPS) {
        double d = pow(r, 0.5);
        return 2 * M_PI * ((cube(r2) - cube(d - r1)) / 3 - ((quad(r2) - quad(d - r1)) / 4
                + ((square(d) - square(r1)) * (square(r2) - square(d - r1))) / 2) / (2 * d));
    }
    return 0.0;
}

double CField::overlapVolume_sphere_cylinder(const CCell& cell1, const CCell& cell2)
{
    double volume = 0.0;

    return volume;
}

double CField::overlapVolume_cylinder_cylinder(const CCell& cell1, const CCell& cell2)
{
    double volume = 0.0;

    return volume;
}

bool CField::is_overlap_sphere_sphere(const CCell& cell1, const CCell& cell2)
{
    dCoord diff = Diff(cell1.getCoord(), cell2.getCoord());
    double r = square(diff.x);
    r += square(diff.y);
    r += square(diff.z);
    double r_sum = square(cell1.getFigure()->getRadius() + cell2.getFigure()->getRadius());
    return (r_sum - r) > EPS;
}

bool CField::is_overlap_sphere_cylinder(const CCell& cell1, const CCell& cell2)
{
    dCoord c1 = cell1.getCoord();
    dCoord c2 = cell2.getCoord();
    dCoord diff = Diff(c1, c2);
    dCoord c2d = c1 + diff;
    double r = square(diff.x);
    r += square(diff.y);
    r += square(diff.z);
    double r1 = cell1.getFigure()->getRadius();
    double r2 = cell2.getFigure()->getRadius();
    double h2 = static_cast<FCylinder*>(cell2.getFigure())->getHeight() * 0.5;
    double r_sum = square(r1 + pow(square(r2) + square(h2), 0.5));
    // 1
    if (r > r_sum) { 
        return false;
    }
    // 2 Transform
    Vector3d transf = cell2.getRotate();
    dCoord c2A(0.0, 0.0, h2);
    Vector3d N = Vector3d::Oy();
    c2A.Rotate(transf.x, Vector3d::Ox());
    N.Rotate  (transf.x, Vector3d::Ox());
    c2A.Rotate(transf.y, N);
    dCoord c2B = c2d - c2A;
    c2A = c2d + c2A;
    Vector3d A1 = c1  - c2A;
    Vector3d B1 = c1  - c2B;
    Vector3d AB = c2B - c2A;
    double base = 2 * h2;//pow(AB.x * AB.x + AB.y * AB.y + AB.z * AB.z, 0.5);
    double baseA1 = A1.Length();
    double baseB1 = B1.Length();
    //double cosA = dCoord::cosA(c1, c2A, c2B);
    //double cosB = dCoord::cosA(c1, c2B, c2A);
    double cosA =  (A1.x * AB.x + A1.y * AB.y + A1.z * AB.z) / (baseA1 * base);
    double cosB = -(B1.x * AB.x + B1.y * AB.y + B1.z * AB.z) / (baseB1 * base);
    if (cosA >= 0 && cosB >= 0) {
        //double S = 0.5 * base * pow(1 - cosA * cosA, 0.5) * baseA1;
        //double SS= 0.5 * base * (r1 + r2);
        return ((r1 + r2) > pow(1 - cosA * cosA, 0.5) * baseA1);
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
            double A = pow(r2 * r2 + baseA1 * baseA1 - 2 * r2 * baseA1 * cosA, 0.5);
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
            double B = pow(r2 * r2 + baseB1 * baseB1 - 2 * r2 * baseB1 * cosB , 0.5);
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
    result = (((r1 + r2) > pow(square(diff.x) + square(diff.y), 0.5)) && ((r1 + h2) > abs(diff.z)));
    if (abs(diff.z) <= h2) {
        return !((square(diff.x) + square(diff.y)) - square(r1 + r2) > EPS);
    } else {
        return ((r1 + h2) - abs(diff.z) > EPS);
    }*/
}

bool CField::is_overlap_cylinder_cylinder(const CCell& cell1, const CCell& cell2)
{
    dCoord c1 = cell1.getCoord();
    dCoord c2 = cell2.getCoord();
    dCoord diff = Diff(c1, c2);
    dCoord c2d = c1 + diff;
    double r1 = cell1.getFigure()->getRadius();
    double h1 = static_cast<FCylinder*>(cell1.getFigure())->getHeight() * 0.5;
    double r2 = cell2.getFigure()->getRadius();
    double h2 = static_cast<FCylinder*>(cell2.getFigure())->getHeight() * 0.5;
    // 2
    Vector3d transf1 = cell1.getRotate();
    Vector3d transf2 = cell2.getRotate();
    
    dCoord c1A(0.0, 0.0, h1);
    Vector3d Ox1 = Vector3d::Ox();//(1.0, 0.0, 0.0);
    Vector3d Oy1 = Vector3d::Oy();//(0.0, 1.0, 0.0);
    Vector3d Oz1 = Vector3d::Oz();//(0.0, 0.0, 1.0);
    c1A.Rotate(transf1.x, Ox1);
    Oy1.Rotate(transf1.x, Ox1);
    Oz1.Rotate(transf1.x, Ox1);
    c1A.Rotate(transf1.y, Oy1);
    Ox1.Rotate(transf1.y, Oy1);
    Oz1.Rotate(transf1.y, Oy1);
    // Ox1, Oy1, Oz1 - оси цилиндра1 после поворота
    
    dCoord c1B = c1 - c1A;
    c1A = c1 + c1A;
    
    dCoord c2A(0.0, 0.0, h2);
    Vector3d Ox2 = Vector3d::Ox();
    Vector3d Oy2 = Vector3d::Oy();
    Vector3d Oz2 = Vector3d::Oz();
    c2A.Rotate(transf2.x, Ox2);
    Oy2.Rotate(transf2.x, Ox2);
    Oz2.Rotate(transf2.x, Ox2);
    c2A.Rotate(transf2.y, Oy2);
    Ox2.Rotate(transf2.y, Oy2);
    Oz2.Rotate(transf2.y, Oy2);
    // Ox2, Oy2, Oz2 - оси цилиндра2 после поворота
    
    dCoord c2B = c2d - c2A;
    c2A = c2d + c2A;
    
    Vector3d norm1 = dCoord::Normal(Ox1, Oy1);
    Vector3d norm2 = dCoord::Normal(Ox2, Oy2);
    
    if (is_overlap_cylinders_point(c1A, c1B, r1, c2A, norm2, r2)) {
        return true;
    }
    if (is_overlap_cylinders_point(c1A, c1B, r1, c2B, norm2, r2)) {
        return true;
    }
    if (is_overlap_cylinders_point(c2A, c2B, r2, c1A, norm1, r1)) {
        return true;
    }
    if (is_overlap_cylinders_point(c2A, c2B, r2, c1B, norm1, r1)) {
        return true;
    }
    //return false;
    Vector3d L1 = c1B - c1A;
    Vector3d L2 = c2B - c2A;
    // c1A - точка на плоскости
    Vector3d S = dCoord::Normal(L1, L2);
    S.Normalize();
    double D  = -dCoord::TermwiseMultiplySum(c1A, S);
    double Smin = (dCoord::TermwiseMultiplySum(c2A, S) + D) / S.Length();
    
    dCoord EA = c1A + S * Smin;

    double t1, t2;
    //dCoord overpoint =
    dCoord::Overlap(L1, L2, EA, c2A, t1, t2);
    if (t2 >= 0 && t2 <= 1 && t1 >= 0 && t1 <= 1) {
        return abs(Smin) <= (r1 + r2);
    }
    return false;
}

bool CField::is_overlap_cylinders_point(const dCoord& base1, const dCoord& base2,
        double r1, const dCoord& other, const Vector3d& area, double r2)
{
    Vector3d C1 = base1 - base2;
    Vector3d L1 = dCoord::Negative(C1);
    
    // плоскость c2A c2B c1A
    Vector3d C2AC1A = other - base1;
    Vector3d SC2ABC1A = dCoord::Normal(C1, C2AC1A);
    SC2ABC1A.Normalize();
    // плоскость у основания c2A
    Vector3d SC2AOC2AB = dCoord::Normal(SC2ABC1A, area);
    SC2AOC2AB.Normalize();
    double t1, t2;
    dCoord op1 = dCoord::Overlap(L1, SC2AOC2AB, base1, other, t1, t2);
    double cosA = dCoord::cosA(base1, op1, other);
    
    if (1 - cosA < 0.1) {
        Vector3d V = other - base1;
        return (V.Length() <= r2);
    }
    
    if (1 + cosA < 0.1) {
        Vector3d V = other - base2;
        return (V.Length() <= r2);
    }
    Vector3d bnorm = base1 - base2;
    bnorm.Normalize();
    double hip = r1 / pow(1 - cosA * cosA, 0.5);
    double kat = hip * cosA;
    
    Vector3d op11 = op1 + bnorm * kat;
    
    Vector3d c2Aop1 = op11 - other;
    Vector3d c1Aop1 = op11 - base1;
    Vector3d c1Bop1 = op11 - base2;
    
    // проверка
    if (hip + r2 >= c2Aop1.Length() && (c1Aop1.x * c1Bop1.x <= 0 && 
            c1Aop1.y * c1Bop1.y <= 0 && c1Aop1.z * c1Bop1.z <= 0)) {
        return true;
    }
    return false;
}

bool CField::is_overlapped(const CCell& cell1, const CCell& cell2)
{
    FigureType t1 = cell1.getFigure()->getType();
    FigureType t2 = cell2.getFigure()->getType();
    if (t1 == fig_sphere && t2 == fig_sphere) {
        return is_overlap_sphere_sphere(cell1, cell2);
    }
    
    if (t1 == fig_sphere && t2 == fig_cylinder) {
        return is_overlap_sphere_cylinder(cell1, cell2);
    }
    
    if (t2 == fig_sphere && t1 == fig_cylinder) {
        return is_overlap_sphere_cylinder(cell2, cell1);
    }
    
    if (t1 == fig_cylinder && t2 == fig_cylinder) {
        return is_overlap_cylinder_cylinder(cell1, cell2);
    }
    std::cerr << "ERROR!!!!" << std::endl;
    return false;
}

bool CField::is_point_overlap_spheres(const CCell& cell)
{
    for (const vcell& vc : clusters) {
        for (const CCell& c : vc) {
            if (is_overlapped(c, cell)) {
                return true;
            }
        }
    }
    return false;
}

dCoord CField::Diff(const dCoord& c1, const dCoord& c2)
{
    dCoord d = c2 - c1;
    dCoord diff;
    if (abs(d.x) < sizes.x - abs(d.x)) {
        diff.x = d.x;
    } else {
        diff.x = -(sizes.x - abs(d.x));
    }
    if (abs(d.y) < sizes.y - abs(d.y)) {
        diff.y = d.y;
    } else {
        diff.y = -(sizes.y - abs(d.y));
    }
    if (abs(d.z) < sizes.z - abs(d.z)) {
        diff.z = d.z;
    } else {
        diff.z = -(sizes.z - abs(d.z));
    }
    return diff;
}

void CField::inPareList(std::vector<vui>& agregate, const Pare& pare)
{
    vui agr;
    vui lagr;
    for (uint i = 0; i < agregate.size(); ++i) {
        for (const uint& ui : agregate[i]) {
            if (pare.a == ui || pare.b == ui) {
                agr.push_back(ui);
                lagr.push_back(i);
            }
        }
    }
    
    switch (lagr.size()) {
        case 0 :
        {
            vui v;
            v.push_back(pare.a);
            v.push_back(pare.b);
            agregate.push_back(v);
        }
            break;
        case 1 :
        {
            // need include 1 cell
            for (const uint& ui : agregate[lagr[0]]) {
                if (pare.a == ui) {
                    agregate[lagr[0]].push_back(pare.b);
                    break;
                }
                if (pare.b == ui) {
                    agregate[lagr[0]].push_back(pare.a);
                    break;
                }
            }
        }
            break;
        case 2 :
            if (lagr[0] == lagr[1]) {
                // both in one cluster
                //cout << " same ";
                break;
            }
            // both in different clusters
            /*agregate[lagr[0]].reserve(agregate[lagr[0]].size() + agregate[lagr[1]].size());
            agregate[lagr[0]].insert(agregate[lagr[0]].end(), agregate[lagr[1]].begin(), agregate[lagr[1]].end());
            agregate[lagr[1]].clear();*/
            while (!agregate[lagr[1]].empty()) {
                agregate[lagr[0]].push_back(agregate[lagr[1]].back());
                agregate[lagr[1]].pop_back();
            }
            /*copy(agregate[lagr[1]].begin(), agregate[lagr[1]].end(), back_inserter(agregate[lagr[0]]));
            agregate[lagr[1]].clear();*/
            // clean empty clusters
            for (uint i = 0; i < agregate.size();) {
                if (agregate[i].empty()) {
                    agregate.erase(agregate.begin() + i);
                } else {
                    ++i;
                }
            }
            break;
    }
}

double CField::leng(const CCell& cell1, const CCell& cell2)
{
    dCoord diff = cell1.getCoord() - cell2.getCoord();
    double r = std::min(square(diff.x), square(sizes.x - abs(diff.x)));
    r += std::min(square(diff.y), square(sizes.y - abs(diff.y)));
    r += std::min(square(diff.z), square(sizes.z - abs(diff.z)));
    return pow(r, 0.5);
}

double CField::quad(double x)
{
    return pow(x, 4.0);
}
