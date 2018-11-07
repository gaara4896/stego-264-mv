#ifndef STEGO_RANDOMISEDHIDESEEK_H
#define STEGO_RANDOMISEDHIDESEEK_H

#include "../algorithm.h"
#include "HideSeek.h"
#include <random>

/**
 * Pair structure to hold "data bit" to "MV number" mapping.
 */
struct Pair {
    ulong bit, mv;
    bool operator<(const Pair &m) const {
        return mv < m.mv;
    }
};

class RandomisedHideSeek : public HideSeek {
public:
    void initAsEncoder(stego_params *params);
    void initAsDecoder(stego_params *params);
    stego_result finalise();

protected:
    void embedIntoMv(int16_t *mv);
    void extractFromMv(int16_t val);

private:
    uint fileSize, dataSize;
    unsigned char *data;
    Pair *bitToMvMapping;

    void initialiseMapping(const stego_params *params, uint dataSize);
};


#endif //STEGO_RANDOMISEDHIDESEEK_H
