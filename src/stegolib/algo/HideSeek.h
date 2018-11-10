#ifndef STEGO_HIDESEEK_H
#define STEGO_HIDESEEK_H

#include "../algorithm.h"

class HideSeek : public Algorithm {
public:
    virtual void embedToPair(int16_t *mvX, int16_t *mvY);
    virtual void extractFromPair(int16_t mvX, int16_t mvY);
    virtual void initAsEncoder(stego_params *params);
    virtual void initAsDecoder(stego_params *params);

protected:
    virtual void extractFromMv(int16_t val);
    virtual void embedIntoMv(int16_t *mv);
    uint index = 0;
    char symb = 0;
};

#endif //STEGO_HIDESEEK_H
