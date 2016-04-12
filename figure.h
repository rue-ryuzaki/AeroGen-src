#ifndef FIGURE_H
#define	FIGURE_H

#include "baseformats.h"

enum FigureType {
    fig_figure, // something default value
    fig_sphere,
    fig_cylinder,
    fig_cube
};

class Figure {
public:
    Figure() { }
    virtual ~Figure() { }
    
    virtual double      getVolume() const = 0;
    virtual double      getArea() const = 0;
    virtual FigureType  getType() const = 0;
    virtual void        setRadius(double r) = 0;
    virtual double      getRadius() const = 0;
    
private:
};

class FSphere : public Figure {
public:
    FSphere(double r = 1.0) : r(r) { }
    ~FSphere() { }
    
    double      getVolume() const { return (4.0 / 3) * M_PI * pow(r, 3.0); }
    double      getArea()   const { return 4 * M_PI * pow(r, 2.0); }
    FigureType  getType()   const { return fig_sphere; }
    void        setRadius(double r) { this->r = r; }
    double      getRadius() const { return r; }
    
private:
    double r;
};

class FCylinder : public Figure {
public:
    FCylinder(double r = 1.0, double h = 1.0) : r(r), h(h) { }
    ~FCylinder() { }
    
    double      getVolume() const { return M_PI * pow(r, 2.0) * h; }
    double      getArea()   const { return 2 * M_PI * r * (h + r); }
    FigureType  getType()   const { return fig_cylinder; }
    void        setRadius(double r) { this->r = r; }
    double      getRadius() const { return r; }
    void        setHeight(double h) { this->h = h; }
    double      getHeight() const { return h; }
    
private:
    double r;
    double h;
};

class FCube : public Figure {
public:
    FCube(double a = 1.0) : a(a) { }
    ~FCube() { }
    
    double      getVolume() const { return pow(a, 3.0); }
    double      getArea()   const { return 6 * a * a; }
    FigureType  getType()   const { return fig_cube; }
    void        setRadius(double r) { this->a = r * 2; }
    double      getRadius() const { return a / 2; }
    void        setSide(double a) { this->a = a; }
    double      getSide()   const { return a; }
    
private:
    double a;
};

#endif	/* FIGURE_H */
