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
    return m_sizes;
}

void MxField::initialize(double porosity, double cellsize)
{
}

std::vector<Cell> MxField::cells() const
{
    std::vector<Cell> result;
    /*for (const ocell& vc : clusters) {
        for (const OCell& c : vc) {
            result.push_back(c);
        }
    }*/
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
        double teta = 2 * M_PI * (rand() / double(RAND_MAX));
        double phi  = 2 * M_PI * (rand() / double(RAND_MAX));
        
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

void MxField::toDLA(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    uint32_t dx = m_sizes.x;
    uint32_t dy = m_sizes.y;
    uint32_t dz = m_sizes.z;
    fprintf(out, "%d\t%d\t%d\n", dx, dy, dz);
    /*for (const ocell& vc : clusters) {
        for (const OCell& cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getRadius());
        }
    }*/
    fclose(out);
}

void MxField::toTXT(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    /*for (const ocell& vc : clusters) {
        for (const OCell& cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getRadius());
        }
    }*/
    fclose(out);
}

void MxField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    fwrite(&m_sizes.x, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.y, sizeof(uint32_t), 1, out);
    fwrite(&m_sizes.z, sizeof(uint32_t), 1, out);
    /*for (const ocell& vc : clusters) {
        for (const OCell& cell : vc) {
            double x = cell.getCoord().x;
            double y = cell.getCoord().y;
            double z = cell.getCoord().z;
            double r = cell.getRadius();
            fwrite(&x, sizeof(double), 1, out);
            fwrite(&y, sizeof(double), 1, out);
            fwrite(&z, sizeof(double), 1, out);
            fwrite(&r, sizeof(double), 1, out);
        }
    }*/
    fclose(out);
}

void MxField::fromDLA(const char* fileName)
{
    FILE* in = fopen(fileName, "r");
    uint32_t dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    m_sizes = Sizes(dx, dy, dz);
    double fx, fy, fz, fr;
    // load structure
    /*while (fscanf(in, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        ocell vc;
        vc.push_back(OCell(Coord<double>(fx, fy, fz), fr));
        clusters.push_back(vc);
    }*/
    fclose(in);
    //Agregate(clusters);
}

void MxField::fromTXT(const char* fileName)
{
    uint32_t dx = 0, dy = 0, dz = 0;
    FILE* in1 = fopen(fileName, "r");
    double fx, fy, fz, fr;
    while (fscanf(in1, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        if (dx < fx + fr) dx = uint32_t(fx + fr + 1);
        if (dy < fy + fr) dy = uint32_t(fy + fr + 1);
        if (dz < fz + fr) dz = uint32_t(fz + fr + 1);
    }
    fclose(in1);
    
    FILE* in2 = fopen(fileName, "r");
    m_sizes = Sizes(dx, dy, dz);
    // load structure
    /*while (fscanf(in2, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        ocell vc;
        vc.push_back(OCell(Coord<double>(fx, fy, fz), fr));
        clusters.push_back(vc);
    }*/
    fclose(in2);
    //Agregate(clusters);
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
    sc -= sizeof(uint32_t) * 3;
    uint32_t total = sc / sizeof(double);
    double f[total];
    // load structure
    fread(&f, sizeof(double), total, loadFile);
    
    /*for (uint32_t i = 0; i < total; i += 4) {
        ocell vc;
        vc.push_back(OCell(Coord<double>(f[i], f[i + 1], f[i + 2]), f[i + 3]));
        clusters.push_back(vc);
    }*/
    
    fclose(loadFile);
    //Agregate(clusters);
}
