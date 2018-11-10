#include "RandomisedHideSeek.h"

#include <assert.h>

extern "C" {
#include <rscode/ecc.h>
#define BLOCKSIZE (8 * CryptoFile::BlockSize + NPAR)
}

void RandomisedHideSeek::initAsEncoder(stego_params *params) {
    Algorithm::initAsEncoder(params);
    if(!(flags & STEGO_DUMMY_PASS)) {
        initialize_ecc();

        fileSize = datafile.remainingData();

        // Total size of embedded data:
        // fileSize + NPAR parity bytes for every (BLOCKSIZE - NPAR) bytes of the file
        uint blocks = (fileSize / (BLOCKSIZE - NPAR)) + (fileSize % (BLOCKSIZE - NPAR) != 0);
        dataSize = blocks * NPAR + fileSize;

        initialiseMapping(static_cast<AlgOptions*>(params->algParams), dataSize);

        // Fill the data buffer with blocks of file data & parity bytes
        data = new unsigned char[dataSize];
        unsigned char fileData[BLOCKSIZE - NPAR];
        uint currentPos = 0;
        while(!datafile.eof() && currentPos < dataSize) {
            auto read = datafile.read(&fileData[0], BLOCKSIZE - NPAR);
            encode_data(fileData, read, data + currentPos);
            currentPos += read + NPAR;
        }
    }
}

void RandomisedHideSeek::initAsDecoder(stego_params *params) {
    Algorithm::initAsDecoder(params);
    if(!(flags & STEGO_DUMMY_PASS)) {
        initialize_ecc();
        AlgOptions *opt = static_cast<AlgOptions*>(params->algParams);
        fileSize = opt->fileSize;
        
        // Total size of embedded data:
        // fileSize + NPAR parity bytes for every (BLOCKSIZE - NPAR) bytes of the file
        uint blocks = (fileSize / (BLOCKSIZE - NPAR)) + (fileSize % (BLOCKSIZE - NPAR) != 0);
        dataSize = blocks * NPAR + fileSize;
        
        initialiseMapping(opt, dataSize);
        
        data = new unsigned char[dataSize]();
    }
}

void RandomisedHideSeek::initialiseMapping(AlgOptions *algParams, uint dataSize) {
    // Build a mapping from a data bit to the particular MV
    uint64_t capacity = algParams->byteCapacity;

    assert(encoder || dataSize <= capacity);

    std::vector<uint8_t> seedData = this->deriveBytes(SEED_SIZE, "StegoRandSeed");
    std::seed_seq seed(seedData.begin(), seedData.end());
    std::default_random_engine rng(seed);

    ulong bitCapacity = ((ulong) capacity) * 8;
    std::uniform_int_distribution<ulong> dist(0, bitCapacity);
    ulong bitDataSize = ((ulong) dataSize) * 8;
    bitToMvMapping = new Pair[bitDataSize];

    std::vector<bool> used(bitCapacity, false);

    for(ulong i = 0; i < bitDataSize; ++i) {
        ulong mvNum = dist(rng);
        while(used[mvNum]) {
            ++mvNum;
            if(mvNum >= bitCapacity) mvNum = 0;
        }
        used[mvNum] = true;
        bitToMvMapping[i] = Pair { i,  mvNum };
    }

    // Sort the mapping in increasing order of MVs, for sequential embedding
    std::sort(bitToMvMapping, bitToMvMapping + bitDataSize);
}

void RandomisedHideSeek::embedIntoMv(int16_t *mv) {
    if(flags & STEGO_DUMMY_PASS) {
        bits_processed++;
    } else {
        if(index >= 8*dataSize) return;
        if(bits_processed == bitToMvMapping[index].mv) {
            // We found a MV that's next on a list to be modified.
            ulong dataBit = bitToMvMapping[index].bit;
            int bit = data[dataBit / 8] >> (dataBit % 8);

            if((bit & 1) && !(*mv & 1)) (*mv)++;
            if(!(bit & 1) && (*mv & 1)) (*mv)--;

            index++;
        }
        bits_processed++;
    }
}

void RandomisedHideSeek::extractFromMv(int16_t val) {
    if(index >= 8*dataSize) return;
    if (bits_processed == bitToMvMapping[index].mv) {
        // We found a MV that was next on a list to be modified.
        ulong dataBit = bitToMvMapping[index].bit;
        data[dataBit / 8] |= (val & 1) << (dataBit % 8);
        index++;
    }

    bits_processed++;
}

stego_result RandomisedHideSeek::finalise() {
    if(!encoder && !(flags & STEGO_DUMMY_PASS)) {
        uint currentPos = 0;
        while(currentPos < dataSize) {
            uint blockSize = std::min((uint)BLOCKSIZE, dataSize - currentPos);
            decode_data(data + currentPos, blockSize);
            if (check_syndrome() != 0) {
                correct_errors_erasures(data + currentPos, blockSize, 0, NULL);
            }
            datafile.write(&data[0] + currentPos, blockSize - NPAR);
            currentPos += blockSize;
        }
    }
    delete data;
    delete bitToMvMapping;
    return Algorithm::finalise();
}
