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
    m_field.resize(sizes.x);
    for (size_t x = 0; x < sizes.x; ++x) {
        m_field[x].resize(sizes.y);
        for (size_t y = 0; y < sizes.y; ++y) {
            m_field[x][y].resize(sizes.z, 0);
        }
    }
}

Sizes MxField::sizes() const
{
    return m_sizes;
}

void MxField::initialize(double porosity, double cellsize)
{
    m_cellSize = cellsize;
    // TODO
}

std::vector<Cell> MxField::cells() const
{
    std::vector<Cell> result;
    for (size_t x = 0; x < m_sizes.x; ++x) {
        for (size_t y = 0; y < m_sizes.y; ++y) {
            for (size_t z = 0; z < m_sizes.z; ++z) {
                if (m_field[x][y][z] != 0) {
                    result.push_back(Cell(new FSphere(radius()), dCoord(x, y, z)));
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
    
    for (uint32_t i = 0; i < stepMax; ++i) {
        /*uint32_t rcluster = rand() % clusters.size();
        uint32_t rcell = rand() % clusters[rcluster].size();
        
        double xc = clusters[rcluster][rcell].getCoord().x;
        double yc = clusters[rcluster][rcell].getCoord().y;
        double zc = clusters[rcluster][rcell].getCoord().z;
        double rc = clusters[rcluster][rcell].getRadius();
        
        //spheric!
        double teta = 2.0 * M_PI * (rand() / double(RAND_MAX));
        double phi  = 2.0 * M_PI * (rand() / double(RAND_MAX));
        
        double ixc = xc + (rc + rmin) * sin(teta) * cos(phi);
        double iyc = yc + (rc + rmin) * sin(teta) * sin(phi);
        double izc = zc + (rc + rmin) * cos(teta);
        
        OCell cell(Coord<double>(ixc, iyc, izc), rmin);
        */
        bool overlap = false;
        /*for (size_t ic = 0; ic < clusters.size(); ++ic) {
            for (size_t icc = 0; icc < clusters[ic].size(); ++icc) {
                if (overlap) {
                    break;
                }
                if (icc != rcell || ic != rcluster) {
                    if (is_overlapped(clusters[ic][icc], cell)) {
                        overlap = true;
                    }
                }
            }
        }*/
        if (!overlap) {
            ++positive;
        }
    }
    return positive;
}

void MxField::initDla(uint32_t count)
{
    if (count == 1) {
        m_field[m_sizes[0] >> 1][m_sizes[1] >> 1][m_sizes[2] >> 1] = 1;
    } else {
        uint32_t birthR[3];
        for (uint8_t c = 0; c < 3; ++c) {
            birthR[c] = uint32_t(m_sizes[c] * 0.45 + 0.001);
        }
        for (uint32_t i = 0; i < count;) {
            uint32_t coord[3];
            for (uint8_t c = 0; c < 3; ++c) {
                coord[c] = (m_sizes[c] / 2 - birthR[c] + (rand() % m_sizes[c])) % m_sizes[c];
            }
            if (m_field[coord[0]][coord[1]][coord[2]] != 0) {
                continue;
            } else {
                m_field[coord[0]][coord[1]][coord[2]] = 1;
                ++i;
            }
        }
    }
}

void MxField::toDLA(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    fprintf(out, "%d\t%d\t%d\n", m_sizes.x, m_sizes.y, m_sizes.z);
    for (uint32_t x = 0; x < m_sizes.x; ++x) {
        for (uint32_t y = 0; y < m_sizes.y; ++y) {
            for (uint32_t z = 0; z < m_sizes.z; ++z) {
                if (m_field[x][y][z] != 0) {
                    fprintf(out, "%d\t%d\t%d\n", x, y, z);
                }
            }
        }
    }
    fclose(out);
}

void MxField::toTXT(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    for (uint32_t x = 0; x < m_sizes.x; ++x) {
        for (uint32_t y = 0; y < m_sizes.y; ++y) {
            for (uint32_t z = 0; z < m_sizes.z; ++z) {
                if (m_field[x][y][z] != 0) {
                    fprintf(out, "%d\t%d\t%d\n", x, y, z);
                }
            }
        }
    }
    fclose(out);
}

void MxField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    fwrite(&m_sizes.x, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.y, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.z, sizeof(uint32_t), 1, out);
    for (uint32_t x = 0; x < m_sizes.x; ++x) {
        for (uint32_t y = 0; y < m_sizes.y; ++y) {
            for (uint32_t z = 0; z < m_sizes.z; ++z) {
                fwrite(&m_field[x][y][z], sizeof(uint8_t), 1, out);
            }
        }
    }
    fclose(out);
}

void MxField::fromDLA(const char* fileName)
{
    FILE* in = fopen(fileName, "r");
    uint32_t dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    m_sizes = Sizes(dx, dy, dz);
    m_field.clear();
    m_field.resize(m_sizes.x);
    for (size_t x = 0; x < m_sizes.x; ++x) {
        m_field[x].resize(m_sizes.y);
        for (size_t y = 0; y < m_sizes.y; ++y) {
            m_field[x][y].resize(m_sizes.z, 0);
        }
    }
    uint32_t fx, fy, fz;
    // load structure
    while (fscanf(in, "%d\t%d\t%d\n", &fx, &fy, &fz) == 3) {
        m_field[fx][fy][fz] = 1;
    }
    fclose(in);
}

void MxField::fromTXT(const char* fileName)
{
    uint32_t dx = 0, dy = 0, dz = 0;
    FILE* in1 = fopen(fileName, "r");
    uint32_t fx, fy, fz;
    while (fscanf(in1, "%d\t%d\t%d\n", &fx, &fy, &fz) == 3) {
        if (dx < fx) dx = uint32_t(fx + 1);
        if (dy < fy) dy = uint32_t(fy + 1);
        if (dz < fz) dz = uint32_t(fz + 1);
    }
    fclose(in1);
    
    FILE* in2 = fopen(fileName, "r");
    m_sizes = Sizes(dx, dy, dz);
    m_field.clear();
    m_field.resize(m_sizes.x);
    for (size_t x = 0; x < m_sizes.x; ++x) {
        m_field[x].resize(m_sizes.y);
        for (size_t y = 0; y < m_sizes.y; ++y) {
            m_field[x][y].resize(m_sizes.z, 0);
        }
    }
    // load structure
    while (fscanf(in2, "%d\t%d\t%d\n", &fx, &fy, &fz) == 3) {
        m_field[fx][fy][fz] = 1;
    }
    fclose(in2);
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
    m_sizes = Sizes(dx, dy, dz);
    m_field.clear();
    m_field.resize(m_sizes.x);
    for (size_t x = 0; x < m_sizes.x; ++x) {
        m_field[x].resize(m_sizes.y);
        for (size_t y = 0; y < m_sizes.y; ++y) {
            m_field[x][y].resize(m_sizes.z, 0);
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
