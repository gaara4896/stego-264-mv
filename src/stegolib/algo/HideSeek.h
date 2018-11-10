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

    void getDataToEmbed();
    void writeRecoveredData();

    bool stopEmbedding = false;
    uint indexLimit = 0;
    uint index = 0;
    uint8_t symb[CryptoFile::BlockSize];

};

#endif //STEGO_HIDESEEK_H
