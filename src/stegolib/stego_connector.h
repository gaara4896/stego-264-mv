//
// Created by el398 on 06/12/15.
//

#ifndef STEGO_CONNECTOR_H
#define STEGO_CONNECTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void stego_init_encoder(const char* filename);
void stego_init_decoder(const char* filename);
void stego_init_algorithm(const char *algname);
void stego_encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride);
void stego_decode(int16_t (*mvs[2])[2], int mv_sample_log2, int mb_width, int mb_height, int mv_stride);

#ifdef __cplusplus
}
#endif

#endif //STEGO_CONNECTOR_H
