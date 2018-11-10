#ifndef STEGO_ALGORITHM_H
#define STEGO_ALGORITHM_H

#include <fstream>
#include <memory>
#include <vector>
#include "stego_connector.h"

class Algorithm {
public:
    virtual void encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride);
    virtual void decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height,
                            int mv_stride, int mb_stride);
    virtual void embedToPair(int16_t *mvX, int16_t *mvY) = 0;
    virtual void extractFromPair(int16_t mvX, int16_t mvY) = 0;

    virtual void initAsEncoder(stego_params *params);
    virtual void initAsDecoder(stego_params *params);
    virtual stego_result finalise();
protected:
    std::vector<uint8_t> deriveBytes(size_t numBytes, std::string salt);

    std::fstream datafile;
    int flags = 0;
    int bits_processed = 0;
    bool encoder;
    std::string password;
};

static Algorithm *algorithm;

#endif //STEGO_ALGORITHM_H
