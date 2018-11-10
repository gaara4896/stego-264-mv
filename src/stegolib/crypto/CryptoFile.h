//
// Created by el on 08/02/16.
//

#ifndef STEGO_CRYPTOFILE_H
#define STEGO_CRYPTOFILE_H

#include <fstream>
#include <aes.h>
#include <modes.h>

/**
 * A file-like object to encrypt and decrypt data on the fly.
 */
class CryptoFile {
public:
    /**
     * Block size of the cipher.
     */
    static const int BlockSize = CryptoPP::AES::BLOCKSIZE;

    /**
     * Key length used by the encryption algorithm.
     */
    static const int KeyLength = CryptoPP::AES::MAX_KEYLENGTH;

    CryptoFile() {};

    /**
     * Constructs an object with encryption disabled.
     * @param path Path to a file.
     * @param mode File open mode, e.g std::ios::in
     */
    CryptoFile(const std::string &path, std::ios_base::openmode mode);

    /**
     * Constructs an object with AES-CTR encryption enabled.
     *
     * @param path Path to a file.
     * @param key An encryption key.
     * @param iv An initialisation vector.
     * @param mode File open mode, e.g std::ios::in
     */
    CryptoFile(const std::string &path, uint8_t key[KeyLength], uint8_t iv[BlockSize], std::ios::openmode mode);

    /**
     * Reads data from the file into a buffer.
     * If encryption is enabled, reading encrypts before being returned to the caller.
     * @param out Output buffer.
     * @param size Number of bytes to read.
     */
    std::streamsize read(uint8_t *out, uint size);

    /**
     * Writes data from a buffer into a file.
     * If encryption is enabled, the output buffer is decrypted before writing.
     *
     * @param out Output buffer.
     * @param size Number of bytes to read.
     */
    std::streamsize write(const uint8_t *out, uint size);

    /**
     * Number of bytes available for reading.
     */
    uint remainingData();

    /**
     * Flush the underlying data stream to the file.
     */
    void flush();

    /**
     * Closes the underlying file.
     */
    void close();

    /**
     * Checks whether we reached the end of file.
     */
    bool eof();

    /**
     * Exposes the underlying file stream, encryption will not be performed.
     */
    std::fstream& exposeStream();
    
private:
    std::shared_ptr<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption> enc;
    std::shared_ptr<CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption> dec;
    uint8_t key[KeyLength], iv[BlockSize];
    bool encrypt;
    std::fstream stream;
};

#endif //STEGO_CRYPTOFILE_H
