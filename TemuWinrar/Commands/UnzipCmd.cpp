#include "UnzipCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"

UnzipCmd::UnzipCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("unzip"), ui(ui), archive(archive)
{
}

void UnzipCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() < 2 || args.size() > 3) {
        ui.error("Usage: unzip <archive> [outDir]");
        return;
    }

    try {
        std::string out;
        if (args.size() == 3) {
            out = args[2];
        }
        else {
            out = "";
        }
        

        archive.unzip(args[1], out);
        ui.inform("Unzip completed. Files unzipped in "+ (out.empty()? std::string("here.") : out));
    }
    catch (const std::exception& e) {
        ui.error(std::string("Unzip failed: ") + e.what());
    }
}

std::string UnzipCmd::description() const
{
    return "Extracts files from archive.\nFormat: unzip <archive> [<outDir>]";
}
