#ifndef MXDLA_MXDLA_H
#define	MXDLA_MXDLA_H

#include <QGLWidget>
#include <string>

#include "mxfield.h"
#include "../basegenerator.h"

class MxDLA : public Generator
{
public:
    MxDLA(QObject* parent) : Generator(parent) { }
    virtual ~MxDLA() { delete m_fld; }

    MxField* field() const { return m_fld; }
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
        m_fld = new MxField(fileName, format);
        m_finished = true;
    }

private:
    MxField* m_fld = nullptr;

    MxDLA(const MxDLA&) = delete;
    MxDLA& operator =(const MxDLA&) = delete;
};

#endif // MXDLA_MXDLA_H
