#ifndef STEGO_CONNECTOR_H
#define STEGO_CONNECTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STEGO_NO_PARAMS = 0,
    STEGO_DUMMY_PASS = 1,
    STEGO_ENABLE_ENCRYPTION = 2
} stego_flags;

typedef struct {
    const char *filename;
    stego_flags flags;
    const char *password;
    void *algParams;
} stego_params;

typedef struct {
    unsigned int bytes_processed;
    int error;
} stego_result;

void stego_init_encoder(stego_params *params);
void stego_init_decoder(stego_params *params);
int stego_init_algorithm(const char *algname);
stego_result stego_finalise();
void stego_encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride);
void stego_decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height, int mv_stride,
                   int mb_stride);

#ifdef __cplusplus
}
#endif

#endif //STEGO_CONNECTOR_H
