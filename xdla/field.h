#ifndef AEROGEN_XDLA_FIELD_H
#define AEROGEN_XDLA_FIELD_H

#include <vector>

#include "../basefield.h"

namespace xdla {
class XField : public Field
{
public:
    XField(const char* fileName, txt_format format);
    XField(const Sizes& sizes = Sizes(50, 50, 50), bool isToroid = false);
    virtual ~XField() { }

    Sizes       sizes()                                          const override;
    void        initialize(double porosity, double cellsize)           override;
    std::vector<Cell> cells()                                    const override;
    uint32_t    monteCarlo(uint32_t stepMax)                     const override;

private:
    void        toDAT(const char* fileName)                      const override;
    void        toDLA(const char* fileName)                      const override;
    void        toTXT(const char* fileName)                      const override;
    void        fromDAT(const char* fileName)                          override;
    void        fromDLA(const char* fileName)                          override;
    void        fromTXT(const char* fileName)                          override;
};
} // xdla

#endif // AEROGEN_XDLA_FIELD_H
