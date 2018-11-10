#include <iostream>
#include <cstring>
#include <sha.h>
#include <pwdbased.h>

#include "algorithm.h"
#include "algo/HideSeek.h"
#include "algo/RandomisedHideSeek.h"
#include "algo/DumpMVs.h"
#include "algo/MVSteg.h"
#include "stego_connector.h"

#define CRYPTO_SALT "StegoEncryption"
#define DEFAULT_PASSWORD "StegoDefaultPassword"

void Algorithm::initAsEncoder(stego_params *params) {
    this->encoder = true;
    this->initialiseAlgorithm(params);
}

void Algorithm::initAsDecoder(stego_params *params) {
    this->encoder = false;
    this->initialiseAlgorithm(params);
}

void Algorithm::initialiseAlgorithm(stego_params *params) {
    // Unpack parameters
    this->flags = params->flags;
    this->password = params->password == nullptr? DEFAULT_PASSWORD : params->password;
    auto iosFlags = std::ios::binary | ((this->encoder)? std::ios::in : std::ios::out);
    // Set up encryption
    if(this->flags & STEGO_ENABLE_ENCRYPTION) {
        std::vector<uint8_t> bytes = deriveBytes(CryptoFile::KeyLength + CryptoFile::BlockSize, CRYPTO_SALT);
        datafile = CryptoFile(params->filename, &bytes[0], &bytes[CryptoFile::KeyLength], iosFlags);
    } else {
        datafile = CryptoFile(params->filename, iosFlags);
    }
}

void Algorithm::encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride) {
    for (int mb_y = 0; mb_y < mb_height; ++mb_y) {
        for (int mb_x = 0; mb_x < mb_width; ++mb_x) {
            int xy = mb_y * mv_stride + mb_x;
            if (mb_type[xy] == 2) {
                embedToPair(&mvs[xy][0], &mvs[xy][1]);
            }
        }
    }
}

void Algorithm::decode(int16_t (*mvs[2])[2], uint32_t *mbtype_table, int mv_sample_log2, int mb_width, int mb_height,
                      int mv_stride, int mb_stride) {
    for (int mb_y = 0; mb_y < mb_height; mb_y++) {
        for (int mb_x = 0; mb_x < mb_width; mb_x++) {
            int xy = (mb_x + mb_y * mv_stride) << mv_sample_log2;
            if(mbtype_table[mb_x + mb_y * mb_stride] != 1) {
                extractFromPair(mvs[0][xy][0], mvs[0][xy][1]);
            }
        }
    }
}

std::vector<uint8_t> Algorithm::deriveBytes(size_t numBytes, std::string salt) {
    std::vector<byte> derived = std::vector<byte>(numBytes);
    const byte *passwordPtr = reinterpret_cast<const byte*>(this->password.c_str());
    const byte *saltPtr = reinterpret_cast<const byte*>(salt.c_str());

    CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> pbkdf2;
    pbkdf2.DeriveKey(&derived[0], numBytes, 0, passwordPtr, this->password.length(), saltPtr, salt.length(), 100000);
    
    return derived;
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

int stego_init_algorithm(const char *algname) {
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
    } else {
        return 1;
    }
    return 0;
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