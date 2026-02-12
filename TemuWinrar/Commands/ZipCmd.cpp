#include "ZipCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"

ZipCmd::ZipCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("zip"), ui(ui), archive(archive)
{
}

void ZipCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() < 3) {
        ui.error("Usage: zip <arhcive path> <file path1> [<file path2+>] ...");
        return;
    }
    try {
        const std::string& archivePath = args[1];

        std::vector<std::string> inputs;
        for (size_t i = 2; i < args.size(); i++)
        {
            inputs.push_back(args[i]);
        }

        for (size_t i = 0; i < inputs.size(); i++)
        {
                archive.zip(archivePath, inputs[i]);
                ui.inform("File "+inputs[i] + " added to archive.");
        }
    }
    catch (const std::exception& e) {
        ui.warn(std::string("Failed to append next file to Archvie.") + e.what());
        throw;
    }
}

std::string ZipCmd::description() const
{
    return "Creates an archive from files.\nFormat: zip <arhcive path> <file path1> [<file path2+>] ...";
}
