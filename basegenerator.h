#ifndef AEROGEN_BASEGENERATOR_H
#define AEROGEN_BASEGENERATOR_H

#include <cstdint>
#include <string>
#include <QObject>

#include "basefield.h"
#include "params.h"

struct distrib
{
    distrib(double r = 0.0, double vol = 0.0)
        : r(r),
          vol(vol),
          count(0)
    { }

    double      r;
    double      vol;
    uint64_t    count;
};

class Generator
{
public:
    Generator(QObject* parent) : m_mainwindow(parent), m_fld(nullptr) { }
    virtual ~Generator() { delete m_fld; m_fld = nullptr; }
    
    virtual Field*  field() const { return m_fld; }
    virtual void    generate(const Sizes& sizes, const RunParams& params) = 0;

    virtual double  surfaceArea(double density, uint32_t steps) const = 0;
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
    bool    m_finished   = false;
    bool    m_cancel     = false;

    QObject* m_mainwindow;

private:
    Field*  m_fld;

    Generator(const Generator&) = delete;
    Generator& operator =(const Generator&) = delete;
};

#endif // AEROGEN_BASEGENERATOR_H
