#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <vector>

#include "devfield.h"
#include "../basegenerator.h"

class Distributor
{
public:
    explicit Distributor(QObject* parent);
    
    void    calculate(Field* fld, double d, double dFrom, double dTo, double dStep);
    
    void    cancel();
    const std::vector<distrib>& distribution() const;

private:
    void    printDistribution(const std::vector<distrib>& m_distr) const;
    std::vector<distrib> distribution(DevField* dFld, double dFrom,
                                      double dTo, double dStep) const;

    QObject*                m_mainwindow;
    std::vector<distrib>    m_distr;
    bool                    m_cancel;
};

#endif // DISTRIBUTOR_H
