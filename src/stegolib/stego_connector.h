#ifndef STEGO_CONNECTOR_H
#define STEGO_CONNECTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Extra options available for @ref stego_params
 */
typedef enum {
    STEGO_NO_PARAMS = 0,
    STEGO_DUMMY_PASS = 1,
    STEGO_ENABLE_ENCRYPTION = 2
} stego_flags;

/**
 * Parameters for a steganography algorithm.
 */
typedef struct {
    const char *datafile; ///< Path to a file for data to be read from / written to.
    stego_flags flags;   ///< Flags @ref stego_flags
    const char *password; ///< Password for encryption
} stego_params;

/**
 * Result report of the encoding or decoding process.
 *
 * Error codes:
 *   * 0 -- no error
 *   * 1 -- file doesn't fit into the video (encoder only)
 */
typedef struct {
    unsigned int bytes_processed; ///< Number of (full) bytes processed
    int error_code;               ///< Error code (see @ref stego_result)
} stego_result;

/**
 * Set up the library to use a particular algorithm for encoding.
 * Currently available options:
 *     - 'hidenseek'
 *     - 'dumpmvs' (dec) / 'dummypass' (enc)
 *     - 'f3'
 *     - 'f4'
 *     - 'msteg'
 *     - 'mvsteg'
 *     - 'outguess1'
 *     - 'rand-hidenseek'
 *     - 'xualg'
 *
 * @param algname Name of the algorithm from the options above.
 * @param params Parameter structure @ref stego_params
 * @param algParams Additional parameters for algorithm of your choice.
 */
void stego_init_encoder(const char *algname, stego_params *params, void* alg_params);

/**
 * Set up the library to use a particular algorithm for decoding.
 * Currently available options:
 *     - 'hidenseek'
 *     - 'dumpmvs' (dec) / 'dummypass' (enc)
 *     - 'f3'
 *     - 'f4'
 *     - 'msteg'
 *     - 'mvsteg'
 *     - 'outguess1'
 *     - 'rand-hidenseek'
 *     - 'xualg'
 *
 * @param algname Name of the algorithm from the options above.
 * @param params Parameter structure @ref stego_params
 * @param algParams Additional parameters for algorithm of your choice.
 */
void stego_init_decoder(const char *algname, stego_params *params, void* alg_params);

/**
 * Query the initialised algorithm to see how much space will a file occupy.
 * Must be done after @ref stego_init_encoder.
 */
unsigned int stego_get_embedded_data_size(unsigned int data_size);

/**
 * Gather results of the embedding / extracting process (see @ref stego_result).
 */
stego_result stego_finalise();

/**
 * Method called by FFmpeg to feed in motion vector data during encoding.
 * (parameters match those available as variables at the point
 * of injection).
 */
void stego_encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride);

/**
 * Method called by FFmpeg to feed in motion vector data during decoding.
 * (parameters match those available as variables at the point
 * of injection).
 */
void stego_decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height, int mv_stride,
                   int mb_stride);

#ifdef __cplusplus
}
#endif

#endif //STEGO_CONNECTOR_H
