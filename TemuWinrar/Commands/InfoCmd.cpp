#include "InfoCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"

#include <stdexcept>

InfoCmd::InfoCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("info"), ui(ui), archive(archive)
{
}

void InfoCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() != 1) {
        ui.error("Usage: info");
        return;
    }

    try {
        std::string text = archive.info(args[0]);
        ui.inform(text);
    }
    catch (const std::exception& e) {
        ui.error(std::string("Info failed: ") + e.what());
    }
}

std::string InfoCmd::description() const
{
    return "Shows archive info.";
}
