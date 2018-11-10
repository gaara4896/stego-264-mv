//
// Created by el on 01/01/16.
//

#include <iostream>
#include "DumpMVs.h"

void DumpMVs::encode(int16_t (*)[2], uint16_t *, int , int , int ) {
    std::cerr << "This algorithm does not support encode mode." << std::endl
              << "Run decoder application to dump MVs." << std::endl;
}

void DumpMVs::decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height,
                      int mv_stride, int mb_stride) {
    auto &stream = datafile.exposeStream();
    if(!dimsWritten) {
        stream << mb_height << " " << mb_width << std::endl;
        dimsWritten = true;
    }
    for (int mb_x = 0; mb_x < mb_width; mb_x++) {
        for (int mb_y = 0; mb_y < mb_height; mb_y++) {
            int xy = (mb_x + mb_y * mv_stride) << mv_sample_log2;
            stream << mvs[0][xy][0] << ", " << mvs[0][xy][1] << ", "
                   << int(mbtype_table[mb_x + mb_y * mb_stride] == 1) << ";" << std::endl;
            bits_processed += 2;
        }
    }
}

void DumpMVs::embedIntoMv(int16_t *, int16_t *) {

}

void DumpMVs::extractFromMv(int16_t , int16_t ) {

}