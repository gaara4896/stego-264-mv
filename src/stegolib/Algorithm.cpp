#include <iostream>
#include <cstring>
#include <sha.h>
#include <pwdbased.h>
#include <osrng.h>

#include "Algorithm.h"
#include "stego_connector.h"

#define CRYPTO_SALT "StegoEncryption"
#define DEFAULT_PASSWORD "StegoDefaultPassword"

void Algorithm::initAsEncoder(stego_params *params) {
    this->encoder = true;
    this->initialiseAlgorithm(params);
    if(params->datafile != nullptr && (this->flags & STEGO_ENABLE_ENCRYPTION)) {
        CryptoPP::AutoSeededRandomPool rnd;
        uint8_t iv[CryptoFile::BlockSize];
        rnd.GenerateBlock(iv, sizeof(iv));
        datafile.setIv(&iv[0]);
    }
}

void Algorithm::initAsDecoder(stego_params *params) {
    this->encoder = false;
    this->initialiseAlgorithm(params);
}

void Algorithm::initialiseAlgorithm(stego_params *params) {
    // Unpack parameters
    this->flags = params->flags;
    if((this->flags & STEGO_ENABLE_ENCRYPTION) && params->password == nullptr) {
        std::cerr << "Using default password for encryption, "
                     "please specify a password to secure your embedding." << std::endl;
    }
    this->password = params->password == nullptr? DEFAULT_PASSWORD : params->password;
    auto iosFlags = std::ios::binary | ((this->encoder)? std::ios::in : std::ios::out);

    // Set up encryption
    if(params->datafile != nullptr) {
        if(this->flags & STEGO_ENABLE_ENCRYPTION) {
            std::vector<uint8_t> bytes = deriveBytes((size_t) CryptoFile::KeyLength, CRYPTO_SALT);
            datafile = CryptoFile(params->datafile, &bytes[0], iosFlags);
        } else {
            datafile = CryptoFile(params->datafile, iosFlags);
        }
    }
}

void Algorithm::encode(int16_t (*mvs)[2], uint16_t *mb_type, int mb_width, int mb_height, int mv_stride) {
    for (int mb_y = 0; mb_y < mb_height; ++mb_y) {
        for (int mb_x = 0; mb_x < mb_width; ++mb_x) {
            int xy = mb_y * mv_stride + mb_x;
            if (mb_type[xy] == 2) {
                embedIntoMv(&mvs[xy][0], &mvs[xy][1]);
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
                extractFromMv(mvs[0][xy][0], mvs[0][xy][1]);
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

unsigned int Algorithm::computeEmbeddingSize(unsigned int dataSize) {
    return CryptoFile::encryptedFileSize(dataSize, (bool)this->flags & STEGO_ENABLE_ENCRYPTION);
}