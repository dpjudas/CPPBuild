
#include "Precomp.h"
#include "PackageManager.h"
#include "HttpClient/HttpClient.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"
#include "IOData/Directory.h"
#include "Zip/ZipReader.h"
#include "BuildSetup.h"
#include <unordered_set>

void PackageManager::update(const BuildSetup& setup)
{
	// To do: make packages path configurable
	std::string packagesDir = "C:\\Development\\Packages";
	Directory::create(packagesDir);

	for (const BuildPackage& package : setup.project.packages)
	{
		if (package.sources.empty())
			throw std::runtime_error("Package " + package.name + " has no source URL");

		HttpUri source = package.sources.front();
		if (source.scheme.empty())
			continue;

		// To do: only update packages if they changed

		std::string packageZip = FilePath::combine(packagesDir, package.name + ".zip");
		download(source, packageZip);

		auto zip = ZipReader::open(File::openExisting(packageZip));

		std::string packageDir = FilePath::combine(packagesDir, package.name);
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
