#include <cstdlib>
#include <ctgmath>
#include "MVSteg.h"

// Chosen by a fair dice roll
#define THRESH 5

bool MVSteg::embedIntoMvComponent(int16_t *mv, int bit) {
    if(*mv == 0) return false;
    int mvbit = (*mv) & 1;

    // If LSBs are not the same and *mv is positive, decrease the absolute value of MV
    if(bit != mvbit && *mv > 0 && !(flags & STEGO_DUMMY_PASS)) {
        (*mv)--;
    }
    // If LSBs are not different and *mv is negative, decrease the absolute value of MV
    if(bit == mvbit && *mv < 0 && !(flags & STEGO_DUMMY_PASS)) {
        (*mv)++;
    }

    return *mv != 0;
}

bool MVSteg::doEmbedding(int16_t *mvX, int16_t *mvY, int bit) {
    if(!usableMv(*mvX, *mvY)) return false;
    bool success;

    if (abs(*mvX) > abs(*mvY)) success = embedIntoMvComponent(mvX, bit);
    else success = embedIntoMvComponent(mvY, bit);
    if(!success) return false;

    // If the maximal component changed, it will re-embed the data
    // If it didn't, re-embedding into the same component is a no-op
    if (abs(*mvX) > abs(*mvY)) success = embedIntoMvComponent(mvX, bit);
    else success = embedIntoMvComponent(mvY, bit);

    return success && usableMv(*mvX, *mvY); // Check for shrinkage
}

void MVSteg::embedIntoMv(int16_t *mvX, int16_t *mvY) {
    if(stopEmbedding) return;

    int bit = symb[index / 8] >> (index % 8);
    bool success = doEmbedding(mvX, mvY, bit & 1);
    if(!success) return;

    index++;
    bits_processed++;

    this->getDataToEmbed();
}

bool MVSteg::extractFromMvComponent(int16_t val, int *bit) {
    if(val == 0) return false;
    *bit = (val < 0)? ~val : val;
    return true;
}

bool MVSteg::doExtraction(int16_t mvX, int16_t mvY, int *bit) {
    if(!usableMv(mvX, mvY)) return false;
    int16_t val = (abs(mvX) > abs(mvY))? mvX : mvY;
    bool success = extractFromMvComponent(val, bit);
    return success;
}

void MVSteg::extractFromMv(int16_t mvX, int16_t mvY) {
    int bit = 0;
    bool success = doExtraction(mvX, mvY, &bit);
    if(!success) return;

    symb[index / 8] |= (bit & 1) << (index % 8);
    index++;
    bits_processed++;

    this->writeRecoveredData();
}

bool MVSteg::usableMv(int16_t mvX, int16_t mvY) {
    double mvValX = double(mvX) / 2;
    double mvValY = double(mvY) / 2;

    return std::hypot(mvValX, mvValY) >= THRESH;
}