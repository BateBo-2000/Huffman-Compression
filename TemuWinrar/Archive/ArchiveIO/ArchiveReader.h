#pragma once
#include <string>
#include <vector>
#include "ArchiveCommon.h"

class ArchiveReader {
public:
    static std::string info(const std::string& archivePath);
    static std::string check(const std::string& archivePath);
    static void unzip(const std::string& archivePath, const std::string& outDir);

private:
    static void readAll(std::istream& is, void* data, size_t n);
    static uint64_t fileSize64(const std::string& path);

    static void readMasterOffsets(const std::string& archivePath,
        std::vector<uint64_t>& offsetsOut);

    static void readLocalHeaderAndName(std::ifstream& in,
        uint64_t offset,
        arch::LocalHeaderFixed& hOut,
        std::string& nameOut);

    static void readPayload(std::ifstream& in,
        const arch::LocalHeaderFixed& h,
        std::vector<uint8_t>& payloadOut);

    static void hashPayload(const std::vector<uint8_t>& payload,
        uint8_t outHash[arch::HASH_SIZE]);

    static bool hashEquals(const uint8_t a[arch::HASH_SIZE], const uint8_t b[arch::HASH_SIZE]);

    static void ensureParentDirs(const std::string& fullPath);
};
