#include <iostream>
#include "stego_connector.h"
#include "Algorithm.h"

#include "algo/HideSeek.h"
#include "algo/RandomisedHideSeek.h"
#include "algo/DumpMvs.h"
#include "algo/MVSteg.h"

static Algorithm *algorithm;

void stego_encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride) {
    if(algorithm == nullptr) {
        std::cerr << "stego_encode error: initialise algorithm first (stego_init_encoder)."
                  << std::endl;
        return;
    }
    algorithm->encode(mvs, mb_type, mb_width, mb_height, mv_stride);
}

void stego_decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height, int mv_stride,
                   int mb_stride) {
    if(algorithm == nullptr) {
        std::cerr << "stego_decode error: initialise algorithm first (stego_init_decoder)."
                  << std::endl;
        return;
    }
    algorithm->decode(mvs, mbtype_table, mv_sample_log2, mb_width, mb_height, mv_stride, mb_stride);
}

void init_algorithm(const char *algname, void* alg_params) {
    if(algorithm != nullptr) {
        delete algorithm;
    }

    if(std::strcmp(algname, "hidenseek") == 0) {
        algorithm = new HideSeek();
    } else if(std::strcmp(algname, "rand-hidenseek") == 0) {
        algorithm = new RandomisedHideSeek(reinterpret_cast<RandomisedHideSeek::AlgOptions*>(alg_params));
    } else if(std::strcmp(algname, "dumpmvs") == 0) {
        algorithm = new DumpMvs();
    } else if(std::strcmp(algname, "mvsteg") == 0) {
        algorithm = new MVSteg();
    }
}

void stego_init_encoder(const char *algname, stego_params *params, void* alg_params) {
    init_algorithm(algname, alg_params);
    algorithm->initAsEncoder(params);
}

void stego_init_decoder(const char *algname, stego_params *params, void* alg_params) {
    init_algorithm(algname, alg_params);
    algorithm->initAsDecoder(params);
}

unsigned int stego_get_embedded_data_size(unsigned int data_size) {
    if(algorithm == nullptr) {
        std::cerr << "stego_get_embedded_data_size error: initialise algorithm first (stego_init_encoder)."
                  << std::endl;
        exit(1);
    }
    return algorithm->computeEmbeddingSize(data_size);
}

stego_result stego_finalise() {
    stego_result result = algorithm->finalise();
    delete algorithm;
    algorithm = nullptr;
    return result;
}
