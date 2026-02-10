#include "ZipCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/Archive.h"

ZipCmd::ZipCmd(UserInterface& ui, Archive& archive)
    : Command("zip"), ui(ui), archive(archive)
{
}

void ZipCmd::execute(const std::vector<std::string>& args)
{
    ui.inform("zip execute");
}

std::string ZipCmd::description() const
{
    return "Creates an archive from files (skeleton).";
}
