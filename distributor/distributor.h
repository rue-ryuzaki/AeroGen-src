#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include "devfield.h"
#include "../basegenerator.h"

class Distributor {
public:
    Distributor(QObject * parent) : mainwindow(parent) { }
    ~Distributor() { }
    
    void Calculation(Field * fld, double d, double dFrom, double dTo, double dStep);
    
    void Cancel() { cancel = true; }
    vector<distrib> getDistr() { return distr; }
private:
    QObject * mainwindow;
    vector<distrib> distr;
    bool cancel = false;
    
    void printDistribution(const vector<distrib> & distr);
    vector<distrib> getDistribution(DevField * dFld, double dFrom, double dTo, double dStep);
};

#endif /* DISTRIBUTOR_H */
