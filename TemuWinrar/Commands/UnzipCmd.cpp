#include "UnzipCmd.h"
#include "../UI/UserInterface.h"
#include "../Archive/Archive.h"

UnzipCmd::UnzipCmd(UserInterface& ui, Archive& archive)
    : Command("unzip"), ui(ui), archive(archive)
{
}

void UnzipCmd::execute(const std::vector<std::string>& args)
{
    ui.inform("unzip execute");
}

std::string UnzipCmd::description() const
{
    return "Extracts files from archive (skeleton).";
}
