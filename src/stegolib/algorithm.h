//
// Created by el398 on 07/12/15.
//

#ifndef STEGO_ALGORITHM_H
#define STEGO_ALGORITHM_H

#include <fstream>

#include "stego_connector.h"

class Algorithm {
public:
    virtual void encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride) = 0;
    virtual void decode(int16_t (*mvs[2])[2], int mv_sample_log2, int mb_width, int mb_height, int mv_stride) = 0;

    virtual void initAsEncoder(stego_params *params);
    virtual void initAsDecoder(stego_params *params);
    virtual stego_result finalise();
protected:
    std::fstream datafile;
    int flags = 0;
    int bits_processed = 0;
    bool encoder;
};

static Algorithm *algorithm;

#endif //STEGO_ALGORITHM_H
