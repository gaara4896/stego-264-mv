//
// Created by el398 on 07/12/15.
//

#include <iostream>
#include <cstring>

#include "algorithm.h"
#include "algo/HideSeek.h"

void Algorithm::initAsEncoder(stego_params *params) {
    datafile.open(params->filename, std::ios::in | std::ios::binary);
    flags = params->flags;
}

void Algorithm::initAsDecoder(stego_params *params) {
    datafile.open(params->filename, std::ios::out | std::ios::binary);
    flags = params->flags;
}

stego_result Algorithm::finalise() {
    return stego_result {
            bits_processed / 8,
            0, NULL
    };
}

void stego_encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride) {
    algorithm->encode(mvs, mb_type, mb_width, mb_height, mv_stride);
}

void stego_decode(int16_t (*mvs[2])[2], int mv_sample_log2, int mb_width, int mb_height, int mv_stride) {
    algorithm->decode(mvs, mv_sample_log2, mb_width, mb_height, mv_stride);
}

void stego_init_algorithm(const char *algname) {
    if(algorithm != nullptr) {
        delete algorithm;
    }

    if(std::strcmp(algname, "hidenseek") == 0) {
        algorithm = new HideSeek();
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