#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <filesystem>

#include "ArchiveCommon.h"

class ArchiveWriter {
public:
    static void createEmpty(const std::string& archivePath);
    static void append(const std::string& archivePath, const std::string& fileFath);

private:
    struct EntryRef {
        std::string name;
        uint64_t offset;
    };

    // IO
    static void writeAll(std::ostream& os, const void* data, size_t n);
    static void readAll(std::istream& is, void* data, size_t n);
    static uint64_t tellp64(std::ostream& os);

    // helpers
    static std::vector<uint8_t> readWholeFile(const std::filesystem::path& p);
    static std::string toArchivePath(const std::filesystem::path& p);
    static void hashPayload(const std::vector<uint8_t>& payload, uint8_t outHash[arch::HASH_SIZE]);

    // master/footer
    static bool readMasterOffsets(const std::string& archivePath, std::vector<uint64_t>& offsetsOut, uint64_t& masterOffsetOut);

    static std::string readNameAt(const std::string& archivePath, uint64_t offset, arch::LocalHeaderFixed& hOut);

    static void writeMasterAndFooter(std::ofstream& out, const std::vector<EntryRef>& entries);

    // entries
    static uint64_t appendDirEntry(std::ofstream& out, const std::string& name);
    static uint64_t appendFileEntry(std::ofstream& out, const std::filesystem::path& p);

    // active list helpers
    static void loadEntryRefs(const std::string& archivePath, const std::vector<uint64_t>& offsets, std::vector<EntryRef>& out);
    static void addOrFail(std::vector<EntryRef>& v, const std::string& name, uint64_t off);
    static void appendPathRecursive(std::ofstream& out, const std::filesystem::path& p, std::vector<EntryRef>& entries);

    //dublicate checkers
    static void scanForConflictsOrThrow(const std::filesystem::path& p, const std::vector<EntryRef>& entries);
    static bool containsEntry(const std::vector<EntryRef>& entries, const std::string& name);
    static void checkConflictOrThrow(const std::string& n, const std::vector<EntryRef>& entries);
};
