#include <iostream>
#include "ofield.h"

OField::OField(const char * fileName, txt_format format) {
    switch (format) {
        case txt_dat:
            fromDAT(fileName);
            break;
        case txt_dla:
            fromDLA(fileName);
            break;
        case txt_txt:
            fromTXT(fileName);
            break;
    }
}

void OField::Initialize(double porosity, double cellsize) {
    int gridsize = int(cellsize);
    clearCells();
    gsizes.x = ((sizes.x % gridsize) == 0 ? (sizes.x / gridsize) : (sizes.x / gridsize + 1));
    gsizes.y = ((sizes.y % gridsize) == 0 ? (sizes.y / gridsize) : (sizes.y / gridsize + 1));
    gsizes.z = ((sizes.z % gridsize) == 0 ? (sizes.z / gridsize) : (sizes.z / gridsize + 1));
    grid = new ocell**[gsizes.x];
    for (int ix = 0; ix < gsizes.x; ++ix) {
        grid[ix] = new ocell*[gsizes.y];
        for (int iy = 0; iy < gsizes.y; ++iy) {
            grid[ix][iy] = new ocell[gsizes.z];
        }
    }
    int Kmax = 666;
    double psi = 0.4;
    double d = cellsize;
    double dmin = Dmin(d, psi);
    double r = d / 2;
    int V = sizes.x * sizes.y * sizes.z;
    double Vpart = VfromR(r);
    int Nmax = (int)((1 - porosity) * V / Vpart);
    int bad = 0;
    int Npart = 0;
    //for (int i = 0; i < 1; ++i) {
        //if (Npart >= Nmax) {
        //    break;
        //}
        while (Npart < Nmax) {
            double x = sizes.x * ((rand()) / (double)RAND_MAX);
            double y = sizes.y * ((rand()) / (double)RAND_MAX);
            double z = sizes.z * ((rand()) / (double)RAND_MAX);
            
            FSphere * sph = new FSphere(r);
            OCell cell(sph, Coord<double>(x, y, z));
            vector<OCell> overlaps = overlap_grid(cell);
            //vector<OCell> overlaps = overlap_spheres(cell);
            int cnt = 0;
            int idx = -1;
            for (uint i = 0; i < overlaps.size(); ++i) {
                if (dmin > leng(cell, overlaps[i])) {
                    ++cnt;
                    idx = i;
                }
            }

            if (cnt == 0) {
                bad = 0;
                AddCell(cell);
                ++Npart;
                continue;
            } else if (cnt == 1) {
                double l = leng(cell, overlaps[idx]);
                if (l != 0) {
                    double alpha = dmin / l;
                    // move cell
                    Coord<double> c1 = cell.getCoord();
                    Coord<double> c2 = overlaps[idx].getCoord();
                    Coord<double> diff = Diff(c1, c2);
                    c1.x = c2.x + diff.x * alpha;
                    c1.y = c2.y + diff.y * alpha;
                    c1.z = c2.z + diff.z * alpha;
                    if (c1.x < 0) {
                        c1.x += sizes.x;
                    }
                    if (c1.y < 0) {
                        c1.y += sizes.y;
                    }
                    if (c1.z < 0) {
                        c1.z += sizes.z;
                    }
                    if (c1.x >= sizes.x) {
                        c1.x -= sizes.x;
                    }
                    if (c1.y >= sizes.y) {
                        c1.y -= sizes.y;
                    }
                    if (c1.z >= sizes.z) {
                        c1.z -= sizes.z;
                    }
                    cell.setCoord(c1);
                    vector<OCell> overlaps2 = overlap_grid(cell);
                    //vector<OCell> overlaps2 = overlap_spheres(cell);
                    cnt = 0;
                    for (const OCell & oc : overlaps2) {
                        if (dmin > leng(cell, oc)) {
                            ++cnt;
                        }
                    }
                    if (cnt == 0) {
                        bad = 0;
                        AddCell(cell);
                        ++Npart;
                        continue;
                    }
                }
            }

            ++bad;

            if (bad >= Kmax) {
                cout << "Too many bad attempts!\n";
                break;
            }
        }
        CleanClusters();
        //ReBuildGrid();
    //}
    delete grid;
}

void OField::AddCell(const OCell & cell) {
    //cells.push_back(cell);
    ocell vc;
    vc.push_back(cell);
    clusters.push_back(vc);
    grid[(int)(cell.getCoord().x / gsizes.x)]
            [(int)(cell.getCoord().y / gsizes.y)]
            [(int)(cell.getCoord().z / gsizes.z)].push_back(cell);
}

void OField::ReBuildGrid() {
    for (int x = 0; x < gsizes.x; ++x) {
        for (int y = 0; y < gsizes.y; ++y) {
            for (int z = 0; z < gsizes.z; ++z) {
                grid[x][y][z].clear();
            }
        }
    }
    for (ocell & vc : clusters) {
        for (OCell & cell : vc) {
            grid[(int)(cell.getCoord().x / gsizes.x)]
                [(int)(cell.getCoord().y / gsizes.y)]
                [(int)(cell.getCoord().z / gsizes.z)].push_back(cell);
        }
    }
}

void OField::CleanClusters() {
    cout << "before agregate " << clusters.size() << endl;
    Agregate(clusters);
    int imax = -1;
    uint smax = 0;
    for (uint i = 0; i < clusters.size(); ++i) {
        if (smax < clusters[i].size()) {
            smax = clusters[i].size();
            imax = i;
        }
    }
    
    for (uint i = 0; i < clusters.size(); ++i) {
        if (i != imax) {
            clusters[i].clear();
        }
    }
    CleanEmptyClusters(clusters);
    cout << "after agregate " << clusters.size() << endl;
}

Coord<double> OField::Diff(Coord<double> c1, Coord<double> c2) {
    Coord<double> d = c1 - c2;
    Coord<double> diff;
    if (abs(d.x) < sizes.x - abs(d.x)) {
        diff.x = (d.x < 0) ? -d.x : d.x;
    } else {
        diff.x = (d.x < 0) ? (sizes.x - abs(d.x)) : -(sizes.x - abs(d.x));
    }
    if (abs(d.y) < sizes.y - abs(d.y)) {
        diff.y = (d.y < 0) ? -d.y : d.y;
    } else {
        diff.y = (d.y < 0) ? (sizes.y - abs(d.y)) : -(sizes.y - abs(d.y));
    }
    if (abs(d.z) < sizes.z - abs(d.z)) {
        diff.z = (d.z < 0) ? -d.z : d.z;
    } else {
        diff.z = (d.z < 0) ? (sizes.z - abs(d.z)) : -(sizes.z - abs(d.z));
    }
    return diff;
}

void OField::setClusters(vector<OCell> & cells) {
    clusters.clear();
    for (OCell & cell : cells) {
        if (!cell.mark) {
            ocell vc;
            vc.push_back(cell);
            clusters.push_back(vc);
        }
    }
    //Agregate(clusters);
}

void OField::restoreClusters(vector<ocell> & cells) {
    clusters.clear();
    for (ocell & vc : cells) {
        clusters.push_back(vc);
    }
    //Agregate(clusters);
    cout << "currsize:" << clusters.size() << endl;
}

double OField::getVolumeAG(const vector<OCell> & varcells) {
    double volume = 0.0;
    for (const OCell & cell : varcells) {
        volume += VfromR(cell.getFigure()->getRadius());
    }
    volume -= overlapVolume(varcells);
    return volume;
}

vector<Pare> OField::AgregateList(vector<ocell>& cl) {
    // agregate list
    vector<Pare> pares;
    
    for (uint i = 0; i < cl.size(); ++i) {
        for (OCell & cell1 : cl[i]) {
            for (uint j = (i + 1); j < cl.size(); ++j) {
                bool overlap = false;
                for (OCell & cell2 : cl[j]) {
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
    return pares;
}

vector<Pare> OField::AgregateList(vector<OCell>& cells) {
    // agregate list
    vector<Pare> pares;
    
    for (uint i = 0; i < cells.size(); ++i) {
        for (uint j = (i + 1); j < cells.size(); ++j) {
            if (is_overlapped(cells[i], cells[j])) {
                pares.push_back(Pare(i, j));
            }
        }
    }
    return pares;
}

vector<Cell> OField::getCells() const {
    vector<Cell> result;
    for (const ocell & vc : clusters) {
        for (const OCell & c : vc) {
            result.push_back(c);
        }
    }
    return result;
}

void OField::Agregate() {
    // agregate list
    vector<Pare> pares = AgregateList(clusters);
    
    vector<vui> agregate;

    for (Pare & p : pares) {
        inPareList(agregate, p);
    }
    
    // check more then 2 cluster agregation!
    
    for (vui & vu : agregate) {
        uint cnt = vu.size();
        
        uint smax = 0;
        uint imax = 0;
        
        for (uint i = 0; i < cnt; ++i) {
            uint ms = clusters[vu[i]].size();
            if (smax < ms) {
                smax = ms;
                imax = i;
            }
        } 

        // agregation in 1 cluster
        for (uint i = 0; i < cnt; ++i) {
            if (i != imax) {
                while (clusters[vu[i]].size() > 0) {
                    clusters[vu[imax]].push_back(clusters[vu[i]].back());
                    clusters[vu[i]].pop_back();
                }
            }
        }
    }
    
    CleanEmptyClusters(clusters);
}

void OField::Agregate(vector<ocell> & cl) {
    // agregate list
    vector<Pare> pares = AgregateList(cl);
    
    vector<vui> agregate;

    for (Pare & p : pares) {
        inPareList(agregate, p);
    }
    
    // check more then 2 cluster agregation!
    
    for (vui & vu : agregate) {
        uint cnt = vu.size();
        
        uint smax = 0;
        uint imax = 0;
        
        for (uint i = 0; i < cnt; ++i) {
            uint ms = cl[vu[i]].size();
            if (smax < ms) {
                smax = ms;
                imax = i;
            }
        } 

        // agregation in 1 cluster
        for (uint i = 0; i < cnt; ++i) {
            if (i != imax) {
                while (cl[vu[i]].size() > 0) {
                    cl[vu[imax]].push_back(cl[vu[i]].back());
                    cl[vu[i]].pop_back();
                }
            }
        }
    }
    
    CleanEmptyClusters(cl);
}

void OField::CleanEmptyClusters(vector<ocell> & cl) {
    // clean empty clusters
    for (uint i = 0; i < cl.size();) {
        if (cl[i].size() == 0) {
            cl.erase(cl.begin() + i);
        } else {
            ++i;
        }
    }
}

void OField::inPareList(vector<vui> & agregate, Pare & pare) {
    vui agr;
    vui lagr;
    for (uint i = 0; i < agregate.size(); ++i) {
        for (uint & ui : agregate[i]) {
            if (pare.a == ui || pare.b == ui) {
                agr.push_back(ui);
                lagr.push_back(i);
            }
        }
    }
    
    switch (lagr.size()) {
        case 0:
        {
            vui v;
            v.push_back(pare.a);
            v.push_back(pare.b);
            agregate.push_back(v);
        }
            break;
        case 1:
        {
            // need include 1 cell
            for (uint & ui : agregate[lagr[0]]) {
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
        case 2:
        {
            if (lagr[0] == lagr[1]) {
                // both in one cluster
                //cout << " same ";
                break;
            }
            // both in different clusters
            while (agregate[lagr[1]].size() > 0) {
                agregate[lagr[0]].push_back(agregate[lagr[1]].back());
                agregate[lagr[1]].pop_back();
            }
            // clean empty clusters
            for (uint i = 0; i < agregate.size();) {
                if (agregate[i].size() == 0) {
                    agregate.erase(agregate.begin() + i);
                } else {
                    ++i;
                }
            }
        }
            break;
    }
}

double OField::leng(const OCell& cell1, const OCell& cell2) {
    Coord<double> c1 = cell1.getCoord();
    Coord<double> c2 = cell2.getCoord();
    Coord<double> diff = c1 - c2;
    double r = min(square(diff.x), square(sizes.x - abs(diff.x)));
    r += min(square(diff.y), square(sizes.y - abs(diff.y)));
    r += min(square(diff.z), square(sizes.z - abs(diff.z)));
    return pow(r, 0.5);
}

double OField::sqleng(const OCell& cell1, const OCell& cell2) {
    Coord<double> c1 = cell1.getCoord();
    Coord<double> c2 = cell2.getCoord();
    Coord<double> diff = c1 - c2;
    double r = min(square(diff.x), square(sizes.x - abs(diff.x)));
    r += min(square(diff.y), square(sizes.y - abs(diff.y)));
    r += min(square(diff.z), square(sizes.z - abs(diff.z)));
    return r;
}

bool OField::is_overlapped(const OCell& cell1, const OCell& cell2) {
    Coord<double> c1 = cell1.getCoord();
    Coord<double> c2 = cell2.getCoord();
    Coord<double> diff = c1 - c2;
    double r = min(square(diff.x), square(sizes.x - abs(diff.x)));
    r += min(square(diff.y), square(sizes.y - abs(diff.y)));
    r += min(square(diff.z), square(sizes.z - abs(diff.z)));
    double r_sum = square(cell1.getFigure()->getRadius() + cell2.getFigure()->getRadius());
    return (r_sum - r) > EPS;
}

bool OField::is_point_overlap_spheres(const OCell& cell) {
    for (const ocell & vc : clusters) {
        for (const OCell & c : vc) {
            if (is_overlapped(c, cell)) {
                return true;
            }
        }
    }
    return false;
}

vector<OCell> OField::overlap_spheres(const OCell& cell) {
    vector<OCell> result;
    for (const ocell & vc : clusters) {
        for (const OCell & c : vc) {
            if (is_overlapped(c, cell)) {
                result.push_back(c);
            }
        }
    }
    return result;
}

vector<OCell> OField::overlap_grid(const OCell& cell) {
    vector<OCell> result;
    int x = (int)(cell.getCoord().x / gsizes.x);
    int y = (int)(cell.getCoord().y / gsizes.y);
    int z = (int)(cell.getCoord().z / gsizes.z);
    
    for (int ix = x - 1; ix < x + 2; ++ix) {
        for (int iy = y - 1; iy < y + 2; ++iy) {
            for (int iz = z - 1; iz < z + 2; ++iz) {
                for (const OCell & c : grid[(ix + gsizes.x) % gsizes.x]
                        [(iy + gsizes.y) % gsizes.y]
                        [(iz + gsizes.z) % gsizes.z]) {
                    if (is_overlapped(c, cell)) {
                        result.push_back(c);
                    }
                }
            }
        }
    }
    return result;
}

void OField::clearCells() {
    clusters.clear();
}

double OField::overlapVolume(const vector<OCell> & cells) {
    double volume = 0.0;
    //for (const ocell & vc : clusters) {
        for (uint i = 0; i < cells.size(); ++i) {
            for (uint j = (i + 1); j < cells.size(); ++j) {
                volume += overlapVolumeCells(cells[i], cells[j]);
            }
        }
    //}
    
    return volume;
}

double OField::overlapVolumeCells(const OCell& cell1, const OCell& cell2) {
    double volume = 0.0;
    Coord<double> c1 = cell1.getCoord();
    Coord<double> c2 = cell2.getCoord();
    Coord<double> diff = c1 - c2;
    double d = min(square(diff.x), square(sizes.x - abs(diff.x)));
    d += min(square(diff.y), square(sizes.y - abs(diff.y)));
    d += min(square(diff.z), square(sizes.z - abs(diff.z)));
    double r1 = cell1.getFigure()->getRadius();
    double r2 = cell2.getFigure()->getRadius();
    double r_sum = square(r1 + r2);
    if ((r_sum - d) > EPS) {
        d = pow(d, 0.5);
        volume = 2 * M_PI * ((r2 * r2 * r2 - (d - r1) * (d - r1) * (d - r1)) / 3
            - ((r2 * r2 * r2 * r2 - (d - r1) * (d - r1) * (d - r1) * (d - r1)) / 4
            + ((d * d - r1 * r1) * (r2 * r2 - (d - r1) * (d - r1))) / 2) / (2 * d));
    }
    return volume;
}

int OField::MonteCarlo(int stepMax) {
    int positive = 0;
    
    double rmin = NitroDiameter / 2;
    
    for (int i = 0; i < stepMax; ++i) {
        uint rcluster = rand() % clusters.size();
        uint rcell = rand() % clusters[rcluster].size();
        
        double xc = clusters[rcluster][rcell].getCoord().x;
        double yc = clusters[rcluster][rcell].getCoord().y;
        double zc = clusters[rcluster][rcell].getCoord().z;
        double rc = clusters[rcluster][rcell].getFigure()->getRadius();
        
        //spheric!
        double teta = 2 * M_PI * (rand() / (double)RAND_MAX);
        double phi  = 2 * M_PI * (rand() / (double)RAND_MAX);
        
        double ixc = xc + (rc + rmin) * sin(teta) * cos(phi);
        double iyc = yc + (rc + rmin) * sin(teta) * sin(phi);
        double izc = zc + (rc + rmin) * cos(teta);
        
        FSphere * sph = new FSphere(rmin);
        OCell cell(sph, Coord<double>(ixc, iyc, izc));
        
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

void OField::toDLA(const char * fileName) const {
    FILE * out = fopen(fileName, "w");
    fprintf(out, "%d\t%d\t%d\n", sizes.x, sizes.y, sizes.z);
    for (const ocell & vc : clusters) {
        for (const OCell & cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getFigure()->getRadius());
        }
    }
    fclose(out);
}

void OField::toTXT(const char * fileName) const {
    FILE * out = fopen(fileName, "w");
    for (const ocell & vc : clusters) {
        for (const OCell & cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getFigure()->getRadius());
        }
    }
    fclose(out);
}

void OField::toDAT(const char * fileName) const {
    FILE * out = fopen(fileName, "wb+");
    fwrite(&sizes.x, sizeof(int), 1, out);
    fwrite(&sizes.y, sizeof(int), 1, out);
    fwrite(&sizes.z, sizeof(int), 1, out);
    for (const ocell & vc : clusters) {
        for (const OCell & cell : vc) {
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

void OField::fromDLA(const char * fileName) {
    FILE * in = fopen(fileName, "r");
    int dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    sizes = Sizes(dx, dy, dz);
    double fx, fy, fz, fr;
    // load structure
    while (fscanf(in, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        ocell vc;
        FSphere * sph = new FSphere(fr);
        vc.push_back(OCell(sph, Coord<double>(fx, fy, fz)));
        clusters.push_back(vc);
    }
    fclose(in);
    Agregate(clusters);
}

void OField::fromTXT(const char * fileName) {
    int dx = 0, dy = 0, dz = 0;
    FILE * in1 = fopen(fileName, "r");
    double fx, fy, fz, fr;
    while (fscanf(in1, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        if (dx < fx + fr) dx = (int)(fx + fr + 1);
        if (dy < fy + fr) dy = (int)(fy + fr + 1);
        if (dz < fz + fr) dz = (int)(fz + fr + 1);
    }
    fclose(in1);
    
    FILE * in2 = fopen(fileName, "r");
    sizes = Sizes(dx, dy, dz);
    // load structure
    while (fscanf(in2, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        ocell vc;
        FSphere * sph = new FSphere(fr);
        vc.push_back(OCell(sph, Coord<double>(fx, fy, fz)));
        clusters.push_back(vc);
    }
    fclose(in2);
    Agregate(clusters);
}

void OField::fromDAT(const char * fileName) {
    FILE * loadFile = fopen(fileName, "rb+");
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
        ocell vc;
        FSphere * sph = new FSphere(f[i + 3]);
        vc.push_back(OCell(sph, Coord<double>(f[i], f[i + 1], f[i + 2])));
        clusters.push_back(vc);
    }
    
    fclose(loadFile);
    Agregate(clusters);
}