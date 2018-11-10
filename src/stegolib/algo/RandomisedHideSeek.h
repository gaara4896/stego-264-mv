#ifndef STEGO_RANDOMISEDHIDESEEK_H
#define STEGO_RANDOMISEDHIDESEEK_H

#include "../Algorithm.h"
#include "HideSeek.h"
#include <random>

#define SEED_SIZE 16

class RandomisedHideSeek : public HideSeek {
public:
    struct AlgOptions {
        uint32_t byteCapacity;
        uint32_t fileSize; // Decoder only
    };

    void initAsEncoder(stego_params *params);
    void initAsDecoder(stego_params *params);
    stego_result finalise();

protected:
    virtual void processMvComponentEmbed(int16_t *mv);
    virtual void processMvComponentExtract(int16_t mv);

private:
    /**
     * Pair structure to hold "data bit" to "MV number" mapping.
     */
    struct Pair {
        ulong bit, mv;
        bool operator<(const Pair &m) const {
            return mv < m.mv;
        }
    };

    uint fileSize, dataSize;
    unsigned char *data;
    Pair *bitToMvMapping;

    void initialiseMapping(AlgOptions *algParams, uint dataSize);
};


#endif //STEGO_RANDOMISEDHIDESEEK_H
