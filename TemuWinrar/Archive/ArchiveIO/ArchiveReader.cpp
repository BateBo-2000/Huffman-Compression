#include "ArchiveReader.h"
#include "../Compression/HuffmanCompressor.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <filesystem>

void ArchiveReader::readAll(std::istream& is, void* data, size_t n) {
    is.read(reinterpret_cast<char*>(data), (std::streamsize)n);
    if (!is) throw std::runtime_error("ArchiveReader: read failed");
}

uint64_t ArchiveReader::fileSize64(const std::string& path) {
    return (uint64_t)std::filesystem::file_size(path);
}

void ArchiveReader::readMasterOffsets(const std::string& archivePath,
    std::vector<uint64_t>& offsetsOut) {
    offsetsOut.clear();

    if (!std::filesystem::exists(archivePath))
        throw std::runtime_error("ArchiveReader: archive does not exist");

    std::ifstream in(archivePath, std::ios::binary);
    if (!in) throw std::runtime_error("ArchiveReader: cannot open archive");

    uint64_t sz = fileSize64(archivePath);
    if (sz < sizeof(arch::FooterFixed))
        throw std::runtime_error("ArchiveReader: archive too small (no footer)");

    in.seekg((std::streamoff)(sz - sizeof(arch::FooterFixed)), std::ios::beg);
    arch::FooterFixed f{};
    readAll(in, &f, sizeof(f));
    if (f.magic != arch::MAGIC_FTR1)
        throw std::runtime_error("ArchiveReader: bad footer magic");

    in.seekg((std::streamoff)f.masterOffset, std::ios::beg);
    arch::MasterHeaderFixed mh{};
    readAll(in, &mh, sizeof(mh));
    if (mh.magic != arch::MAGIC_MHDR)
        throw std::runtime_error("ArchiveReader: bad master magic");

    offsetsOut.resize((size_t)mh.count);
    if (mh.count)
        readAll(in, offsetsOut.data(), offsetsOut.size() * sizeof(uint64_t));
}

void ArchiveReader::readLocalHeaderAndName(std::ifstream& in,
    uint64_t offset,
    arch::LocalHeaderFixed& hOut,
    std::string& nameOut) {
    in.seekg((std::streamoff)offset, std::ios::beg);
    readAll(in, &hOut, sizeof(hOut));

    if (!arch::headerLooksValid(hOut))
        throw std::runtime_error("ArchiveReader: corrupt LocalHeader");

    nameOut.assign(hOut.nameLen, '\0');
    if (hOut.nameLen)
        readAll(in, nameOut.data(), hOut.nameLen);
}

void ArchiveReader::readPayload(std::ifstream& in,
    const arch::LocalHeaderFixed& h,
    std::vector<uint8_t>& payloadOut) {
    payloadOut.clear();
    if (arch::isDir(h.flags)) return;

    payloadOut.resize((size_t)h.compressedSize);
    if (h.compressedSize)
        readAll(in, payloadOut.data(), payloadOut.size());
}

void ArchiveReader::hashPayloadStdHash(const std::vector<uint8_t>& payload,
    uint8_t outHash[arch::HASH_SIZE]) {
    std::hash<std::string_view> H;
    std::string_view sv(reinterpret_cast<const char*>(payload.data()), payload.size());
    size_t h = H(sv);

    std::memset(outHash, 0, arch::HASH_SIZE);
    std::memcpy(outHash, &h, sizeof(h)); // demo: HASH_SIZE=8
}

bool ArchiveReader::hashEquals(const uint8_t a[arch::HASH_SIZE], const uint8_t b[arch::HASH_SIZE]) {
    return std::memcmp(a, b, arch::HASH_SIZE) == 0;
}

void ArchiveReader::ensureParentDirs(const std::string& fullPath) {
    std::filesystem::path p(fullPath);
    std::filesystem::path parent = p.parent_path();
    if (!parent.empty())
        std::filesystem::create_directories(parent);
}

std::string ArchiveReader::info(const std::string& archivePath) {
    std::vector<uint64_t> offsets;
    readMasterOffsets(archivePath, offsets);

    std::ifstream in(archivePath, std::ios::binary);
    if (!in) throw std::runtime_error("ArchiveReader: cannot open archive");

    std::ostringstream out;

    out << "Archive: " << archivePath << "\n";
    out << "Entries: " << offsets.size() << "\n\n";

    for (uint64_t off : offsets) {
        arch::LocalHeaderFixed h{};
        std::string name;
        readLocalHeaderAndName(in, off, h, name);

        out << (arch::isDir(h.flags) ? "[DIR ] " : "[FILE] ") << name << "\n";
        out << "  original:   " << h.originalSize << " bytes\n";
        out << "  compressed: " << h.compressedSize << " bytes\n";

        if (!arch::isDir(h.flags) && h.originalSize > 0) {
            double ratio = 100.0 * (double)h.compressedSize / (double)h.originalSize;
            out << "  ratio:      " << ratio << "%\n";
        }

        out << "\n";
    }

    return out.str();
}


std::string ArchiveReader::check(const std::string& archivePath) {
    std::vector<uint64_t> offsets;
    readMasterOffsets(archivePath, offsets);

    std::ifstream in(archivePath, std::ios::binary);
    if (!in) throw std::runtime_error("ArchiveReader: cannot open archive");


    std::string res;
    res.clear();

    for (uint64_t off : offsets) {
        arch::LocalHeaderFixed h{};
        std::string name;
        readLocalHeaderAndName(in, off, h, name);

        if (arch::isDir(h.flags)) continue;

        std::vector<uint8_t> payload;
        readPayload(in, h, payload);

        uint8_t calc[arch::HASH_SIZE];
        hashPayloadStdHash(payload, calc);

        if (!hashEquals(calc, h.hash)) {
            res+="[CORRUPT] " + name + "\n";
        }
    }

    if (res.size()==0) res += "Check OK: no corrupted entries.\n";
    return res;
}

void ArchiveReader::unzip(const std::string& archivePath, const std::string& outDir) {
    std::vector<uint64_t> offsets;
    readMasterOffsets(archivePath, offsets);

    std::ifstream in(archivePath, std::ios::binary);
    if (!in) throw std::runtime_error("ArchiveReader: cannot open archive");

    HuffmanCompressor hc;

    for (uint64_t off : offsets) {
        arch::LocalHeaderFixed h{};
        std::string name;
        readLocalHeaderAndName(in, off, h, name);

        // Build output path
        std::filesystem::path outPath = std::filesystem::path(outDir) / std::filesystem::path(name);   //This makes <outDir> '/' <name>

        if (arch::isDir(h.flags)) {
            std::filesystem::create_directories(outPath);
            continue;
        }

        // Read compressed payload
        std::vector<uint8_t> blob;
        readPayload(in, h, blob);

        // Decompress
        std::vector<uint8_t> plain = hc.decompress(blob);

        // Ensure dirs + write file
        std::filesystem::path parent = outPath.parent_path();
        if (!parent.empty())
            std::filesystem::create_directories(parent);

        std::ofstream out(outPath, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("ArchiveReader: cannot create output file: " + outPath.string());

        if (!plain.empty())
            out.write(reinterpret_cast<const char*>(plain.data()), (std::streamsize)plain.size());
    }
}
