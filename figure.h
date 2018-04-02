#ifndef AEROGEN_FIGURE_H
#define AEROGEN_FIGURE_H

#include "baseformats.h"

enum FigureType {
    fig_figure, // something default value
    fig_sphere,
    fig_cylinder,
    fig_cube
};

struct IFigure
{
    virtual ~IFigure() { }
    
    virtual double      volume() const = 0;
    virtual double      area()   const = 0;
    virtual FigureType  type()   const = 0;
    virtual double      radius() const = 0;
    virtual void        setRadius(double r) = 0;
};

class FSphere : public IFigure
{
public:
    explicit FSphere(double r = 1.0) : r(r) { }
    
    double      volume() const override { return (4.0 / 3.0) * M_PI * r * r * r; }
    double      area()   const override { return 4.0 * M_PI * r * r; }
    FigureType  type()   const override { return fig_sphere; }
    double      radius() const override { return r; }
    void        setRadius(double r) override { this->r = r; }
    
private:
    double r;
};

class FCylinder : public IFigure
{
public:
    FCylinder(double r = 1.0, double h = 1.0) : r(r), h(h) { }
    
    double      volume() const override { return M_PI * r * r * h; }
    double      area()   const override { return 2.0 * M_PI * r * (h + r); }
    FigureType  type()   const override { return fig_cylinder; }
    double      radius() const override { return r; }
    void        setRadius(double r) override { this->r = r; }
    double      height() const { return h; }
    void        setHeight(double h) { this->h = h; }
    
private:
    double r;
    double h;
};

class FCube : public IFigure
{
public:
    explicit FCube(double a = 1.0) : a(a) { }
    
    double      volume() const override { return a * a * a; }
    double      area()   const override { return 6.0 * a * a; }
    FigureType  type()   const override { return fig_cube; }
    double      radius() const override { return a / 2.0; }
    void        setRadius(double r) override { this->a = r * 2.0; }
    double      side()   const { return a; }
    void        setSide(double a) { this->a = a; }
    
private:
    double a;
};

#endif // AEROGEN_FIGURE_H
