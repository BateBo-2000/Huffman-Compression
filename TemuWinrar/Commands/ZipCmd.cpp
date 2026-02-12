#include "ZipCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"

ZipCmd::ZipCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("zip"), ui(ui), archive(archive)
{
}

void ZipCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        ui.error("Usage: zip <archive> <files/dirs...>");
        return;
    }

    try {
        const std::string& archivePath = args[0];

        std::vector<std::string> inputs(
            args.begin() + 1,
            args.end()
        );

        archive.zip(archivePath, inputs);

        ui.inform("Zip completed.");
    }
    catch (const std::exception& e) {
        ui.error(std::string("Zip failed: ") + e.what());
    }
}

std::string ZipCmd::description() const
{
    return "Creates an archive from files.";
}
