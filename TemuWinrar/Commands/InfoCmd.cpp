#include "InfoCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/Archive.h"

InfoCmd::InfoCmd(UserInterface& ui, Archive& archive)
    : Command("info"), ui(ui), archive(archive)
{
}

void InfoCmd::execute(const std::vector<std::string>& args)
{
    ui.inform("info execute");
}

std::string InfoCmd::description() const
{
    return "Shows archive info (skeleton).";
}
