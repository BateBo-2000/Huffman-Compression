#pragma once
#include <string>
#include <vector>

class ArchiveMaster {
public:
    // zip = create if missing + append inputs
    void zip(const std::string& archivePath, const std::string& input);

    void unzip(const std::string& archivePath, const std::string& outDir);

    std::string info(const std::string& archivePath);

    std::string check(const std::string& archivePath);

    void update(const std::string& archivePath, const std::string& fileToRemove, const std::string& fileToAdd);
};
