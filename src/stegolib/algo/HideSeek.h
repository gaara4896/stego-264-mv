#ifndef STEGO_HIDESEEK_H
#define STEGO_HIDESEEK_H

#include "../algorithm.h"

class HideSeek : public Algorithm {
public:
    void encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride);
    void decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height,
                    int mv_stride, int mb_stride);
    virtual void initAsEncoder(stego_params *params);
    virtual void initAsDecoder(stego_params *params);

protected:
    virtual void extractFromMv(int16_t val);
    virtual void embedIntoMv(int16_t *mv);
    uint index = 0;
    char symb = 0;
};

#endif //STEGO_HIDESEEK_H
