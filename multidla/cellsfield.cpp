#include "cellsfield.h"

#include <iostream>
#include <fstream>

size_t GetElementsFromSize(const MCoord & size) {
    size_t total = 1;
    for (size_t i = 0; i < MCoord::GetDefDims(); ++i) {
        total *= size.GetCoord(i);
    }
    return total;
}

CellsField::CellsField(const char * fileName, txt_format format) : cellSize(2.0) {
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

CellsField::CellsField(const MCoord & size, const MCoord & UpperCorner, double cellSize):
        // where size - size of field
        // UpperCorner - upper left corner coordinate
        mNullPnt(UpperCorner), mSize(size), cellSize(cellSize) {
    size_t total = GetElementsFromSize(size);
    mCells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        mCells[p] = FREE_CELL;
    }
}

FieldElement CellsField::GetElement(const MCoord & c) const {
    // relative coordinate
    if (IsElementInField(c)) {
        Coordinate absCoord = CoordToAbs(c);
        FieldElement res = mCells[absCoord];
        return res;
    }
    throw MOutOfBoundError();
}

vector<Cell> CellsField::getCells() const {
    vector<Cell> result;
    int dx = mSize.GetCoord(0);
    int dy = mSize.GetCoord(1);
    int dz = mSize.GetCoord(2);
    for (int ix = 0; ix < dx; ++ix) {
        for (int iy = 0; iy < dy; ++iy) {
            for (int iz = 0; iz < dz; ++iz) {
                if (GetElement(MCoord(ix, iy, iz)) != 0) {
                    Figure * sph = new FSphere(0.5 * cellSize);
                    result.push_back(Cell(sph, Coord<double>(ix * getSide(), iy *  getSide(), iz *  getSide())));
                }
            }
        }	
    }
    return result;
}

bool CellsField::IsSet(const MCoord & c) const {
    FieldElement curr = GetElement(c);
    bool res = (curr != FREE_CELL);
    return res;
}

void CellsField::SetElementVal(const MCoord & c, FieldElement val) {
    if (IsElementInField(c)) {
        Coordinate absCoord = CoordToAbs(c);
        mCells[absCoord] = val;
        return;
    }
    throw MOutOfBoundError();
}

FieldElement CellsField::GetElementVal(const MCoord & c) {
    if (IsElementInField(c)) {
        Coordinate absCoord = CoordToAbs(c);
        return mCells[absCoord];
    }
    throw MOutOfBoundError();
}

void CellsField::Clear() {
    size_t total = GetElementsFromSize(mSize);
    for (size_t i = 0; i < total; ++i) {
        mCells[i] = FREE_CELL;
    }
}

Coordinate CellsField::GetTotalElements() const {
    size_t total = GetElementsFromSize(mSize);
    Coordinate res = 0;
    for (size_t i = 0; i < total; ++i) {
        if (mCells[i] == FREE_CELL) {
            ++res;
        }
    }
    return res;
}

Coordinate CellsField::CoordToAbs(const MCoord & c) const {
    MCoord correctedC = c - this->mNullPnt;
    Coordinate res = 0;
    int sizeMul = 1;
    // result == X + Y * MaxX + Z * MaxX * MaxY
    for (size_t i = 0; i < MCoord::GetDefDims(); ++i) {
        res += correctedC.GetCoord(i) * sizeMul;
        sizeMul *= mSize.GetCoord(i);
    }
    return res;
}

bool CellsField::IsElementInField(const MCoord & c) const {
    bool res = true;
    for (size_t i = 0; i < MCoord::GetDefDims(); ++i) {
        Coordinate currCord = c.GetCoord(i);
        Coordinate leftC = this->mNullPnt.GetCoord(i);
        Coordinate rigthC = leftC + this->mSize.GetCoord(i);
        if (currCord < leftC || rigthC <= currCord) {
            res = false;
            break;
        }
    }
    return res;
}

void CellsField::Fill(FieldElement val) {
    Coordinate total = GetTotalElements();
    for (int pnt = 0; pnt < total; ++pnt) {
        this->mCells[pnt] = val;
    }
}

Coordinate CellsField::GetCellsCnt() const {
    // returns total amount of cells in field
    return (Coordinate)GetElementsFromSize(this->mSize);
}

void CellsField::toDAT(const char * fileName) const {
    FILE * out = fopen(fileName, "wb+");
    Coordinate total = (Coordinate)GetElementsFromSize(mSize);
    fwrite(this->mCells, sizeof(FieldElement), total, out);
    fclose(out);
}

void CellsField::toDLA(const char * fileName) const {
    FILE * out = fopen(fileName, "w");
    int dx = GetSize().GetCoord(0);
    int dy = GetSize().GetCoord(1);
    int dz = GetSize().GetCoord(2);
    fprintf(out, "%d\t%d\t%d\n", dx, dy, dz);
    for (int ix = 0; ix < dx; ++ix) {
        for (int iy = 0; iy < dy; ++iy) {
            for (int iz = 0; iz < dz; ++iz) {
                if (GetElement(MCoord(ix, iy, iz)) != 0) {
                    fprintf(out, "%d\t%d\t%d\n", ix, iy, iz);
                }
            }
        }	
    }
    fclose(out);
}

void CellsField::toTXT(const char * fileName) const {
    FILE * out = fopen(fileName, "w");
    for (int ix = 0; ix < GetSize().GetCoord(0); ++ix) {
        for (int iy = 0; iy < GetSize().GetCoord(1); ++iy) {
            for (int iz = 0; iz < GetSize().GetCoord(2); ++iz) {
                if (GetElement(MCoord(ix, iy, iz)) != 0) {
                    fprintf(out, "%d\t%d\t%d\n", ix, iy, iz);
                }
            }
        }	
    }
    fclose(out);
}

void CellsField::fromDLA(const char * fileName) {
    FILE * in = fopen(fileName, "r");
    int dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    //MCoord::SetDefDims(3);
    mSize = MCoord(dx, dy, dz);
    mNullPnt = MCoord();
    size_t total = GetElementsFromSize(mSize);
    mCells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        mCells[p] = FREE_CELL;
    }
    
    // load structure
    while (fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz) == 3) {
        //cout << dx << " " << dy << " " << dz << endl;
        SetElementVal(MCoord(dx, dy, dz), OCUPIED_CELL);
    }
    fclose(in);
}

void CellsField::fromTXT(const char * fileName) {
    int x = 0, y = 0, z = 0;
    FILE * in1 = fopen(fileName, "r");
    int dx, dy, dz;
    while (fscanf(in1, "%d\t%d\t%d\n", &dx, &dy, &dz) == 3) {
        if (x < dx) x = dx;
        if (y < dy) y = dy;
        if (z < dz) z = dz;
    }
    fclose(in1);
    
    FILE * in2 = fopen(fileName, "r");
    //MCoord::SetDefDims(3);
    mSize = MCoord(++x, ++y, ++z);
    mNullPnt = MCoord();
    size_t total = GetElementsFromSize(mSize);
    mCells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        mCells[p] = FREE_CELL;
    }
    // load structure
    while (fscanf(in2, "%d\t%d\t%d\n", &dx, &dy, &dz) == 3) {
        SetElementVal(MCoord(dx, dy, dz), OCUPIED_CELL);
    }
    fclose(in2);
}

void CellsField::Resize(MCoord & newSize, MCoord & leftUpperCorner) {
    MCoord oldSize = this->GetSize();
    if (oldSize == newSize) {
        this->Clear();
    } else {
        size_t newTotal = GetElementsFromSize(newSize);
        FieldElement * newCells = new FieldElement[newTotal];

        delete [] this->mCells;
        this->mCells = newCells;

        this->mSize = newSize;
    }
    this->mNullPnt = leftUpperCorner;
}

//void Expand(Coordinate);
//void Expand(Coordinate, Coordinate, Coordinate = 0);

void CellsField::fromDAT(const char * fileName) {
    //MCoord::SetDefDims(3);
    FILE * loadFile = fopen(fileName, "rb+");

    //Define file size:
    fseek(loadFile, 0L, SEEK_END);
    long sc = ftell(loadFile);
    fseek(loadFile, 0L, SEEK_SET);
    long total2 = sc / sizeof(FieldElement);

    FieldElement * cells = new FieldElement[total2];
    fread(cells, sizeof(FieldElement), total2, loadFile);
    fclose(loadFile);

    int b = (pow(total2, 1.0 / 3) + 0.1);
    
    mSize = MCoord(b, b, b);
    mNullPnt = MCoord();
    size_t total = GetElementsFromSize(mSize);
    mCells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        mCells[p] = FREE_CELL;
    }
    
    int index = 0;
    for (int ix = 0; ix < b; ++ix) {
        for (int iy = 0; iy < b; ++iy) {
            for (int iz = 0; iz < b; ++iz) {
                SetElementVal(MCoord(ix, iy, iz), cells[index]);
                ++index;
            }
        }	
    }
}

int CellsField::MonteCarlo(int stepMax) {
    int positive = 0;
    
    double rmin = NitroDiameter / (2 * getSide());
    double rc = r;
    
    for (int i = 0; i < stepMax;) {
        int xm = GetSize().GetCoord(0);
        int ym = GetSize().GetCoord(1);
        int zm = GetSize().GetCoord(2);
        int xc = rand() % xm;
        int yc = rand() % ym;
        int zc = rand() % zm;
        if (GetElement(MCoord(xc, yc, zc)) == OCUPIED_CELL) {
            ++i;
            //spheric!
            double teta = 2 * M_PI * (rand() / (double)RAND_MAX);
            double phi  = 2 * M_PI * (rand() / (double)RAND_MAX);

            double ixc = (double)xc + (rc + rmin) * sin(teta) * cos(phi);
            double iyc = (double)yc + (rc + rmin) * sin(teta) * sin(phi);
            double izc = (double)zc + (rc + rmin) * cos(teta);
            
            //bool overlap = false;
            
            if (xc != 0 && yc != 0 && zc != 0 && is_overlapped(MCoord(xc - 1, yc - 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != 0 && is_overlapped(MCoord(xc - 1, yc - 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && zc != 0 && is_overlapped(MCoord(xc - 1, yc, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != ym - 1 && zc != 0 && is_overlapped(MCoord(xc - 1, yc + 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != ym - 1 && zc != zm - 1 && is_overlapped(MCoord(xc - 1, yc + 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != ym - 1 && is_overlapped(MCoord(xc - 1, yc + 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && zc != zm - 1 && is_overlapped(MCoord(xc - 1, yc, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != 0 && zc != zm - 1 && is_overlapped(MCoord(xc - 1, yc - 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && is_overlapped(MCoord(xc - 1, yc, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            
            
            if (yc != 0 && zc != 0 && is_overlapped(MCoord(xc, yc - 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != 0 && is_overlapped(MCoord(xc, yc - 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (zc != 0 && is_overlapped(MCoord(xc, yc, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != ym - 1 && zc != 0 && is_overlapped(MCoord(xc, yc + 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != ym - 1 && zc != zm - 1 && is_overlapped(MCoord(xc, yc + 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != ym - 1 && is_overlapped(MCoord(xc, yc + 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (zc != zm - 1 && is_overlapped(MCoord(xc, yc, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            
            
            if (xc != xm - 1 && yc != 0 && zc != 0 && is_overlapped(MCoord(xc + 1, yc - 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != 0 && is_overlapped(MCoord(xc + 1, yc - 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && zc != 0 && is_overlapped(MCoord(xc + 1, yc, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != ym - 1 && zc != 0 && is_overlapped(MCoord(xc + 1, yc + 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != ym - 1 && zc != zm - 1 && is_overlapped(MCoord(xc + 1, yc + 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != ym - 1 && is_overlapped(MCoord(xc + 1, yc + 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && zc != zm - 1 && is_overlapped(MCoord(xc + 1, yc, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != 0 && zc != zm - 1 && is_overlapped(MCoord(xc + 1, yc - 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && is_overlapped(MCoord(xc + 1, yc, zc),
                    rc, ixc, iyc, izc, rmin)) {
                continue;
            }
            
            //if (!overlap) {
                ++positive;
            //}
        }
    }
    return positive;
}

bool CellsField::is_overlapped(const MCoord & m1, double r1, double ixc,
        double iyc, double izc, double r2) {
    if (GetElement(m1) == FREE_CELL) {
        return false;
    }
    double diffx = m1.GetCoord(0) - ixc;
    double diffy = m1.GetCoord(1) - iyc;
    double diffz = m1.GetCoord(2) - izc;
    double r = diffx * diffx;
    r += diffy * diffy;
    r += diffz * diffz;
    double r_sum = (r1 + r2) * (r1 + r2);
    return (r_sum - r) > EPS;
}
