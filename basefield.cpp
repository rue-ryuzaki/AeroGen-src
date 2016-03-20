#include "basefield.h"

void Field::toFile(const char* fileName, txt_format format) const {
    switch (format) {
        case txt_dat:
            toDAT(fileName);
            break;
        case txt_dla:
            toDLA(fileName);
            break;
        case txt_txt:
            toTXT(fileName);
            break;
    }
}

/*Field * Field::fromFile(const char* fileName, txt_format format) {
    Field * result;
    switch (format) {
        case txt_dat:
            result->fromDAT(fileName);
            break;
        case txt_dla:
            result->fromDLA(fileName);
            break;
        case txt_txt:
            result->fromTXT(fileName);
            break;
    }
    return result;
}*/
