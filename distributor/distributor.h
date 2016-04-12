#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include "devfield.h"
#include "../basegenerator.h"

class Distributor {
public:
    Distributor(QObject * parent);
    ~Distributor();
    
    void    Calculation(Field * fld, double d, double dFrom, double dTo, double dStep);
    
    void    Cancel();
    vector<distrib> getDistr() const;

private:
    void printDistribution(const vector<distrib> & distr);
    vector<distrib> getDistribution(DevField * dFld, double dFrom, double dTo, double dStep);

    QObject * mainwindow;
    vector<distrib> distr;
    bool cancel = false;
};

#endif /* DISTRIBUTOR_H */
