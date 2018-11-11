#ifndef STEGO_MVSTEG_H
#define STEGO_MVSTEG_H

#include "HideSeek.h"

class MVSteg : public HideSeek {
public:
    virtual void extractFromMv(int16_t mv_x, int16_t mv_y);
    virtual void embedIntoMv(int16_t *mv_x, int16_t *mv_y);
    virtual bool doEmbedding(int16_t *mvX, int16_t *mvY, int bit);
    virtual bool doExtraction(int16_t mvX, int16_t mvY, int *bit);
    virtual bool usableMv(int16_t mvX, int16_t mvY);

protected:
    bool embedIntoMvComponent(int16_t *mv, int bit);
    bool extractFromMvComponent(int16_t val, int *bit);
};

#endif //STEGO_MVSTEG_H
