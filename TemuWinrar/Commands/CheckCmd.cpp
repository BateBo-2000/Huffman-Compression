#include "CheckCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/Archive.h"
#include <sstream>

CheckCmd::CheckCmd(UserInterface& ui, Archive& archive)
    : Command("check"), ui(ui), archive(archive)
{
}

void CheckCmd::execute(const std::vector<std::string>& args)
{
    ui.inform("check execute");
}

std::string CheckCmd::description() const
{
    return "Checks archive integrity (skeleton).";
}
