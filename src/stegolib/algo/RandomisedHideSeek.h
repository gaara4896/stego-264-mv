#ifndef STEGO_RANDOMISEDHIDESEEK_H
#define STEGO_RANDOMISEDHIDESEEK_H

#include "../algorithm.h"
#include "HideSeek.h"
#include <random>

#define SEED_SIZE 8

/**
 * Pair structure to hold "data bit" to "MV number" mapping.
 */
struct Pair {
    ulong bit, mv;
    bool operator<(const Pair &m) const {
        return mv < m.mv;
    }
};

struct AlgOptions {
    uint32_t byteCapacity;
    uint32_t fileSize;
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

    void initialiseMapping(AlgOptions *algParams, uint dataSize);
};


#endif //STEGO_RANDOMISEDHIDESEEK_H
