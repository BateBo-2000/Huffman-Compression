#include "UnzipCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"

UnzipCmd::UnzipCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("unzip"), ui(ui), archive(archive)
{
}

void UnzipCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() != 2) {
        ui.error("Usage: unzip <archive> <outDir>");
        return;
    }

    try {
        archive.unzip(args[0], args[1]);
        ui.inform("Unzip completed.");
    }
    catch (const std::exception& e) {
        ui.error(std::string("Unzip failed: ") + e.what());
    }
}

std::string UnzipCmd::description() const
{
    return "Extracts files from archive.";
}
