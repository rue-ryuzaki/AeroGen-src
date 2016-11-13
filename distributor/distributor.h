#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <vector>

#include "devfield.h"
#include "../basegenerator.h"

class Distributor
{
public:
    explicit Distributor(QObject * parent);
    ~Distributor();
    
    void    Calculation(Field * fld, double d, double dFrom, double dTo, double dStep);
    
    void    Cancel();
    std::vector<distrib> getDistr() const;

private:
    void printDistribution(const std::vector<distrib> & distr) const;
    std::vector<distrib> getDistribution(DevField * dFld, double dFrom, double dTo, double dStep) const;

    QObject * mainwindow;
    std::vector<distrib> distr;
    bool cancel = false;
};

#endif /* DISTRIBUTOR_H */
