#pragma once

#include "Package.h"

class HttpUri;
class BuildSetup;
class BuildPackage;

class PackageManager
{
public:
	PackageManager(const std::string& workDir);

	void createPackage(const BuildSetup& setup);

	void update(const BuildSetup& setup);

	const Package& getPackage(const std::string& name);
	std::string getPackagePath(const std::string& name);

private:
	void updatePackage(const BuildSetup& setup, const BuildPackage& pkgdesc, std::map<std::string, std::string>& etags);

	bool download(const HttpUri& url, const std::string& filename, std::string& etag);
	std::string getPackageCacheFilename();
	std::map<std::string, std::string> loadPackageCache();
	void savePackageCache(const std::map<std::string, std::string>& cache);

	std::string workDir;
	std::string packagesDir;
	std::map<std::string, Package> packages;
};
