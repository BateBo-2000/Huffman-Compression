#pragma once
#include "App.h"

#include "UI/ConsoleInterface.h"
#include "Execution/Controller.h"
#include "Execution/Invoker.h"
#include "Archive/Archive.h"
#include "Compression/HuffmanCompressor.h"
#include "Compression/LZWCompressor.h"

App::App()
{
	init();
}

void App::run()
{
	
}

void App::init()
{
	initCmd();
}

void App::initCmd()
{
}
