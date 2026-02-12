#include "UpdateCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"

UpdateCmd::UpdateCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("update"), ui(ui), archive(archive)
{
}

void UpdateCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() < 1) {
        ui.error("Usage: update <archive> <files...>");
        return;
    }

    try {
        std::vector<std::string> inputs;
        if (args.size() > 1)
            inputs.assign(args.begin() + 1, args.end());

        archive.update(args[0], inputs);
        ui.inform("Update completed."); // practically won't happen now
    }
    catch (const std::exception& e) {
        ui.error(std::string("Update failed: ") + e.what());
    }
}

std::string UpdateCmd::description() const
{
    return "Updates files inside an archive (still not implemented).";
}
