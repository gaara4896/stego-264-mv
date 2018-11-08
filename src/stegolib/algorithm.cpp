#include <iostream>
#include <cstring>

#include "algorithm.h"
#include "algo/HideSeek.h"
#include "algo/RandomisedHideSeek.h"
#include "algo/DumpMVs.h"
#include "algo/MVSteg.h"

void Algorithm::initAsEncoder(stego_params *params) {
    datafile.open(params->filename, std::ios::in | std::ios::binary);
    flags = params->flags;
    encoder = true;
}

void Algorithm::initAsDecoder(stego_params *params) {
    datafile.open(params->filename, std::ios::out | std::ios::binary);
    flags = params->flags;
    encoder = false;
}

stego_result Algorithm::finalise() {
    int error = 0;

    if(encoder && !datafile.eof()) {
        error = 1;
    }
    datafile.close();
    return stego_result {
            uint(bits_processed / 8), error
    };
}

void stego_encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride) {
    algorithm->encode(mvs, mb_type, mb_width, mb_height, mv_stride);
}

void stego_decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height, int mv_stride,
                   int mb_stride) {
    algorithm->decode(mvs, mbtype_table, mv_sample_log2, mb_width, mb_height, mv_stride, mb_stride);
}

void stego_init_algorithm(const char *algname) {
    if (algorithm != nullptr) {
        delete algorithm;
    }

    if (std::strcmp(algname, "hidenseek") == 0) {
        algorithm = new HideSeek();
    } else if (std::strcmp(algname, "rand-hidenseek") == 0) {
        algorithm = new RandomisedHideSeek();
    } else if (std::strcmp(algname, "dumpmvs") == 0) {
        algorithm = new DumpMVs();
    } else if (std::strcmp(algname, "mvsteg") == 0) {
        algorithm = new MVSteg();
    }
}

void stego_init_encoder(stego_params *params) {
    algorithm->initAsEncoder(params);
}

void stego_init_decoder(stego_params *params) {
    algorithm->initAsDecoder(params);
}

stego_result stego_finalise() {
    stego_result result = algorithm->finalise();
    delete algorithm;
    algorithm = nullptr;
    return result;
}