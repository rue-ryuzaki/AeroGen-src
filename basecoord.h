#ifndef BASECOORD_H
#define	BASECOORD_H

#include <stdlib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "baseformats.h"

using namespace std;

template <class type>
class Coord {
public:
    Coord(type x = 0, type y = 0, type z = 0) : x(x), y(y), z(z) { }
    Coord(const Coord& c) : Coord(c.x, c.y, c.z) { }
    virtual ~Coord() { }
    
    Coord operator+ (const Coord &) const;
    Coord operator- (const Coord &) const;
    Coord operator+ (const Sizes &) const;
    Coord operator- (const Sizes &) const;
    Coord operator* (const double&) const;
    Coord operator* (const int   &) const;
    Coord operator* (const uint  &) const;
    Coord operator/ (const double&) const;
    Coord operator/ (const int   &) const;
    Coord operator/ (const uint  &) const;
    bool operator== (const Coord &) const;
    bool operator!= (const Coord &) const;
    Coord& operator=(const Coord & rhs);

    double Length() const { return pow(x * x + y * y + z * z, 0.5); }
    void Rotate(double angle, double Ax, double Ay, double Az);
    void Rotate(double angle, const Coord &);
    void Print() const { cout << x << " " << y << " " << z << endl; }
    void Normalize() {
        double l = Length();
        x /= l;
        y /= l;
        z /= l;
    }
    void Negative();

    static Coord Negative(const Coord &);
    // нормаль к плоскости, образованной 2 векторами
    static Coord Normal(const Coord&, const Coord&);
    static Coord Ox() { return Coord(1, 0, 0); }
    static Coord Oy() { return Coord(0, 1, 0); }
    static Coord Oz() { return Coord(0, 0, 1); }
    static type TermwiseMultiplySum(const Coord&, const Coord&);
    static Coord Overlap(const Coord& vec1, const Coord& vec2, const Coord& p1,
        const Coord& p2, double& t1, double& t2);
    static double cosA(const Coord&, const Coord&, const Coord&);

    type x;
    type y;
    type z;

protected:
};

typedef Coord<double> Vector3d;

struct Quaternion {
    double x, y, z, w;
    
    Quaternion(double _x, double _y, double _z, double _w) {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }
    
    void Normalize() {
        double Length = sqrt(x * x + y * y + z * z + w * w);

        x /= Length;
        y /= Length;
        z /= Length;
        w /= Length;
    }
    
    Quaternion Conjugate() {
        Quaternion ret(-x, -y, -z, w);
        return ret;
    }
    
    Quaternion operator*(const Quaternion& rhs) {
        const double w = (this->w * rhs.w) - (this->x * rhs.x) - (this->y * rhs.y) - (this->z * rhs.z);
        const double x = (this->x * rhs.w) + (this->w * rhs.x) + (this->y * rhs.z) - (this->z * rhs.y);
        const double y = (this->y * rhs.w) + (this->w * rhs.y) + (this->z * rhs.x) - (this->x * rhs.z);
        const double z = (this->z * rhs.w) + (this->w * rhs.z) + (this->x * rhs.y) - (this->y * rhs.x);
        Quaternion ret(x, y, z, w);
        return ret;
    }
    
    Quaternion operator*(const Coord<double>& rhs) {
        const double w = - (this->x * rhs.x) - (this->y * rhs.y) - (this->z * rhs.z);
        const double x =   (this->w * rhs.x) + (this->y * rhs.z) - (this->z * rhs.y);
        const double y =   (this->w * rhs.y) + (this->z * rhs.x) - (this->x * rhs.z);
        const double z =   (this->w * rhs.z) + (this->x * rhs.y) - (this->y * rhs.x);
        Quaternion ret(x, y, z, w);
        return ret;
    }
};

template <class type>
Coord<type> Coord<type>::operator+ (const Coord<type> & rhs) const {
    Coord res;
    res.x = this->x + rhs.x;
    res.y = this->y + rhs.y;
    res.z = this->z + rhs.z;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator- (const Coord<type> & rhs) const {
    Coord res;
    res.x = this->x - rhs.x;
    res.y = this->y - rhs.y;
    res.z = this->z - rhs.z;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator+ (const Sizes & rhs) const {
    Coord res;
    res.x = this->x + rhs.x;
    res.y = this->y + rhs.y;
    res.z = this->z + rhs.z;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator- (const Sizes & rhs) const {
    Coord res;
    res.x = this->x - rhs.x;
    res.y = this->y - rhs.y;
    res.z = this->z - rhs.z;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator* (const double & v) const {
    Coord res;
    res.x = this->x * v;
    res.y = this->y * v;
    res.z = this->z * v;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator* (const int & v) const {
    Coord res;
    res.x = this->x * v;
    res.y = this->y * v;
    res.z = this->z * v;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator* (const uint & v) const {
    Coord res;
    res.x = this->x * v;
    res.y = this->y * v;
    res.z = this->z * v;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator/ (const double & v) const {
    Coord res;
    res.x = this->x / v;
    res.y = this->y / v;
    res.z = this->z / v;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator/ (const int & v) const {
    Coord res;
    res.x = this->x / v;
    res.y = this->y / v;
    res.z = this->z / v;
    return res;
}

template <class type>
Coord<type> Coord<type>::operator/ (const uint & v) const {
    Coord res;
    res.x = this->x / v;
    res.y = this->y / v;
    res.z = this->z / v;
    return res;
}

template <class type>
bool Coord<type>::operator== (const Coord<type> & rhs) const {
    bool res = true;
    res = res && (rhs.x == this->x);
    res = res && (rhs.y == this->y);
    res = res && (rhs.z == this->z);
    return res;
}

template <class type>
bool Coord<type>::operator!= (const Coord & rhs) const {
    return !(*this == rhs);
}

template <class type>
Coord<type> & Coord<type>::operator= (const Coord<type> & rhs) {
    if (this != &rhs) {
        this->x = rhs.x;
        this->y = rhs.y;
        this->z = rhs.z;
    }
    return *this;
}

template <class type>
void Coord<type>::Rotate(double angle, double Ax, double Ay, double Az) {
    Coord<double> rotVec(Ax, Ay, Az);
    Rotate(angle, rotVec);
}

template <class type>
void Coord<type>::Rotate(double angle, const Coord<type> & vec) {
    const double SinHalfAngle = sin(ToRadian(angle / 2));
    const double CosHalfAngle = cos(ToRadian(angle / 2));

    const double Rx = vec.x * SinHalfAngle;
    const double Ry = vec.y * SinHalfAngle;
    const double Rz = vec.z * SinHalfAngle;
    const double Rw = CosHalfAngle;
    Quaternion RotationQ(Rx, Ry, Rz, Rw);

    Quaternion ConjugateQ = RotationQ.Conjugate();
    ConjugateQ.Normalize();
    Quaternion W = RotationQ * (*this) * ConjugateQ;

    x = W.x;
    y = W.y;
    z = W.z;
}

template <class type>
void Coord<type>::Negative() { 
    this->x = -this->x;
    this->y = -this->y;
    this->z = -this->z;
}

template <class type>
Coord<type> Coord<type>::Negative(const Coord<type>& c) {
    Coord<type> result;
    result.x = -c.x;
    result.y = -c.y;
    result.z = -c.z;
    return result;
}

template <class type>
Coord<type> Coord<type>::Normal(const Coord<type>& c1, const Coord<type>& c2) {
    Coord<type> result;
    result.x = c1.y * c2.z - c1.z * c2.y;
    result.y = c1.z * c2.x - c1.x * c2.z;
    result.z = c1.x * c2.y - c1.y * c2.x;
    return result;
}

template <class type>
type Coord<type>::TermwiseMultiplySum(const Coord<type>& c1, const Coord<type>& c2) {
    type result = c1.x * c2.x;
    result += c1.y * c2.y;
    result += c1.z * c2.z;
    return result;
}

template <class type>
Coord<type> Coord<type>::Overlap(const Coord<type>& vec1, const Coord<type>& vec2,
        const Coord<type>& p1, const Coord<type>& p2, double& t1, double& t2) {
    t2 = ((p1.y - p2.y) * vec1.x + (p2.x - p1.x) * vec1.y) / (vec2.y * vec1.x - vec2.x * vec1.y);
    t1 = (p2.x + t2 * vec2.x - p1.x) / vec1.x;
    Coord<type> result = p1 + vec1 * t1;
    return result;
}

template <class type>
double Coord<type>::cosA(const Coord<type>& A,  const Coord<type>& B, const Coord<type>& C) {
    Coord<type> BA = A - B;
    Coord<type> BC = C - B;
    return (BA.x * BC.x + BA.y * BC.y + BA.z * BC.z) / (BA.Length() * BC.Length());
}

#endif	/* BASECOORD_H */
