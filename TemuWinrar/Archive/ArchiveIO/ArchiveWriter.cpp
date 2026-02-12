#include "ArchiveWriter.h"
#include "../Compression/HuffmanCompressor.h"

#include <stdexcept>
#include <cstring>

void ArchiveWriter::writeAll(std::ostream& os, const void* data, size_t n) {
    os.write((const char*)data, (std::streamsize)n);
    if (!os) throw std::runtime_error("write failed");
}

//Todo use the member funtion from ArchiveReader
void ArchiveWriter::readAll(std::istream& is, void* data, size_t n) {
    is.read((char*)data, (std::streamsize)n);
    if (!is) throw std::runtime_error("read failed");
}

uint64_t ArchiveWriter::tellp64(std::ostream& os) {
    auto p = os.tellp();
    if (p < 0) throw std::runtime_error("tellp failed");
    return (uint64_t)p;
}

//Todo use the member funtion from ArchiveReader

/*
  USE ONLY FOR SINGLE FILE INPUTS.
  Don't use on the whole archive and or multiple files
*/
std::vector<uint8_t> ArchiveWriter::readWholeFile(const std::filesystem::path& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) throw std::runtime_error("open fail");

    in.seekg(0, std::ios::end);
    size_t sz = (size_t)in.tellg();
    in.seekg(0);

    std::vector<uint8_t> v(sz);
    if (sz) readAll(in, v.data(), sz);
    return v;
}

//stringify path
std::string ArchiveWriter::toArchivePath(const std::filesystem::path& p) {
    return p.generic_string();
}

//Todo use reader's version
void ArchiveWriter::hashPayload(const std::vector<uint8_t>& payload, uint8_t outHash[arch::HASH_SIZE]) {
    std::hash<std::string_view> H;
    std::string_view sv((const char*)payload.data(), payload.size());
    size_t h = H(sv);
    std::memset(outHash, 0, arch::HASH_SIZE);
    std::memcpy(outHash, &h, sizeof(h));
}

//Todo use reader's version - better
bool ArchiveWriter::readMasterOffsets(const std::string& path, std::vector<uint64_t>& offs, uint64_t& masterOff) {
    offs.clear();
    masterOff = 0;

    if (!std::filesystem::exists(path)) return false;

    std::ifstream in(path, std::ios::binary);
    unsigned long long sz = std::filesystem::file_size(path);
    if (sz < sizeof(arch::FooterFixed)) return false;

    in.seekg(sz - sizeof(arch::FooterFixed));
    arch::FooterFixed f{};
    readAll(in, &f, sizeof(f));
    if (f.magic != arch::MAGIC_FTR1) return false;

    masterOff = f.masterOffset;

    in.seekg(masterOff);
    arch::MasterHeaderFixed mh{};
    readAll(in, &mh, sizeof(mh));

    offs.resize(mh.count);
    if (mh.count) readAll(in, offs.data(), mh.count * sizeof(uint64_t));
    return true;
}

//Todo use reader's version - readLocalHeaderAndName
std::string ArchiveWriter::readNameAt(const std::string& path, uint64_t off, arch::LocalHeaderFixed& h) {
    std::ifstream in(path, std::ios::binary);
    in.seekg(off);
    readAll(in, &h, sizeof(h));
    std::string name(h.nameLen, '\0');
    if (h.nameLen) readAll(in, name.data(), h.nameLen);
    return name;
}

void ArchiveWriter::writeMasterAndFooter(std::ofstream& out,
    const std::vector<EntryRef>& entries) {
    uint64_t mo = tellp64(out);

    arch::MasterHeaderFixed mh{ arch::MAGIC_MHDR, entries.size() };
    writeAll(out, &mh, sizeof(mh));

    for (auto& e : entries)
        writeAll(out, &e.offset, sizeof(uint64_t));

    arch::FooterFixed f{ arch::MAGIC_FTR1, mo };
    writeAll(out, &f, sizeof(f));
}

uint64_t ArchiveWriter::appendDirEntry(std::ofstream& out, const std::string& name) {
    uint64_t off = tellp64(out);

    arch::LocalHeaderFixed h{};
    h.magic = arch::MAGIC_LHDR;
    h.flags = arch::FLAG_IS_DIR;
    h.nameLen = (uint32_t)name.size();
    std::memset(h.hash, 0, arch::HASH_SIZE);

    writeAll(out, &h, sizeof(h));
    writeAll(out, name.data(), name.size());
    return off;
}

uint64_t ArchiveWriter::appendFileEntry(std::ofstream& out, const std::filesystem::path& filePath) {
    std::vector<uint8_t> newFileRaw = readWholeFile(filePath);
    HuffmanCompressor hc;
    std::vector<uint8_t> blob = hc.compress(newFileRaw);

    out.clear();
    out.seekp(0, std::ios::end);
    std::streampos pos = out.tellp();
    if (pos < 0) {
        throw std::runtime_error("appendFileEntry: tellp failed (invalid stream position)");
    }
    uint64_t off = static_cast<uint64_t>(pos);      //

    std::string name = toArchivePath(filePath);

    arch::LocalHeaderFixed h{};
    h.magic = arch::MAGIC_LHDR;
    h.originalSize = newFileRaw.size();
    h.compressedSize = blob.size();
    h.nameLen = (uint32_t)name.size();
    hashPayload(blob, h.hash);

    writeAll(out, &h, sizeof(h));
    writeAll(out, name.data(), name.size());
    writeAll(out, blob.data(), blob.size());
    return off;
}

void ArchiveWriter::addOrFail(std::vector<EntryRef>& v, const std::string& name, uint64_t off)
{
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (v[i].name == name)
            throw std::runtime_error("Duplicate entry name: " + name);
    }
    v.push_back(EntryRef{ name, off });
}

void ArchiveWriter::loadEntryRefs(const std::string& path, const std::vector<uint64_t>& offs, std::vector<EntryRef>& out)
{
    out.clear();

    for (size_t i = 0; i < offs.size(); ++i)
    {
        uint64_t off = offs[i];

        arch::LocalHeaderFixed h{};
        std::string name = readNameAt(path, off, h);

        EntryRef e;
        e.name = name;
        e.offset = off;

        out.push_back(e);
    }
}


void ArchiveWriter::appendPathRecursive(std::ofstream& out,
    const std::filesystem::path& p,
    std::vector<EntryRef>& entries)
{

    if (std::filesystem::is_directory(p))
    {
        std::string dn = toArchivePath(p) + "/";
        uint64_t dnOff = appendDirEntry(out, dn);
        addOrFail(entries, dn, dnOff);

        for (std::filesystem::recursive_directory_iterator it(p), end; it != end; ++it)
        {
            const std::filesystem::directory_entry& de = *it;

            if (de.is_directory())
            {
                std::string d = toArchivePath(de.path()) + "/";
                uint64_t off = appendDirEntry(out, d);
                addOrFail(entries, d, off);
            }
            else if (de.is_regular_file())
            {
                std::string n = toArchivePath(de.path());
                uint64_t off = appendFileEntry(out, de.path());
                addOrFail(entries, n, off);
            }
        }
    }
    else if (std::filesystem::is_regular_file(p))
    {
        std::string n = toArchivePath(p);
        uint64_t off = appendFileEntry(out, p);
        addOrFail(entries, n, off);
    }
}


void ArchiveWriter::createEmpty(const std::string& path) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    std::vector<EntryRef> empty;
    writeMasterAndFooter(out, empty);
}

bool ArchiveWriter::containsEntry(const std::vector<EntryRef>& entries, const std::string& name)
{
    for (const auto& e : entries)
        if (e.name == name) return true;
    return false;
}


void ArchiveWriter::checkConflictOrThrow(const std::string& n, const std::vector<EntryRef>& entries)
{
    if (containsEntry(entries, n))
        throw std::runtime_error("Entry already exists in archive: " + n);
}

void ArchiveWriter::scanForConflictsOrThrow(const std::filesystem::path& p, const std::vector<EntryRef>& entries)
{

    if (std::filesystem::is_directory(p))
    {
        std::string rootName = toArchivePath(p) + "/";
        checkConflictOrThrow(rootName, entries);

        std::filesystem::recursive_directory_iterator it(p);
        std::filesystem::recursive_directory_iterator end;

        for (; it != end; ++it)
        {
            const std::filesystem::directory_entry& de = *it;

            if (std::filesystem::is_directory(de.path()))
            {
                std::string d = toArchivePath(de.path()) + "/";
                checkConflictOrThrow(d, entries);
            }
            else if (std::filesystem::is_regular_file(de.path()))
            {
                std::string f = toArchivePath(de.path());
                checkConflictOrThrow(f, entries);
            }
        }
    }
    else if (std::filesystem::is_regular_file(p))
    {
        std::string name = toArchivePath(p);
        checkConflictOrThrow(name, entries);
    }
    else
    {
        throw std::runtime_error(
            "Path is neither file nor directory: " + p.string());
    }
}


void ArchiveWriter::append(const std::string& archivePath, const std::string& fileFath) {

    if (!std::filesystem::exists(fileFath)) {
        throw std::invalid_argument("File path doesnt exist.");
    }

    std::vector<uint64_t> offs;
    uint64_t masterOff = 0;

    if (!readMasterOffsets(archivePath, offs, masterOff)) {
        createEmpty(archivePath);
        readMasterOffsets(archivePath, offs, masterOff);
    }

    std::vector<EntryRef> entries;
    loadEntryRefs(archivePath, offs, entries);

    //Todo optimize
    scanForConflictsOrThrow(std::filesystem::path(fileFath), entries);  //scands for dublicate names


    std::filesystem::resize_file(archivePath, masterOff);

    std::ofstream out(archivePath, std::ios::binary | std::ios::app);

    appendPathRecursive(out, std::filesystem::path(fileFath), entries);

    writeMasterAndFooter(out, entries);
}
