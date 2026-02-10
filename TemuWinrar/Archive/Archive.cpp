#include "Archive.h"

#include <iostream>

void Archive::zip()
{
	std::cout << "Archive Zip";
}
 
void Archive::unzip()
{
	std::cout << "Archive Unzip";
}

void Archive::check() const
{
	std::cout << "Archive check";
}

void Archive::info() const
{
	std::cout << "Archive info";
}

void Archive::update()
{
	std::cout << "Archive update";
}
