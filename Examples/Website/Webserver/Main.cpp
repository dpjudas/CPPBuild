
#include "Precomp.h"
#include "Main.h"
#include "Common/Webserver/ActionModule.h"
#include "Common/Webserver/FileModule.h"
#include "Common/Webserver/Webserver.h"
#include "Common/IOData/Directory.h"
#include "Common/IOData/File.h"
#include "Common/IOData/FilePath.h"
#include <iostream>

int main()
{
	auto logger = ConsoleLogger::create();

	auto webserver = Webserver::create(WebserverType::tcpSocket);
	webserver->addModule(FileModule::createZip("/", FilePath::combine(Directory::exePath(), "Website.webpkg"), "Website.html"));
	webserver->start();

	char c;
	std::cin >> c;

	return 0;
}
