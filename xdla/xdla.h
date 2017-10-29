#ifndef XDLA_H
#define	XDLA_H

#include <QGLWidget>
#include <string>

#include "xfield.h"
#include "../basegenerator.h"

class xDLA : public Generator
{
public:
    xDLA(QObject* parent) : Generator(parent) { }
    virtual ~xDLA() { delete m_fld; }

    xField* field() const { return m_fld; }
    void    generate(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                     uint32_t hit, uint32_t cluster, double cellsize);
    double  surfaceArea(double density) const;
    void    density(double density, double& denAero, double& porosity) const;

    void    save(const char* fileName, txt_format format) const
    {
        m_fld->toFile(fileName, format);
    }

    void    load(const char* fileName, txt_format format)
    {
        if (m_fld) {
            delete m_fld;
        }
        m_fld = new xField(fileName, format);
        m_finished = true;
    }

private:
    xField* m_fld = nullptr;

    xDLA(const xDLA&) = delete;
    xDLA& operator =(const xDLA&) = delete;
};

#endif // XDLA_H
