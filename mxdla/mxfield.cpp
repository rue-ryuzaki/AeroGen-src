#include "mxfield.h"

#include <fstream>
#include <iostream>
#include <vector>

MxField::MxField(const char* fileName, txt_format format)
    : Field(fileName, format)
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

MxField::MxField(Sizes sizes)
    : Field(sizes)
{
}

Sizes MxField::sizes() const
{
    return Sizes(m_sides.x * m_cellSize, m_sides.y * m_cellSize, m_sides.z * m_cellSize);
}

void MxField::initialize(double porosity, double cellsize)
{
    m_cellSize = cellsize;
    m_sides.x = m_sizes.x / cellsize;
    m_sides.y = m_sizes.y / cellsize;
    m_sides.z = m_sizes.z / cellsize;
    m_field.resize(m_sides.x);
    for (size_t x = 0; x < m_sides.x; ++x) {
        m_field[x].resize(m_sides.y);
        for (size_t y = 0; y < m_sides.y; ++y) {
            m_field[x][y].resize(m_sides.z, 0);
        }
    }
}

std::vector<Cell> MxField::cells() const
{
    std::vector<Cell> result;
    for (size_t x = 0; x < m_sides.x; ++x) {
        for (size_t y = 0; y < m_sides.y; ++y) {
            for (size_t z = 0; z < m_sides.z; ++z) {
                if (m_field[x][y][z] != 0) {
                    result.push_back(Cell(//new FCube(side()),
                                          new FSphere(radius()),
                                          dCoord(x * side(), y * side(), z * side())));
                }
            }
        }
    }
    return result;
}

uint32_t MxField::monteCarlo(uint32_t stepMax)
{
    uint32_t positive = 0;
    
    double rmin = NitroDiameter / 2.0;
    std::vector<Cell> clusters = cells();
    
    for (uint32_t i = 0; i < stepMax; ++i) {
        uint32_t rcluster = rand() % clusters.size();
        const Cell& curr = clusters[rcluster];
        
        double xc = curr.coord().x;
        double yc = curr.coord().y;
        double zc = curr.coord().z;
        double rc = curr.figure()->radius();
        
        //spheric!
        double teta = 2.0 * M_PI * (rand() / double(RAND_MAX));
        double phi  = 2.0 * M_PI * (rand() / double(RAND_MAX));
        
        double ixc = xc + (rc + rmin) * sin(teta) * cos(phi);
        double iyc = yc + (rc + rmin) * sin(teta) * sin(phi);
        double izc = zc + (rc + rmin) * cos(teta);
        
        Cell cell(new FSphere(rmin), dCoord(ixc, iyc, izc));

        bool overlap = false;
        for (size_t ic = 0; ic < clusters.size(); ++ic) {
            if (overlap) {
                break;
            }
            if (ic != rcluster) {
                if (is_overlapped(clusters[ic], cell)) {
                    overlap = true;
                }
            }
        }
        if (!overlap) {
            ++positive;
        }
    }
    return positive;
}

void MxField::initDla(double por, uint32_t initial, uint32_t step, uint32_t hit)
{
    uint32_t birthR[3];
    uint32_t deathR[3];
    for (uint8_t i = 0; i < 3; ++i) {
        birthR[i] = uint32_t(m_sides[i] * 0.45 + 0.001);
        deathR[i] = uint32_t(m_sides[i] * 0.49 + 0.001);
    }
    const double cellVolume = VfromD(m_cellSize);
    double allvol = (1.0 - por) * m_sizes.volume();
    double currVol = 0.0;
    if (initial == 1) {
        m_field[m_sides[0] >> 1][m_sides[1] >> 1][m_sides[2] >> 1] = 1;
        currVol += cellVolume;
    } else {
        for (uint32_t i = 0; i < initial;) {
            uint32_t coord[3];
            for (uint8_t c = 0; c < 3; ++c) {
                coord[c] = ((m_sides[c] >> 1) - birthR[c] + (rand() % m_sides[c])) % m_sides[c];
            }
            if (m_field[coord[0]][coord[1]][coord[2]] != 0) {
                continue;
            } else {
                m_field[coord[0]][coord[1]][coord[2]] = 1;
                currVol += cellVolume;
                ++i;
            }
        }
    }
    uint32_t coord[3];
    while (currVol < allvol) {
        for (uint8_t i = 0; i < 3; ++i) {
            coord[i] = rand() % m_sides[i];
        }
        if (!isInside(birthR, coord) || m_field[coord[0]][coord[1]][coord[2]] != 0) {
            continue;
        }
        while (true) {
            uint32_t idx = rand() % 3;
            uint32_t sign = rand() % 2;
            int32_t step = (sign == 0) ? -1 : 1;
            coord[idx] += step;
            if (!isInside(deathR, coord)) {
                break;
            }
            if (m_field[coord[0]][coord[1]][coord[2]] != 0) {
                coord[idx] -= step;
                m_field[coord[0]][coord[1]][coord[2]] = 1;
                currVol += cellVolume;
                break;
            }
        }
    }
}

void MxField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    fwrite(&m_sides.x, sizeof(uint32_t), 1, out);
    fwrite(&m_sides.y, sizeof(uint32_t), 1, out);
    fwrite(&m_sides.z, sizeof(uint32_t), 1, out);
    for (uint32_t x = 0; x < m_sides.x; ++x) {
        for (uint32_t y = 0; y < m_sides.y; ++y) {
            for (uint32_t z = 0; z < m_sides.z; ++z) {
                fwrite(&m_field[x][y][z], sizeof(uint8_t), 1, out);
            }
        }
    }
    fclose(out);
}

void MxField::toDLA(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        std::cout << m_sides.x << "\t" << m_sides.y << "\t" << m_sides.z << std::endl;
        for (uint32_t x = 0; x < m_sides.x; ++x) {
            for (uint32_t y = 0; y < m_sides.y; ++y) {
                for (uint32_t z = 0; z < m_sides.z; ++z) {
                    if (m_field[x][y][z] != 0) {
                        std::cout << x << "\t" << y << "\t" << z << "\t" << uint32_t(radius()) << std::endl;
                    }
                }
            }
        }
        file.close();
    }
}

void MxField::toTXT(const char* fileName) const
{
    std::ofstream file;
    file.open(fileName, std::ios_base::trunc);
    if (file.is_open()) {
        for (uint32_t x = 0; x < m_sides.x; ++x) {
            for (uint32_t y = 0; y < m_sides.y; ++y) {
                for (uint32_t z = 0; z < m_sides.z; ++z) {
                    if (m_field[x][y][z] != 0) {
                        std::cout << x << "\t" << y << "\t" << z << "\t" << uint32_t(radius()) << std::endl;
                    }
                }
            }
        }
        file.close();
    }
}

void MxField::fromDAT(const char* fileName)
{
    FILE* loadFile = fopen(fileName, "rb+");
    //Define file size:
    fseek(loadFile, 0L, SEEK_END);
    uint32_t sc = ftell(loadFile);
    fseek(loadFile, 0L, SEEK_SET);
    uint32_t dx, dy, dz;
    fread(&dx, sizeof(uint32_t), 1, loadFile);
    fread(&dy, sizeof(uint32_t), 1, loadFile);
    fread(&dz, sizeof(uint32_t), 1, loadFile);
    m_sides = Sizes(dx, dy, dz);
    m_field.clear();
    m_field.resize(m_sides.x);
    for (size_t x = 0; x < m_sides.x; ++x) {
        m_field[x].resize(m_sides.y);
        for (size_t y = 0; y < m_sides.y; ++y) {
            m_field[x][y].resize(m_sides.z, 0);
        }
    }
    sc -= sizeof(uint32_t) * 3;
    uint32_t total = sc / sizeof(uint8_t);
    uint8_t f[total];
    // load structure
    fread(&f, sizeof(uint8_t), total, loadFile);
    for (uint32_t i = 0; i < total; ++i) {
//        ocell vc;
//        vc.push_back(OCell(Coord<double>(f[i], f[i + 1], f[i + 2]), f[i + 3]));
//        clusters.push_back(vc);
    }
    fclose(loadFile);
}

void MxField::fromDLA(const char* fileName)
{
    std::fstream file;
    file.open(fileName, std::ios::in);
    if (file.is_open()) {
        uint32_t dx, dy, dz;
        file >> dx;
        file >> dy;
        file >> dz;
        m_sides = Sizes(dx, dy, dz);
        m_field.clear();
        m_field.resize(m_sides.x);
        for (size_t x = 0; x < m_sides.x; ++x) {
            m_field[x].resize(m_sides.y);
            for (size_t y = 0; y < m_sides.y; ++y) {
                m_field[x][y].resize(m_sides.z, 0);
            }
        }
        uint32_t fx, fy, fz, fr;
        do {
            file >> fx;
            file >> fy;
            file >> fz;
            file >> fr;
            m_field[fx][fy][fz] = 1;
            m_cellSize = 2.0 * fr;
        } while (!file.eof());
        file.close();
    }
}

void MxField::fromTXT(const char* fileName)
{
//    std::fstream file;
//    file.open(fileName, std::ios::in);
//    if (file.is_open()) {
//        m_sides = Sizes(150, 150, 150);
//        m_field.clear();
//        m_field.resize(m_sides.x);
//        for (size_t x = 0; x < m_sides.x; ++x) {
//            m_field[x].resize(m_sides.y);
//            for (size_t y = 0; y < m_sides.y; ++y) {
//                m_field[x][y].resize(m_sides.z, 0);
//            }
//        }
//        uint32_t fx, fy, fz, fr;
//        do {
//            file >> fx;
//            file >> fy;
//            file >> fz;
//            file >> fr;
//            m_field[fx][fy][fz] = 1;
//            m_cellSize = 2.0 * fr;
//        } while (!file.eof());
//        file.close();
//    }
    uint32_t dx = 0, dy = 0, dz = 0;
    FILE* in1 = fopen(fileName, "r");
    uint32_t fx, fy, fz, fr;
    while (fscanf(in1, "%d\t%d\t%d\t%d\n", &fx, &fy, &fz, &fr) == 4) {
        if (dx < fx) dx = uint32_t(fx + 1);
        if (dy < fy) dy = uint32_t(fy + 1);
        if (dz < fz) dz = uint32_t(fz + 1);
    }
    fclose(in1);
    
    FILE* in2 = fopen(fileName, "r");
    m_sides = Sizes(dx, dy, dz);
    m_field.clear();
    m_field.resize(m_sides.x);
    for (size_t x = 0; x < m_sides.x; ++x) {
        m_field[x].resize(m_sides.y);
        for (size_t y = 0; y < m_sides.y; ++y) {
            m_field[x][y].resize(m_sides.z, 0);
        }
    }
    // load structure
    while (fscanf(in2, "%d\t%d\t%d\t%d\n", &fx, &fy, &fz, &fr) == 4) {
        m_field[fx][fy][fz] = 1;
        m_cellSize = 2.0 * fr;
    }
        fclose(in2);
}

bool MxField::is_overlapped(const Cell &cell1, const Cell &cell2) const
{
    dCoord diff = cell1.coord() - cell2.coord();
    double r = std::min(square(diff.x), square(m_sizes.x - std::abs(diff.x)));
    r += std::min(square(diff.y), square(m_sizes.y - std::abs(diff.y)));
    r += std::min(square(diff.z), square(m_sizes.z - std::abs(diff.z)));
    double r_sum = square(cell1.figure()->radius() + cell2.figure()->radius());
    return (r_sum - r) > EPS;
}

bool MxField::isInside(uint32_t r[], uint32_t coord[]) const
{
    for (uint8_t i = 0; i < 3; ++i) {
        if (m_sides[i] == 1) {
            continue;
        }
        if (coord[i] < ((m_sides[i] >> 1) - r[i]) || coord[i] >= ((m_sides[i] >> 1) + r[i])) {
            return false;
        }
    }
    return true;
}
