#include "ofield.h"

#include <fstream>
#include <iostream>

OField::OField(const char* fileName, txt_format format)
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

OField::OField(const Sizes& sizes)
    : Field(sizes),
      FlexibleField()
{
}

Sizes OField::sizes() const
{
    return m_sizes;
}

std::vector<Cell> OField::cells() const
{
    std::vector<Cell> result;
    for (const std::vector<OCell>& vc : m_clusters) {
        result.reserve(result.size() + vc.size());
        result.insert(result.end(), vc.begin(), vc.end());
    }
    return result;
}

const std::vector<ocell>& OField::clusters() const
{
    return m_clusters;
}

void OField::initialize(double porosity, double cellsize)
{
    m_clusters.clear();
    uint32_t gridsize = uint32_t(cellsize);
    Sizes gsizes = gridSizes(m_sizes, gridsize);
    std::vector<std::vector<std::vector<ocell> > > grid;
    grid.resize(gsizes.x);
    for (uint32_t ix = 0; ix < gsizes.x; ++ix) {
        grid[ix].resize(gsizes.y);
        for (uint32_t iy = 0; iy < gsizes.y; ++iy) {
            grid[ix][iy].resize(gsizes.z);
        }
    }
    uint32_t Kmax = 666;
    double psi = 0.4;
    double dmin = Dmin(cellsize, psi);
    double r = cellsize / 2.0;
    double Vpart = VfromR(r);
    uint32_t Nmax = uint32_t((1.0 - porosity) * m_sizes.volume() / Vpart);
    uint32_t bad = 0;
    uint32_t Npart = 0;
    //for (uint32_t i = 0; i < 1; ++i) {
        //if (Npart >= Nmax) {
        //    break;
        //}
        while (Npart < Nmax) {
            double x = m_sizes.x * ((rand()) / double(RAND_MAX));
            double y = m_sizes.y * ((rand()) / double(RAND_MAX));
            double z = m_sizes.z * ((rand()) / double(RAND_MAX));

            OCell cell(new FSphere(r), dCoord(x, y, z));
            std::vector<OCell> overlaps = overlapGrid(grid, cell, gsizes);
            //std::vector<OCell> overlaps = overlap_spheres(cell);
            uint32_t cnt = 0;
            int32_t idx = -1;
            for (int32_t i = 0; i < int32_t(overlaps.size()); ++i) {
                if (dmin > leng(&cell, &overlaps[i])) {
                    ++cnt;
                    idx = i;
                }
            }

            if (cnt == 0) {
                bad = 0;
                addCell(grid, cell, m_sizes, gsizes);
                ++Npart;
                continue;
            } else if (cnt == 1) {
                double l = leng(&cell, &overlaps[idx]);
                if (l != 0) {
                    double alpha = dmin / l;
                    // move cell
                    dCoord c1 = cell.coord();
                    dCoord c2 = overlaps[idx].coord();
                    dCoord dif = diff(c1, c2);
                    c1.x = c2.x + dif.x * alpha;
                    c1.y = c2.y + dif.y * alpha;
                    c1.z = c2.z + dif.z * alpha;
                    if (c1.x < 0) {
                        c1.x += m_sizes.x;
                    }
                    if (c1.y < 0) {
                        c1.y += m_sizes.y;
                    }
                    if (c1.z < 0) {
                        c1.z += m_sizes.z;
                    }
                    if (c1.x >= m_sizes.x) {
                        c1.x -= m_sizes.x;
                    }
                    if (c1.y >= m_sizes.y) {
                        c1.y -= m_sizes.y;
                    }
                    if (c1.z >= m_sizes.z) {
                        c1.z -= m_sizes.z;
                    }
                    cell.setCoord(c1);
                    std::vector<OCell> overlaps2 = overlapGrid(grid, cell, gsizes);
                    //std::vector<OCell> overlaps2 = overlap_spheres(cell);
                    cnt = 0;
                    for (const OCell& oc : overlaps2) {
                        if (dmin > leng(&cell, &oc)) {
                            ++cnt;
                        }
                    }
                    if (cnt == 0) {
                        bad = 0;
                        addCell(grid, cell, m_sizes, gsizes);
                        ++Npart;
                        continue;
                    }
                }
            }

            ++bad;

            if (bad >= Kmax) {
                std::cout << "Too many bad attempts!\n";
                break;
            }
        }
        cleanClusters();
        //reBuildGrid();
    //}
}

uint32_t OField::monteCarlo(uint32_t stepMax) const
{
    uint32_t positive = 0;

    double rmin = NitroDiameter / 2.0;

    for (uint32_t i = 0; i < stepMax; ++i) {
        uint32_t rcluster = rand() % uint32_t(m_clusters.size());
        uint32_t rcell = rand() % uint32_t(m_clusters[rcluster].size());

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

        OCell cell(new FSphere(rmin), dCoord(ixc, iyc, izc));

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

void OField::agregate()
{
    // agregate list
    std::vector<Pare> pares = agregateList(m_clusters);

    agregateClusters(m_clusters, pares);
}

void OField::setClusters(const std::vector<OCell>& cells)
{
    m_clusters.clear();
    for (const OCell& cell : cells) {
        if (!cell.mark) {
            m_clusters.push_back({ cell });
        }
    }
    //agregate();
}

void OField::restoreClusters(const std::vector<ocell>& cells)
{
    m_clusters.clear();
    m_clusters.reserve(cells.size());
    m_clusters.insert(m_clusters.end(), cells.begin(), cells.end());
    //agregate();
    std::cout << "currsize:" << m_clusters.size() << std::endl;
}

std::vector<Pare> OField::agregateList(const std::vector<OCell>& cells) const
{
    // agregate list
    std::vector<Pare> pares;
    for (uint32_t i = 0; i < cells.size(); ++i) {
        for (uint32_t j = (i + 1); j < cells.size(); ++j) {
            if (isOverlapped(&cells[i], &cells[j])) {
                pares.push_back(Pare(i, j));
            }
        }
    }
    return pares;
}

double OField::getVolumeAG(const std::vector<OCell>& cells) const
{
    double volume = 0.0;
    for (const OCell& cell : cells) {
        volume += VfromR(cell.figure()->radius());
    }
    volume -= overlapVolume(cells);
    return volume;
}

double OField::overlapVolume(const std::vector<OCell>& cells) const
{
    double volume = 0.0;
    for (size_t i = 0; i < cells.size(); ++i) {
        for (size_t j = (i + 1); j < cells.size(); ++j) {
            volume += overlapVolumeCells(cells[i], cells[j]);
        }
    }
    return volume;
}

double OField::overlapVolumeCells(const OCell& cell1, const OCell& cell2) const
{
    dCoord diff = cell1.coord() - cell2.coord();
    double d = std::min(square(diff.x), square(m_sizes.x - std::abs(diff.x)));
    d += std::min(square(diff.y), square(m_sizes.y - std::abs(diff.y)));
    d += std::min(square(diff.z), square(m_sizes.z - std::abs(diff.z)));
    double r1 = cell1.figure()->radius();
    double r2 = cell2.figure()->radius();
    double r_sum = square(r1 + r2);
    if ((r_sum - d) > EPS) {
        d = sqrt(d);
        return 2.0 * M_PI * ((r2 * r2 * r2 - (d - r1) * (d - r1) * (d - r1)) / 3.0
            - ((r2 * r2 * r2 * r2 - (d - r1) * (d - r1) * (d - r1) * (d - r1)) / 4.0
            + ((d * d - r1 * r1) * (r2 * r2 - (d - r1) * (d - r1))) / 2.0) / (2.0 * d));
    }
    return 0.0;
}

void OField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    fwrite(&m_sizes.x, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.y, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.z, sizeof(uint32_t), 1, out);
    for (const std::vector<OCell>& vc : m_clusters) {
        for (const OCell& cell : vc) {
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

void OField::toDLA(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        std::cout << m_sizes.x << "\t" << m_sizes.y << "\t" << m_sizes.z << std::endl;
        for (const std::vector<OCell>& vc : m_clusters) {
            for (const OCell& cell : vc) {
                std::cout << cell.coord().x << "\t" << cell.coord().y << "\t"
                          << cell.coord().z << "\t" << cell.figure()->radius() << std::endl;
            }
        }
        file.close();
    }
}

void OField::toTXT(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        for (const std::vector<OCell>& vc : m_clusters) {
            for (const OCell& cell : vc) {
                std::cout << cell.coord().x << "\t" << cell.coord().y << "\t"
                          << cell.coord().z << "\t" << cell.figure()->radius() << std::endl;
            }
        }
        file.close();
    }
}

void OField::fromDAT(const char* fileName)
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
        std::vector<OCell> vc = { OCell(new FSphere(f[i + 3]), dCoord(f[i], f[i + 1], f[i + 2])) };
        m_clusters.push_back(vc);
    }

    fclose(loadFile);
    agregate();
}

void OField::fromDLA(const char* fileName)
{
    FILE* in = fopen(fileName, "r");
    uint32_t dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    m_sizes = Sizes(dx, dy, dz);
    double fx, fy, fz, fr;
    // load structure
    while (fscanf(in, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        std::vector<OCell> vc = { OCell(new FSphere(fr), dCoord(fx, fy, fz)) };
        m_clusters.push_back(vc);
    }
    fclose(in);
    agregate();
}

void OField::fromTXT(const char* fileName)
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
        std::vector<OCell> vc = { OCell(new FSphere(fr), dCoord(fx, fy, fz)) };
        m_clusters.push_back(vc);
    }
    fclose(in2);
    agregate();
}

void OField::cleanClusters()
{
    std::cout << "before agregate " << m_clusters.size() << std::endl;
    agregate();
    std::pair<size_t, size_t> max = { 0, m_clusters[0].size() };
    for (size_t i = 1; i < m_clusters.size(); ++i) {
        if (max.second < m_clusters[i].size()) {
            m_clusters[max.first].clear();
            max.first = i;
            max.second = m_clusters.size();
        } else {
            m_clusters[i].clear();
        }
    }
    cleanEmptyClusters(m_clusters);
    std::cout << "after agregate " << m_clusters.size() << std::endl;
}

std::vector<Pare> OField::agregateList(const std::vector<ocell>& cl) const
{
    // agregate list
    std::vector<Pare> pares;

    for (uint32_t i = 0; i < cl.size(); ++i) {
        for (const OCell& cell1 : cl[i]) {
            for (uint32_t j = (i + 1); j < cl.size(); ++j) {
                for (const OCell& cell2 : cl[j]) {
                    if (isOverlapped(&cell1, &cell2)) {
                        pares.push_back(Pare(i, j));
                        break;
                    }
                }
            }
        }
    }
    return pares;
}

bool OField::isPointOverlapSpheres(const OCell& cell) const
{
    for (const std::vector<OCell>& vc : m_clusters) {
        for (const OCell& c : vc) {
            if (isOverlapped(&c, &cell)) {
                return true;
            }
        }
    }
    return false;
}

std::vector<OCell> OField::overlapSpheres(const OCell& cell) const
{
    std::vector<OCell> result;
    for (const std::vector<OCell>& vc : m_clusters) {
        for (const OCell& c : vc) {
            if (isOverlapped(&c, &cell)) {
                result.push_back(c);
            }
        }
    }
    return result;
}

std::vector<OCell> OField::overlapGrid(std::vector<std::vector<std::vector<ocell> > >& grid,
                                       const OCell& cell, const Sizes& gsizes) const
{
    std::vector<OCell> result;
    int32_t x = int32_t(cell.coord().x * gsizes.x / m_sizes.x);
    int32_t y = int32_t(cell.coord().y * gsizes.y / m_sizes.y);
    int32_t z = int32_t(cell.coord().z * gsizes.z / m_sizes.z);
    
    for (int32_t ix = x - 1; ix < x + 2; ++ix) {
        for (int32_t iy = y - 1; iy < y + 2; ++iy) {
            for (int32_t iz = z - 1; iz < z + 2; ++iz) {
                for (const OCell& c : grid[(ix + gsizes.x) % gsizes.x]
                        [(iy + gsizes.y) % gsizes.y]
                        [(iz + gsizes.z) % gsizes.z]) {
                    if (isOverlapped(&c, &cell)) {
                        result.push_back(c);
                    }
                }
            }
        }
    }
    return result;
}
