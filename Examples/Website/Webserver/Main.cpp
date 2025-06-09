
#include "Precomp.h"
#include "Main.h"
#include "Webserver/ActionModule.h"
#include "Webserver/FileModule.h"
#include "Webserver/Webserver.h"
#include "IOData/Directory.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"
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
