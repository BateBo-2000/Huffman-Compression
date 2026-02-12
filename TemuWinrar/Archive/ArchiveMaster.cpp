#include "ArchiveMaster.h"
#include "ArchiveIO/ArchiveWriter.h"
#include "ArchiveIO/ArchiveReader.h"

#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

void ArchiveMaster::zip(const std::string& archivePath,
    const std::vector<std::string>& inputs) {
    if (!fs::exists(archivePath)) {
        ArchiveWriter::createEmpty(archivePath);
    }

    ArchiveWriter::append(archivePath, inputs);
}

void ArchiveMaster::unzip(const std::string& archivePath,
    const std::string& outDir) {
    ArchiveReader::unzip(archivePath, outDir);
}

std::string ArchiveMaster::info(const std::string& archivePath) {
    return ArchiveReader::info(archivePath);
}

std::string ArchiveMaster::check(const std::string& archivePath) {
    return ArchiveReader::check(archivePath);
}

void ArchiveMaster::update(const std::string&,
    const std::vector<std::string>&) {
    throw std::runtime_error("update command not implemented yet");
}
