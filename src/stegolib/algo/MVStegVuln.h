//
// Created by el on 12/01/16.
//

#ifndef STEGO_MVSTEGVULN_H
#define STEGO_MVSTEGVULN_H

#include "MVSteg.h"

class MVStegVuln : public MVSteg {
protected:
    virtual void modifyMV(int16_t *mv);
    virtual void extractFromPair(int16_t mv_x, int16_t mv_y);
    virtual void embedToPair(int16_t *mv_x, int16_t *mv_y);
};

#endif //STEGO_MVSTEGVULN_H
