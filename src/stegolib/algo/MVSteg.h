//
// Created by el on 03/01/16.
//

#ifndef STEGO_MVSTEG_H
#define STEGO_MVSTEG_H

#include "HideSeek.h"

class MVSteg : public HideSeek {
public:
    virtual void extractFromPair(int16_t mv_x, int16_t mv_y);
    virtual void embedToPair(int16_t *mv_x, int16_t *mv_y);

protected:
    virtual void modifyMV(int16_t *mv);
};

#endif //STEGO_MVSTEG_H
