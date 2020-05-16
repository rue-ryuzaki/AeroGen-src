#include "field.h"

#include <fstream>
#include <iostream>

namespace osm {
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

void XField::initialize(double porosity, double cellsize)
{
    m_clusters.clear();
    uint32_t gridsize = uint32_t(cellsize);
    Sizes gsizes = gridSizes(m_sizes, gridsize);
    std::vector<std::vector<std::vector<std::vector<XCell> > > > grid;
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

            XCell cell(new FSphere(r), dCoord(x, y, z));
            std::vector<XCell> overlaps = overlapGrid(grid, cell, gsizes);
            //std::vector<XCell> overlaps = overlap_spheres(cell);
            uint32_t cnt = 0;
            size_t idx = size_t(-1);
            for (size_t i = 0; i < overlaps.size(); ++i) {
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
                    std::vector<XCell> overlaps2 = overlapGrid(grid, cell, gsizes);
                    //std::vector<XCell> overlaps2 = overlapSpheres(cell);
                    cnt = 0;
                    for (const auto& oc : overlaps2) {
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

uint32_t XField::monteCarlo(uint32_t stepMax) const
{
    uint32_t positive = 0;

    double rmin = NitroDiameter / 2.0;

    for (uint32_t i = 0; i < stepMax; ++i) {
        size_t rcluster = size_t(rand() % int32_t(m_clusters.size()));
        size_t rcell = size_t(rand() % int32_t(m_clusters[rcluster].size()));

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
                    if (isOverlapped(&m_clusters[ic][icc], &cell, m_isToroid)) {
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

const std::vector<std::vector<XCell> >& XField::clusters() const
{
    return m_clusters;
}

void XField::agregate()
{
    // agregate list
    std::vector<Pare> pares = agregateList(m_clusters);

    agregateClusters(m_clusters, pares);
}

void XField::setClusters(const std::vector<XCell>& cells)
{
    m_clusters.clear();
    for (const auto& cell : cells) {
        if (!cell.mark) {
            m_clusters.push_back({ cell });
        }
    }
    //agregate();
}

void XField::restoreClusters(const std::vector<std::vector<XCell> >& cells)
{
    m_clusters.clear();
    m_clusters.reserve(cells.size());
    m_clusters.insert(m_clusters.end(), cells.begin(), cells.end());
    //agregate();
    std::cout << "currsize:" << m_clusters.size() << std::endl;
}

std::vector<Pare> XField::agregateList(const std::vector<XCell>& cells) const
{
    // agregate list
    std::vector<Pare> pares;
    for (uint32_t i = 0; i < cells.size(); ++i) {
        for (uint32_t j = (i + 1); j < cells.size(); ++j) {
            if (isOverlapped(&cells[i], &cells[j], m_isToroid)) {
                pares.push_back(Pare(i, j));
            }
        }
    }
    return pares;
}

double XField::getVolumeAG(const std::vector<XCell>& cells) const
{
    double volume = 0.0;
    for (const auto& cell : cells) {
        volume += cell.figure()->volume();
    }
    volume -= overlapVolume(cells);
    return volume;
}

double XField::overlapVolume(const std::vector<XCell>& cells) const
{
    double volume = 0.0;
    for (size_t i = 0; i < cells.size(); ++i) {
        for (size_t j = (i + 1); j < cells.size(); ++j) {
            volume += overlapVolumeSphSph(&cells[i], &cells[j]);
        }
    }
    return volume;
}

double XField::overlapVolumeCells(const XCell& cell1, const XCell& cell2) const
{
    return overlapVolumeSphSph(&cell1, &cell2);
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

void XField::cleanClusters()
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

std::vector<Pare> XField::agregateList(const std::vector<std::vector<XCell> >& cl) const
{
    // agregate list
    std::vector<Pare> pares;
    for (uint32_t i = 0; i < cl.size(); ++i) {
        for (const auto& cell1 : cl[i]) {
            for (uint32_t j = (i + 1); j < cl.size(); ++j) {
                for (const auto& cell2 : cl[j]) {
                    if (isOverlapped(&cell1, &cell2, m_isToroid)) {
                        pares.push_back(Pare(i, j));
                        break;
                    }
                }
            }
        }
    }
    return pares;
}

bool XField::isPointOverlapSpheres(const XCell& cell) const
{
    for (const auto& vc : m_clusters) {
        for (const auto& c : vc) {
            if (isOverlapped(&c, &cell, m_isToroid)) {
                return true;
            }
        }
    }
    return false;
}

std::vector<XCell> XField::overlapSpheres(const XCell& cell) const
{
    std::vector<XCell> result;
    for (const auto& vc : m_clusters) {
        for (const auto& c : vc) {
            if (isOverlapped(&c, &cell, m_isToroid)) {
                result.push_back(c);
            }
        }
    }
    return result;
}

std::vector<XCell> XField::overlapGrid(std::vector<std::vector<std::vector<std::vector<XCell> > > >& grid,
                                       const XCell& cell, const Sizes& gsizes) const
{
    std::vector<XCell> result;
    int32_t x = int32_t(cell.coord().x * gsizes.x / m_sizes.x);
    int32_t y = int32_t(cell.coord().y * gsizes.y / m_sizes.y);
    int32_t z = int32_t(cell.coord().z * gsizes.z / m_sizes.z);
    for (int32_t ix = x - 1; ix < x + 2; ++ix) {
        for (int32_t iy = y - 1; iy < y + 2; ++iy) {
            for (int32_t iz = z - 1; iz < z + 2; ++iz) {
                for (const auto& c : grid[uint32_t(ix + int32_t(gsizes.x)) % gsizes.x]
                        [uint32_t(iy + int32_t(gsizes.y)) % gsizes.y]
                        [uint32_t(iz + int32_t(gsizes.z)) % gsizes.z]) {
                    if (isOverlapped(&c, &cell, m_isToroid)) {
                        result.push_back(c);
                    }
                }
            }
        }
    }
    return result;
}
} // osm
