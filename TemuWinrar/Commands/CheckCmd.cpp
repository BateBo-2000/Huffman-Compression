#include "CheckCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"


CheckCmd::CheckCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("check"), ui(ui), archive(archive)
{
}

void CheckCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() != 2) {
        ui.error("Usage: check <archive>");
        return;
    }

    try {
        std::string result = archive.check(args[1]);
        ui.inform(result);
    }
    catch (const std::exception& e) {
        ui.error(std::string("Check failed: ") + e.what());
    }
}

std::string CheckCmd::description() const
{
    return "Checks archive integrity.";
}
