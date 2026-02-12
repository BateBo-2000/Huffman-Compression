#include "UpdateCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/ArchiveMaster.h"

UpdateCmd::UpdateCmd(UserInterface& ui, ArchiveMaster& archive)
    : Command("update"), ui(ui), archive(archive)
{
}

void UpdateCmd::execute(const std::vector<std::string>& args)
{
    if (args.size() < 3 || args.size()>4) {
        ui.error("Usage: update <archive> <fileToRemove> [<fileToAddOnHisPlace>]");
        return;
    }

    try {
        std::string arg3 = (args.size() == 3 ? "" : args[3]);
        archive.update(args[1], args[2], arg3);
        ui.inform("Update completed.");
    }
    catch (const std::exception& e) {
        ui.error(std::string("Update failed: ") + e.what());
    }
}

std::string UpdateCmd::description() const
{
    return "Updates files inside an archive (still not implemented).\nFormat:update <archive> <fileInArchive> [<newUncompressedFile>].\nIf not given new file location it will just remove the file given.";
}
