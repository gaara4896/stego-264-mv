//
// Created by el398 on 10/12/15.
//

#ifndef STEGO_RANDOMISEDHIDESEEK_H
#define STEGO_RANDOMISEDHIDESEEK_H

#include "../algorithm.h"
#include <random>

struct Pair {
    ulong f, s;
    bool operator<(const Pair &m) const {
        return s < m.s;
    }
};

class RandomisedHideSeek : public Algorithm {
public:
    RandomisedHideSeek();
    void encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride);
    void decode(int16_t (*mvs[2])[2], int mv_sample_log2, int mb_width, int mb_height, int mv_stride);
    void initAsEncoder(stego_params *params);
    void initAsDecoder(stego_params *params);
    stego_result finalise();

private:
    uint index = 0;
    uint fileSize;
    char *data;
    Pair *bitToMvMapping;

    void embedIntoMv(int16_t *mv);
    void extractFromMv(int16_t val);
    void initialiseMapping(const stego_params *params, int fileSize);
};


#endif //STEGO_RANDOMISEDHIDESEEK_H
