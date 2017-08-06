#ifndef BASEGENERATOR_H
#define	BASEGENERATOR_H

#include <QGLWidget>
#include <string>

#include "basefield.h"

struct distrib
{
    distrib (double r = 0.0, double vol = 0.0)
        : r(r),
          vol(vol),
          count(0)
    { }

    double r;
    double vol;
    double count;
};

class Generator
{
public:
    Generator(QObject* parent) : mainwindow(parent) { }
    virtual ~Generator() { delete m_fld; }
    
    virtual Field*  field() const { return m_fld; }
    virtual void    generate(const Sizes& sizes, double por, int initial, int step,
                             int hit, size_t cluster, double cellsize) = 0;

    virtual double  surfaceArea(double density) const = 0;
    virtual void    density(double density, double& denAero, double& porosity) const = 0;

    bool finished() const { return m_finished; }
    void cancel(bool value) { m_cancel = value; }
    
    virtual void save(const char* fileName, txt_format format) const = 0;
    virtual void save(const std::string& fileName, txt_format format) const
    {
        save(fileName.c_str(), format);
    }

    virtual void load(const char* fileName, txt_format format) = 0;
    virtual void load(const std::string& fileName, txt_format format)
    {
        load(fileName.c_str(), format);
    }

    bool run = false;

protected:
    bool    m_calculated = false;
    bool    m_finished   = false;
    bool    m_cancel     = false;

    QObject* mainwindow;

private:
    Field*  m_fld = nullptr;
};

#endif	// BASEGENERATOR_H
