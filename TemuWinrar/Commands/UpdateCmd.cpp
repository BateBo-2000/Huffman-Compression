#include "UpdateCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/Archive.h"

UpdateCmd::UpdateCmd(UserInterface& ui, Archive& archive)
    : Command("update"), ui(ui), archive(archive)
{
}

void UpdateCmd::execute(const std::vector<std::string>& args)
{
    ui.inform("update execute");
}

std::string UpdateCmd::description() const
{
    return "Updates files inside an archive (skeleton).";
}
