#ifndef STEGO_HIDESEEK_H
#define STEGO_HIDESEEK_H

#include "../Algorithm.h"

/**
 * Hide & Seek algorithm. Sets both motion vector
 * component LSBs to those of the payload.
 */
class HideSeek : public Algorithm {
public:
    virtual void initAsEncoder(stego_params *params);
    virtual void initAsDecoder(stego_params *params);
    virtual void embedIntoMv(int16_t *mvX, int16_t *mvY);
    virtual void extractFromMv(int16_t mvX, int16_t mvY);

    virtual bool embedIntoMvComponent(int16_t *mv, int bit);
    virtual bool extractFromMvComponent(int16_t mv, int *bit);

    virtual stego_result finalise();

protected:
    void getDataToEmbed();
    void writeRecoveredData();
    virtual void processMvComponentEmbed(int16_t *mv);
    virtual void processMvComponentExtract(int16_t mv);

    bool stopEmbedding = false;
    uint indexLimit = 0;
    uint index = 0;
    uint8_t symb[CryptoFile::BlockSize];
};

#endif //STEGO_HIDESEEK_H
