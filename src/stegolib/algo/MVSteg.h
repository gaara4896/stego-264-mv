//
// Created by el on 03/01/16.
//

#ifndef STEGO_MVSTEG_H
#define STEGO_MVSTEG_H

#include "../algorithm.h"

class MVSteg : public Algorithm {
public:
    void initAsEncoder(stego_params *params);
    void initAsDecoder(stego_params *params);
    virtual void extractFromPair(int16_t mv_x, int16_t mv_y);
    virtual void embedToPair(int16_t *mv_x, int16_t *mv_y);

protected:
    virtual void modifyMV(int16_t *mv);
    uint index = 0;
    char symb = 0;
};

#endif //STEGO_MVSTEG_H
