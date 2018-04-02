#include "basefield.h"

void Field::toFile(const char* fileName, txt_format format) const
{
    switch (format) {
        case txt_dat :
            toDAT(fileName);
            break;
        case txt_dla :
            toDLA(fileName);
            break;
        case txt_txt :
            toTXT(fileName);
            break;
    }
}

/*Field* Field::fromFile(const char* fileName, txt_format format)
{
    Field* result;
    switch (format) {
        case txt_dat :
            result->fromDAT(fileName);
            break;
        case txt_dla :
            result->fromDLA(fileName);
            break;
        case txt_txt :
            result->fromTXT(fileName);
            break;
    }
    return result;
}*/

double Field::overlapVolumeSphSph(const Cell* cell1, const Cell* cell2, bool isToroid) const
{
    dCoord d = diff(cell1->coord(), cell2->coord(), isToroid);
    double r = sqr(d.x) + sqr(d.y) + sqr(d.z);
//    dCoord diff = cell2->coord() - cell1->coord();
//    double r = std::min(sqr(diff.x), double(sqr(m_sizes.x - abs(diff.x))));
//    r += std::min(sqr(diff.y), double(sqr(m_sizes.y - abs(diff.y))));
//    r += std::min(sqr(diff.z), double(sqr(m_sizes.z - abs(diff.z))));

    double r1 = cell1->figure()->radius();
    double r2 = cell2->figure()->radius();

    if (sqr(r1 + r2) > r) {
        double d = std::sqrt(r);
        return 2.0 * M_PI * ((cube(r2) - cube(d - r1)) / 3.0 - ((quad(r2) - quad(d - r1)) / 4.0
                + ((sqr(d) - sqr(r1)) * (sqr(r2) - sqr(d - r1))) / 2.0) / (2.0 * d));
    }
    return 0.0;
}

double Field::overlapVolumeSphCyl(const Cell* /*cell1*/, const Cell* /*cell2*/, bool /*isToroid*/) const
{
    return 0.0;
}

double Field::overlapVolumeCylCyl(const Cell* /*cell1*/, const Cell* /*cell2*/, bool /*isToroid*/) const
{
    return 0.0;
}

bool Field::isOverlapSphSph(const Cell* cell1, const Cell* cell2, bool isToroid) const
{
    dCoord dif = diff(cell1->coord(), cell2->coord(), isToroid);
    double r = sqr(dif.x) + sqr(dif.y) + sqr(dif.z);
    double r_sum = sqr(cell1->figure()->radius() + cell2->figure()->radius());
    return (r_sum - r) > EPS;
}

bool Field::isOverlapSphCyl(const Cell* cell1, const Cell* cell2, bool isToroid) const
{
    const dCoord& c1 = cell1->coord();
    const dCoord& c2 = cell2->coord();
    dCoord dif = diff(c1, c2, isToroid);
    dCoord c2d = c1 + dif;
    double r = sqr(dif.x) + sqr(dif.y) + sqr(dif.z);
    double r1 = cell1->figure()->radius();
    double r2 = cell2->figure()->radius();
    double h2 = static_cast<FCylinder*>(cell2->figure())->height() * 0.5;
    double r_sum = sqr(r1 + std::sqrt(sqr(r2) + sqr(h2)));
    // 1
    if (r > r_sum) {
        return false;
    } // TODO isToroid
    // 2 Transform
    const Vector3d& transf = cell2->rotate();
    dCoord c2A(0.0, 0.0, h2);
    Vector3d N = Vector3d::Oy();
    c2A.rotate(transf.x, Vector3d::Ox());
    N.rotate  (transf.x, Vector3d::Ox());
    c2A.rotate(transf.y, N);
    dCoord c2B = c2d - c2A;
    c2A = c2d + c2A;
    Vector3d A1 = c1  - c2A;
    Vector3d B1 = c1  - c2B;
    Vector3d AB = c2B - c2A;
    double base = 2.0 * h2;//sqrt(AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);
    double baseA1 = A1.length();
    double baseB1 = B1.length();
    //double cosA = dCoord::cosA(c1, c2A, c2B);
    //double cosB = dCoord::cosA(c1, c2B, c2A);
    double cosA =  (A1.x * AB.x + A1.y * AB.y + A1.z * AB.z) / (baseA1 * base);
    double cosB = -(B1.x * AB.x + B1.y * AB.y + B1.z * AB.z) / (baseB1 * base);
    if (cosA >= 0 && cosB >= 0) {
        //double S = 0.5 * base * std::sqrt(1 - cosA * cosA) * baseA1;
        //double SS= 0.5 * base * (r1 + r2);
        return ((r1 + r2) > std::sqrt(1.0 - cosA * cosA) * baseA1);
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
            double A = std::sqrt(r2 * r2 + baseA1 * baseA1 - 2.0 * r2 * baseA1 * cosA);
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
            double B = std::sqrt(r2 * r2 + baseB1 * baseB1 - 2.0 * r2 * baseB1 * cosB);
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
    result = (((r1 + r2) > std::sqrt(square(diff.x) + square(diff.y))) && ((r1 + h2) > abs(diff.z)));
    if (abs(diff.z) <= h2) {
        return !((square(diff.x) + square(diff.y)) - square(r1 + r2) > EPS);
    } else {
        return ((r1 + h2) - abs(diff.z) > EPS);
    }*/
}

bool Field::isOverlapCylCyl(const Cell* cell1, const Cell* cell2, bool isToroid) const
{
    const dCoord& c1 = cell1->coord();
    const dCoord& c2 = cell2->coord();
    dCoord dif = diff(c1, c2, isToroid); // TODO isToroid
    dCoord c2d = c1 + dif;
    double r1 = cell1->figure()->radius();
    double h1 = static_cast<FCylinder*>(cell1->figure())->height() * 0.5;
    double r2 = cell2->figure()->radius();
    double h2 = static_cast<FCylinder*>(cell2->figure())->height() * 0.5;
    // 2
    const Vector3d& transf1 = cell1->rotate();
    const Vector3d& transf2 = cell2->rotate();

    dCoord c1A(0.0, 0.0, h1);
    Vector3d Ox1 = Vector3d::Ox();//(1.0, 0.0, 0.0);
    Vector3d Oy1 = Vector3d::Oy();//(0.0, 1.0, 0.0);
    Vector3d Oz1 = Vector3d::Oz();//(0.0, 0.0, 1.0);
    c1A.rotate(transf1.x, Ox1);
    Oy1.rotate(transf1.x, Ox1);
    Oz1.rotate(transf1.x, Ox1);
    c1A.rotate(transf1.y, Oy1);
    Ox1.rotate(transf1.y, Oy1);
    Oz1.rotate(transf1.y, Oy1);
    // Ox1, Oy1, Oz1 - оси цилиндра1 после поворота

    dCoord c1B = c1 - c1A;
    c1A = c1 + c1A;

    dCoord c2A(0.0, 0.0, h2);
    Vector3d Ox2 = Vector3d::Ox();
    Vector3d Oy2 = Vector3d::Oy();
    Vector3d Oz2 = Vector3d::Oz();
    c2A.rotate(transf2.x, Ox2);
    Oy2.rotate(transf2.x, Ox2);
    Oz2.rotate(transf2.x, Ox2);
    c2A.rotate(transf2.y, Oy2);
    Ox2.rotate(transf2.y, Oy2);
    Oz2.rotate(transf2.y, Oy2);
    // Ox2, Oy2, Oz2 - оси цилиндра2 после поворота

    dCoord c2B = c2d - c2A;
    c2A = c2d + c2A;

    Vector3d norm1 = dCoord::normal(Ox1, Oy1);
    Vector3d norm2 = dCoord::normal(Ox2, Oy2);

    if (isOverlapCylPoint(c1A, c1B, r1, c2A, norm2, r2, isToroid)) {
        return true;
    }
    if (isOverlapCylPoint(c1A, c1B, r1, c2B, norm2, r2, isToroid)) {
        return true;
    }
    if (isOverlapCylPoint(c2A, c2B, r2, c1A, norm1, r1, isToroid)) {
        return true;
    }
    if (isOverlapCylPoint(c2A, c2B, r2, c1B, norm1, r1, isToroid)) {
        return true;
    }
    //return false;
    Vector3d L1 = c1B - c1A;
    Vector3d L2 = c2B - c2A;
    // c1A - точка на плоскости
    Vector3d S = dCoord::normal(L1, L2);
    S.normalize();
    double D  = -dCoord::termwiseMultiplySum(c1A, S);
    double Smin = (dCoord::termwiseMultiplySum(c2A, S) + D) / S.length();

    dCoord EA = c1A + S * Smin;

    double t1, t2;
    //dCoord overpoint =
    dCoord::overlap(L1, L2, EA, c2A, t1, t2);
    if (t2 >= 0 && t2 <= 1 && t1 >= 0 && t1 <= 1) {
        return abs(Smin) <= (r1 + r2);
    }
    return false;
}

bool Field::isOverlapCylPoint(const dCoord& base1, const dCoord& base2,
                              double r1, const dCoord& other,
                              const Vector3d& area, double r2, bool /*isToroid*/) const
{
    Vector3d C1 = base1 - base2;
    Vector3d L1 = dCoord::negative(C1);
    // плоскость c2A c2B c1A
    Vector3d C2AC1A = other - base1;
    Vector3d SC2ABC1A = dCoord::normal(C1, C2AC1A);
    SC2ABC1A.normalize();
    // плоскость у основания c2A
    Vector3d SC2AOC2AB = dCoord::normal(SC2ABC1A, area);
    SC2AOC2AB.normalize();
    double t1, t2;
    dCoord op1 = dCoord::overlap(L1, SC2AOC2AB, base1, other, t1, t2);
    double cosA = dCoord::cosA(base1, op1, other);
    if (1.0 - cosA < 0.1) {
        Vector3d V = other - base1;
        return (V.length() <= r2);
    }
    if (1.0 + cosA < 0.1) {
        Vector3d V = other - base2;
        return (V.length() <= r2);
    }
    Vector3d bnorm = base1 - base2;
    bnorm.normalize();
    double hip = r1 / std::sqrt(1.0 - cosA * cosA);
    double kat = hip * cosA;

    Vector3d op11 = op1 + bnorm * kat;

    Vector3d c2Aop1 = op11 - other;
    Vector3d c1Aop1 = op11 - base1;
    Vector3d c1Bop1 = op11 - base2;

    // проверка
    return (hip + r2 >= c2Aop1.length()
            && (c1Aop1.x * c1Bop1.x <= 0
                && c1Aop1.y * c1Bop1.y <= 0 && c1Aop1.z * c1Bop1.z <= 0));
//    if (hip + r2 >= c2Aop1.length()
//            && (c1Aop1.x * c1Bop1.x <= 0
//                && c1Aop1.y * c1Bop1.y <= 0 && c1Aop1.z * c1Bop1.z <= 0)) {
//        return true;
//    }
//    return false;
}

bool Field::isOverlapped(const Cell* cell1, const Cell* cell2, bool isToroid) const
{
    FigureType t1 = cell1->figure()->type();
    FigureType t2 = cell2->figure()->type();
    if (t1 == fig_sphere && t2 == fig_sphere) {
        return isOverlapSphSph(cell1, cell2, isToroid);
    }
    if (t1 == fig_sphere && t2 == fig_cylinder) {
        return isOverlapSphCyl(cell1, cell2, isToroid);
    }
    if (t2 == fig_sphere && t1 == fig_cylinder) {
        return isOverlapSphCyl(cell2, cell1, isToroid);
    }
    if (t1 == fig_cylinder && t2 == fig_cylinder) {
        return isOverlapCylCyl(cell1, cell2, isToroid);
    }
    return isOverlapSphSph(cell1, cell2, isToroid); // default
}

dCoord Field::diff(const dCoord& c1, const dCoord& c2, bool isToroid) const
{
    dCoord d = c2 - c1;
    if (isToroid) {
        dCoord diff;
        if (std::abs(d.x) < m_sizes.x - std::abs(d.x)) {
            diff.x = d.x;
        } else {
            diff.x = -(m_sizes.x - std::abs(d.x));
        }
        if (std::abs(d.y) < m_sizes.y - std::abs(d.y)) {
            diff.y = d.y;
        } else {
            diff.y = -(m_sizes.y - std::abs(d.y));
        }
        if (std::abs(d.z) < m_sizes.z - std::abs(d.z)) {
            diff.z = d.z;
        } else {
            diff.z = -(m_sizes.z - std::abs(d.z));
        }
        return diff;
    } else {
        return d;
    }
}

double Field::leng(const Cell* cell1, const Cell* cell2, bool isToroid) const
{
    dCoord d = diff(cell1->coord(), cell2->coord(), isToroid);
    return std::sqrt(sqr(d.x) + sqr(d.y) + sqr(d.z));
//    dCoord diff = cell1->coord() - cell2->coord();
//    double r = std::min(sqr(diff.x), double(sqr(m_sizes.x - abs(diff.x))));
//    r += std::min(sqr(diff.y), double(sqr(m_sizes.y - abs(diff.y))));
//    r += std::min(sqr(diff.z), double(sqr(m_sizes.z - abs(diff.z))));
//    return std::sqrt(r);
}
