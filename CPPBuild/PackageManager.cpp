
#include "Precomp.h"
#include "PackageManager.h"
#include "Package.h"
#include "HttpClient/HttpClient.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"
#include "IOData/Directory.h"
#include "IOData/MemoryDevice.h"
#include "Zip/ZipReader.h"
#include "Zip/ZipWriter.h"
#include "BuildSetup.h"
#include <unordered_set>
#include <iostream>

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
	Directory::create(packagesDir);

	for (const BuildPackage& pkgdesc : setup.project.packages)
	{
		// To do: only update packages if they changed

		std::string packageZip;

		if ((pkgdesc.source.size() >= 5 && pkgdesc.source.substr(0, 5) == "http:") ||
			(pkgdesc.source.size() >= 6 && pkgdesc.source.substr(0, 6) == "https:"))
		{
			HttpUri source = pkgdesc.source;
			packageZip = FilePath::combine(packagesDir, "package.zip");
			download(source, packageZip);
		}
		else if (!pkgdesc.source.empty())
		{
			packageZip = FilePath::combine(setup.sourcePath, { pkgdesc.subdirectory, pkgdesc.source });
		}
		else
		{
			throw std::runtime_error("Empty package url");
		}

		std::unique_ptr<ZipReader> zip = ZipReader::open(File::openExisting(packageZip));

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

void PackageManager::createPackage(const BuildSetup& setup)
{
	Directory::create(packagesDir);

	for (const BuildPackageInstaller& def : setup.project.packageInstallers)
	{
		std::string sourcePath = FilePath::combine(setup.sourcePath, def.subdirectory);
		std::string packageFilename = FilePath::combine(packagesDir, def.name) + ".zip";

		auto memdevice = MemoryDevice::create();
		auto zip = ZipWriter::create(memdevice);

		Package package;
		package.name = def.name;
		package.defines = def.defines;
		for (const auto& src : def.copyFiles)
			package.copyFiles.push_back(PackageCopyFile{ src.src, src.dest });
		package.cCompileOptions = def.cCompileOptions;
		package.cxxCompileOptions = def.cxxCompileOptions;
		package.linkOptions = def.linkOptions;
		package.includePaths = def.includePaths;
		package.linkLibraries = def.linkLibraries;
		package.libraryPaths = def.libraryPaths;
		for (const auto& configdef : def.configurations)
		{
			PackageConfiguration config;
			config.defines = configdef.second.defines;
			config.cCompileOptions = configdef.second.cCompileOptions;
			config.cxxCompileOptions = configdef.second.cxxCompileOptions;
			config.linkOptions = configdef.second.linkOptions;
			config.includePaths = configdef.second.includePaths;
			config.linkLibraries = configdef.second.linkLibraries;
			config.libraryPaths = configdef.second.libraryPaths;
			for (const auto& src : configdef.second.copyFiles)
				config.copyFiles.push_back(PackageCopyFile{ src.src, src.dest });
			package.configurations[configdef.first] = std::move(config);
		}

		std::string packageJson = package.toJson().to_json();
		zip->addFile("package.json", true, packageJson.data(), packageJson.size());

		for (const auto& file : def.files)
		{
			std::shared_ptr<DataBuffer> data = File::readAllBytes(FilePath::combine(sourcePath, file.src));
			zip->addFile(file.dest, true, data->data(), data->size());
		}

		zip->writeToc();
		zip.reset();
		File::writeAllBytes(FilePath::combine(packagesDir, packageFilename), memdevice->buffer());
	}
}
