#pragma once
#include <cstdint>

namespace arch {


    static constexpr size_t   HASH_SIZE = 8;            // = size_of(size_t)
    static constexpr uint32_t MAGIC_LHDR = 0x5244484Cu; // 'LHDR'
    static constexpr uint32_t MAGIC_MHDR = 0x5244484Du; // 'MHDR'
    static constexpr uint32_t MAGIC_FTR1 = 0x31525446u; // 'FTR1'


    enum : uint8_t {
        FLAG_IS_DIR = 1 << 0,   // directory entry (no payload)
    };

#pragma pack(push,1)   //https://learn.microsoft.com/en-us/cpp/preprocessor/pack?view=msvc-170



    // Before each entry
    struct LocalHeaderFixed
    {
        uint32_t magic;            // MAGIC_LHDR
        uint8_t  flags;            // FLAG_IS_DIR
        uint64_t originalSize;     // bytes before compression (0 for dir)
        uint64_t compressedSize;   // payload bytes (0 for dir)
        uint8_t  hash[HASH_SIZE];  // hash of payload (compressed blob) OR original bytes (your choice)
        uint32_t nameLen;          // bytes of name after header
        // then: name[nameLen]
        // then: payload[compressedSize]
    };

    // At the end - list of offsets to LocalHeaderFixed (ACTIVE set)
    struct MasterHeaderFixed
    {
        uint32_t magic;            // MAGIC_MHDR
        uint64_t count;            // number of entries
        // then: uint64_t offsets[count]; // each points to a LocalHeaderFixed
    };

    // At the very end:
    struct FooterFixed
    {
        uint32_t magic;            // MAGIC_FTR1
        uint64_t masterOffset;     // file offset where MasterHeaderFixed begins
    };
#pragma pack(pop)   

    // Small comon helper functions
    inline bool isDir(uint8_t flags) noexcept { 
        return (flags & FLAG_IS_DIR) != 0; 
    }

    inline bool headerLooksValid(const LocalHeaderFixed& h) noexcept {
        if (h.magic != MAGIC_LHDR) return false;
        if (isDir(h.flags) && (h.originalSize != 0 || h.compressedSize != 0)) return false;
        return true;
    }

}
