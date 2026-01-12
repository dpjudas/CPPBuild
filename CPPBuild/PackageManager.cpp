
#include "Precomp.h"
#include "PackageManager.h"
#include "Package.h"
#include "HttpClient/HttpClient.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"
#include "IOData/Directory.h"
#include "Zip/ZipReader.h"
#include "BuildSetup.h"
#include <unordered_set>

PackageManager::PackageManager(const std::string& workDir) : workDir(workDir)
{
	std::string buildDir = FilePath::combine(workDir, "Build");
	packagesDir = FilePath::combine(buildDir, "Packages");
}

std::string PackageManager::getPackagePath(const std::string& name)
{
	return FilePath::combine(packagesDir, name);
}

const Package& PackageManager::getPackage(const std::string& name)
{
	auto it = packages.find(name);
	if (it != packages.end())
		return it->second;

	std::string packageDir = FilePath::combine(packagesDir, name);
	try
	{
		Package pkg = Package::fromJson(JsonValue::parse(File::readAllText(FilePath::combine(packageDir, "package.json"))));
		Package& result = packages[name];
		result = pkg;
		return result;
	}
	catch (...)
	{
		throw std::runtime_error("Package '" + name + "' not found");
	}
}

void PackageManager::update(const BuildSetup& setup)
{
	// To do: should we have a global packages dir too?
	Directory::create(packagesDir);

	for (const BuildPackage& pkgdesc : setup.project.packages)
	{
		HttpUri source = pkgdesc.source;
		if (source.scheme.empty())
			continue;

		// To do: only update packages if they changed

		std::string packageZip = FilePath::combine(packagesDir, "package.zip");
		download(source, packageZip);

		auto zip = ZipReader::open(File::openExisting(packageZip));

		Package pkg = Package::fromJson(JsonValue::parse(zip->readAllText("package.json")));

		std::string packageDir = FilePath::combine(packagesDir, pkg.name);
		Directory::create(packageDir);
		std::unordered_set<std::string> createdDirs;

		for (const ZipFileEntry& entry : zip->getFiles())
		{
			std::string path = FilePath::removeLastComponent(entry.filename);
			if (createdDirs.insert(path).second)
				Directory::create(FilePath::combine(packageDir, path));
			File::writeAllBytes(FilePath::combine(packageDir, entry.filename), zip->readAllBytes(entry.filename));
		}
	}
}

void PackageManager::download(const HttpUri& url, const std::string& filename)
{
	HttpRequest request("GET", url);
	HttpResponse response = HttpClient::send(request);
	if (response.statusCode == 200)
	{
		File::writeAllBytes(filename, response.data.data(), response.data.size());
	}
	else
	{
		throw std::runtime_error("Could not download " + url.toString() + ": " + response.statusText);
	}
}
