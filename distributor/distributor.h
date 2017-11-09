#ifndef AEROGEN_DISTRIBUTOR_DISTRIBUTOR_H
#define AEROGEN_DISTRIBUTOR_DISTRIBUTOR_H

#include <vector>

#include "devfield.h"
#include "../basegenerator.h"

class Distributor
{
public:
    explicit Distributor(QObject* parent);
    
    void    calculate(Field* fld, double d, double from, double to, double step);
    
    void    cancel();
    const std::vector<distrib>& distribution() const;

private:
    void    calcDistr(DevField* fld, double from, double to, double step);

    QObject*                m_mainwindow;
    std::vector<distrib>    m_distr;
    bool                    m_cancel;

    Distributor(const Distributor&) = delete;
    Distributor& operator =(const Distributor&) = delete;
};

#endif // AEROGEN_DISTRIBUTOR_DISTRIBUTOR_H
