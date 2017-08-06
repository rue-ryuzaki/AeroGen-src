#include "cellsfield.h"

#include <fstream>
#include <iostream>
#include <vector>

size_t elementsFromSize(const MCoord& size)
{
    size_t total = 1;
    for (size_t i = 0; i < MCoord::defDims(); ++i) {
        total *= size.coord(i);
    }
    return total;
}

CellsField::CellsField()
    : m_nullPnt(MCoord()),
      m_size(MCoord()),
      m_cellSize(2.0)
{
}

CellsField::CellsField(const char* fileName, txt_format format)
    : Field(fileName, format),
      m_cellSize(2.0)
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

CellsField::CellsField(const MCoord& size, const MCoord& UpperCorner, double cellSize)
    :
        // where size - size of field
        // UpperCorner - upper left corner coordinate
      m_nullPnt(UpperCorner),
      m_size(size),
      m_cellSize(cellSize)
{
    size_t total = elementsFromSize(size);
    m_cells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        m_cells[p] = FREE_CELL;
    }
}

CellsField::~CellsField()
{
    delete [] m_cells;
}

std::vector<Cell> CellsField::cells() const
{
    std::vector<Cell> result;
    int dx = m_size.coord(0);
    int dy = m_size.coord(1);
    int dz = m_size.coord(2);
    for (int ix = 0; ix < dx; ++ix) {
        for (int iy = 0; iy < dy; ++iy) {
            for (int iz = 0; iz < dz; ++iz) {
                if (element(MCoord(ix, iy, iz)) != 0) {
                    IFigure* sph = new FSphere(0.5 * m_cellSize);
                    result.push_back(Cell(sph, dCoord(ix * side(), iy *  side(), iz *  side())));
                }
            }
        }
    }
    return result;
}

Sizes CellsField::sizes() const
{
    return Sizes(int(m_size.coord(0) * side()),
            int(m_size.coord(1) * side()), int(m_size.coord(2) * side()));
}

void CellsField::initialize(double porosity, double cellsize)
{
}

int CellsField::monteCarlo(int stepMax)
{
    int positive = 0;

    double rmin = NitroDiameter / (2 * side());
    double rc = r;

    for (int i = 0; i < stepMax;) {
        int xm = size().coord(0);
        int ym = size().coord(1);
        int zm = size().coord(2);
        int xc = rand() % xm;
        int yc = rand() % ym;
        int zc = rand() % zm;
        if (element(MCoord(xc, yc, zc)) == OCUPIED_CELL) {
            ++i;
            //spheric!
            double teta = 2 * M_PI * (rand() / double(RAND_MAX));
            double phi  = 2 * M_PI * (rand() / double(RAND_MAX));

            double ixc = double(xc) + (rc + rmin) * sin(teta) * cos(phi);
            double iyc = double(yc) + (rc + rmin) * sin(teta) * sin(phi);
            double izc = double(zc) + (rc + rmin) * cos(teta);

            //bool overlap = false;

            if (xc != 0 && yc != 0 && zc != 0 && isOverlapped(MCoord(xc - 1, yc - 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != 0 && isOverlapped(MCoord(xc - 1, yc - 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && zc != 0 && isOverlapped(MCoord(xc - 1, yc, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != ym - 1 && zc != 0 && isOverlapped(MCoord(xc - 1, yc + 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != ym - 1 && zc != zm - 1 && isOverlapped(MCoord(xc - 1, yc + 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != ym - 1 && isOverlapped(MCoord(xc - 1, yc + 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && zc != zm - 1 && isOverlapped(MCoord(xc - 1, yc, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && yc != 0 && zc != zm - 1 && isOverlapped(MCoord(xc - 1, yc - 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != 0 && isOverlapped(MCoord(xc - 1, yc, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }


            if (yc != 0 && zc != 0 && isOverlapped(MCoord(xc, yc - 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != 0 && isOverlapped(MCoord(xc, yc - 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (zc != 0 && isOverlapped(MCoord(xc, yc, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != ym - 1 && zc != 0 && isOverlapped(MCoord(xc, yc + 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != ym - 1 && zc != zm - 1 && isOverlapped(MCoord(xc, yc + 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (yc != ym - 1 && isOverlapped(MCoord(xc, yc + 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (zc != zm - 1 && isOverlapped(MCoord(xc, yc, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }


            if (xc != xm - 1 && yc != 0 && zc != 0 && isOverlapped(MCoord(xc + 1, yc - 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != 0 && isOverlapped(MCoord(xc + 1, yc - 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && zc != 0 && isOverlapped(MCoord(xc + 1, yc, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != ym - 1 && zc != 0 && isOverlapped(MCoord(xc + 1, yc + 1, zc - 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != ym - 1 && zc != zm - 1 && isOverlapped(MCoord(xc + 1, yc + 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != ym - 1 && isOverlapped(MCoord(xc + 1, yc + 1, zc),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && zc != zm - 1 && isOverlapped(MCoord(xc + 1, yc, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && yc != 0 && zc != zm - 1 && isOverlapped(MCoord(xc + 1, yc - 1, zc + 1),
                    rc, ixc, iyc, izc, rmin)) {
                //overlap = true;
                continue;
            }
            if (xc != xm - 1 && isOverlapped(MCoord(xc + 1, yc, zc),
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

FieldElement CellsField::element(const MCoord& c) const
{
    // relative coordinate
    if (isElementInField(c)) {
        Coordinate absCoord = coordToAbs(c);
        FieldElement res = m_cells[absCoord];
        return res;
    }
    throw MOutOfBoundError();
}

bool CellsField::isSet(const MCoord& c) const
{
    FieldElement curr = element(c);
    bool res = (curr != FREE_CELL);
    return res;
}

void CellsField::setElement(const MCoord& c)
{
    setElementVal(c, OCUPIED_CELL);
}

void CellsField::unSetElement(const MCoord& c)
{
    setElementVal(c, FREE_CELL);
}

void CellsField::setElementVal(const MCoord& c, FieldElement val)
{
    if (isElementInField(c)) {
        Coordinate absCoord = coordToAbs(c);
        m_cells[absCoord] = val;
        return;
    }
    throw MOutOfBoundError();
}

FieldElement CellsField::elementVal(const MCoord& c)
{
    if (isElementInField(c)) {
        Coordinate absCoord = coordToAbs(c);
        return m_cells[absCoord];
    }
    throw MOutOfBoundError();
}

void CellsField::clear()
{
    size_t total = elementsFromSize(m_size);
    for (size_t i = 0; i < total; ++i) {
        m_cells[i] = FREE_CELL;
    }
}

Coordinate CellsField::totalElements() const
{
    size_t total = elementsFromSize(m_size);
    Coordinate res = 0;
    for (size_t i = 0; i < total; ++i) {
        if (m_cells[i] == FREE_CELL) {
            ++res;
        }
    }
    return res;
}

Coordinate CellsField::cellsCnt() const
{
    // returns total amount of cells in field
    return (Coordinate)elementsFromSize(this->m_size);
}

MCoord CellsField::size() const
{
    return m_size;
}

MCoord CellsField::nullPnt() const
{
    return m_nullPnt;
}

size_t CellsField::dims() const
{
    return m_dims;
}

void CellsField::fill(FieldElement val)
{
    Coordinate total = totalElements();
    for (int pnt = 0; pnt < total; ++pnt) {
        this->m_cells[pnt] = val;
    }
}

void CellsField::resize(MCoord& newSize, MCoord& leftUpperCorner)
{
    MCoord oldSize = this->size();
    if (oldSize == newSize) {
        this->clear();
    } else {
        size_t newTotal = elementsFromSize(newSize);
        FieldElement* newCells = new FieldElement[newTotal];

        delete [] this->m_cells;
        this->m_cells = newCells;

        this->m_size = newSize;
    }
    this->m_nullPnt = leftUpperCorner;
}

bool CellsField::isElementInField(const MCoord& c) const
{
    bool res = true;
    for (size_t i = 0; i < MCoord::defDims(); ++i) {
        Coordinate currCord = c.coord(i);
        Coordinate leftC = this->m_nullPnt.coord(i);
        Coordinate rigthC = leftC + this->m_size.coord(i);
        if (currCord < leftC || rigthC <= currCord) {
            res = false;
            break;
        }
    }
    return res;
}

void CellsField::toDAT(const char* fileName) const
{
    FILE* out = fopen(fileName, "wb+");
    Coordinate total = (Coordinate)elementsFromSize(m_size);
    fwrite(this->m_cells, sizeof(FieldElement), total, out);
    fclose(out);
}

void CellsField::toDLA(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    int dx = size().coord(0);
    int dy = size().coord(1);
    int dz = size().coord(2);
    fprintf(out, "%d\t%d\t%d\n", dx, dy, dz);
    for (int ix = 0; ix < dx; ++ix) {
        for (int iy = 0; iy < dy; ++iy) {
            for (int iz = 0; iz < dz; ++iz) {
                if (element(MCoord(ix, iy, iz)) != 0) {
                    fprintf(out, "%d\t%d\t%d\n", ix, iy, iz);
                }
            }
        }	
    }
    fclose(out);
}

void CellsField::toTXT(const char* fileName) const
{
    FILE* out = fopen(fileName, "w");
    for (int ix = 0; ix < size().coord(0); ++ix) {
        for (int iy = 0; iy < size().coord(1); ++iy) {
            for (int iz = 0; iz < size().coord(2); ++iz) {
                if (element(MCoord(ix, iy, iz)) != 0) {
                    fprintf(out, "%d\t%d\t%d\n", ix, iy, iz);
                }
            }
        }	
    }
    fclose(out);
}

void CellsField::fromDAT(const char* fileName)
{
    //MCoord::SetDefDims(3);
    FILE* loadFile = fopen(fileName, "rb+");

    //Define file size:
    fseek(loadFile, 0L, SEEK_END);
    long sc = ftell(loadFile);
    fseek(loadFile, 0L, SEEK_SET);
    long total2 = sc / sizeof(FieldElement);

    FieldElement* cells = new FieldElement[total2];
    fread(cells, sizeof(FieldElement), total2, loadFile);
    fclose(loadFile);

    int b = (pow(total2, 1.0 / 3) + 0.1);

    m_size = MCoord(b, b, b);
    m_nullPnt = MCoord();
    size_t total = elementsFromSize(m_size);
    m_cells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        m_cells[p] = FREE_CELL;
    }

    int index = 0;
    for (int ix = 0; ix < b; ++ix) {
        for (int iy = 0; iy < b; ++iy) {
            for (int iz = 0; iz < b; ++iz) {
                setElementVal(MCoord(ix, iy, iz), cells[index]);
                ++index;
            }
        }
    }
}

void CellsField::fromDLA(const char* fileName)
{
    FILE* in = fopen(fileName, "r");
    int dx, dy, dz;
    fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz);
    //MCoord::SetDefDims(3);
    m_size = MCoord(dx, dy, dz);
    m_nullPnt = MCoord();
    size_t total = elementsFromSize(m_size);
    m_cells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        m_cells[p] = FREE_CELL;
    }
    
    // load structure
    while (fscanf(in, "%d\t%d\t%d\n", &dx, &dy, &dz) == 3) {
        //cout << dx << " " << dy << " " << dz << endl;
        setElementVal(MCoord(dx, dy, dz), OCUPIED_CELL);
    }
    fclose(in);
}

void CellsField::fromTXT(const char* fileName)
{
    int x = 0, y = 0, z = 0;
    FILE* in1 = fopen(fileName, "r");
    int dx, dy, dz;
    while (fscanf(in1, "%d\t%d\t%d\n", &dx, &dy, &dz) == 3) {
        if (x < dx) x = dx;
        if (y < dy) y = dy;
        if (z < dz) z = dz;
    }
    fclose(in1);
    
    FILE* in2 = fopen(fileName, "r");
    //MCoord::SetDefDims(3);
    m_size = MCoord(++x, ++y, ++z);
    m_nullPnt = MCoord();
    size_t total = elementsFromSize(m_size);
    m_cells = new FieldElement[total];
    for (size_t p = 0; p < total; ++p) {
        m_cells[p] = FREE_CELL;
    }
    // load structure
    while (fscanf(in2, "%d\t%d\t%d\n", &dx, &dy, &dz) == 3) {
        setElementVal(MCoord(dx, dy, dz), OCUPIED_CELL);
    }
    fclose(in2);
}

//void Expand(Coordinate);
//void Expand(Coordinate, Coordinate, Coordinate = 0);

bool CellsField::isOverlapped(const MCoord& m1, double r1, double ixc,
                               double iyc, double izc, double r2)
{
    if (element(m1) == FREE_CELL) {
        return false;
    }
    double diffx = m1.coord(0) - ixc;
    double diffy = m1.coord(1) - iyc;
    double diffz = m1.coord(2) - izc;
    double r = diffx * diffx;
    r += diffy * diffy;
    r += diffz * diffz;
    double r_sum = (r1 + r2) * (r1 + r2);
    return (r_sum - r) > EPS;
}

Coordinate CellsField::coordToAbs(const MCoord& c) const
{
    MCoord correctedC = c - this->m_nullPnt;
    Coordinate res = 0;
    int sizeMul = 1;
    // result == X + Y * MaxX + Z * MaxX * MaxY
    for (size_t i = 0; i < MCoord::defDims(); ++i) {
        res += correctedC.coord(i) * sizeMul;
        sizeMul *= m_size.coord(i);
    }
    return res;
}
