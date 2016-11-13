#include "xfield.h"

#include <fstream>
#include <iostream>
#include <vector>

xField::xField(const char * fileName, txt_format format)
{
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

xField::xField(Sizes sizes)
    : Field(sizes)
{
}

Sizes xField::getSizes() const
{
    return sizes;
}

void xField::Initialize(double porosity, double cellsize)
{
}

std::vector<Cell> xField::getCells() const {
    std::vector<Cell> result;
    /*for (const ocell & vc : clusters) {
        for (const OCell & c : vc) {
            result.push_back(c);
        }
    }*/
    return result;
}

int xField::MonteCarlo(int stepMax) {
    int positive = 0;
    
    double rmin = NitroDiameter / 2;
    
    for (int i = 0; i < stepMax; ++i) {
        /*uint rcluster = rand() % clusters.size();
        uint rcell = rand() % clusters[rcluster].size();
        
        double xc = clusters[rcluster][rcell].getCoord().x;
        double yc = clusters[rcluster][rcell].getCoord().y;
        double zc = clusters[rcluster][rcell].getCoord().z;
        double rc = clusters[rcluster][rcell].getRadius();
        
        //spheric!
        double teta = 2 * M_PI * (rand() / (double)RAND_MAX);
        double phi  = 2 * M_PI * (rand() / (double)RAND_MAX);
        
        double ixc = xc + (rc + rmin) * sin(teta) * cos(phi);
        double iyc = yc + (rc + rmin) * sin(teta) * sin(phi);
        double izc = zc + (rc + rmin) * cos(teta);
        
        OCell cell(Coord<double>(ixc, iyc, izc), rmin);
        */
        bool overlap = false;
        /*for (uint ic = 0; ic < clusters.size(); ++ic) {
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
        }*/
        if (!overlap) {
            ++positive;
        }
    }
    return positive;
}

void xField::toDLA(const char * fileName) const
{
    FILE * out = fopen(fileName, "w");
    int dx = sizes.x;
    int dy = sizes.y;
    int dz = sizes.z;
    fprintf(out, "%d\t%d\t%d\n", dx, dy, dz);
    /*for (const ocell & vc : clusters) {
        for (const OCell & cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getRadius());
        }
    }*/
    fclose(out);
}

void xField::toTXT(const char * fileName) const
{
    FILE * out = fopen(fileName, "w");
    /*for (const ocell & vc : clusters) {
        for (const OCell & cell : vc) {
            fprintf(out, "%lf\t%lf\t%lf\t%lf\n", cell.getCoord().x,
                    cell.getCoord().y, cell.getCoord().z, cell.getRadius());
        }
    }*/
    fclose(out);
}

void xField::toDAT(const char * fileName) const
{
    FILE * out = fopen(fileName, "wb+");
    fwrite(&sizes.x, sizeof(int), 1, out);
    fwrite(&sizes.y, sizeof(int), 1, out);
    fwrite(&sizes.z, sizeof(int), 1, out);
    /*for (const ocell & vc : clusters) {
        for (const OCell & cell : vc) {
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

void xField::fromDLA(const char * fileName)
{
    FILE * in = fopen(fileName, "r");
    int dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    sizes = Sizes(dx, dy, dz);
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

void xField::fromTXT(const char * fileName)
{
    int dx = 0, dy = 0, dz = 0;
    FILE * in1 = fopen(fileName, "r");
    double fx, fy, fz, fr;
    while (fscanf(in1, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        if (dx < fx + fr) dx = int(fx + fr + 1);
        if (dy < fy + fr) dy = int(fy + fr + 1);
        if (dz < fz + fr) dz = int(fz + fr + 1);
    }
    fclose(in1);
    
    FILE * in2 = fopen(fileName, "r");
    sizes = Sizes(dx, dy, dz);
    // load structure
    /*while (fscanf(in2, "%lf\t%lf\t%lf\t%lf\n", &fx, &fy, &fz, &fr) == 4) {
        ocell vc;
        vc.push_back(OCell(Coord<double>(fx, fy, fz), fr));
        clusters.push_back(vc);
    }*/
    fclose(in2);
    //Agregate(clusters);
}

void xField::fromDAT(const char * fileName)
{
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
    
    /*for (int i = 0; i < total; i += 4) {
        ocell vc;
        vc.push_back(OCell(Coord<double>(f[i], f[i + 1], f[i + 2]), f[i + 3]));
        clusters.push_back(vc);
    }*/
    
    fclose(loadFile);
    //Agregate(clusters);
}
