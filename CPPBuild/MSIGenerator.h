#pragma once

class InstallerComponent
{
public:
	std::string name;
	std::string msiComponentId;
	std::vector<std::string> files;
};

class InstallerFeature
{
public:
	std::string name;
	std::vector<std::string> components;
};

class InstallerDefinition
{
public:
	std::string name;
	std::string installDir;
	std::string msiProductName;
	std::string msiProductVersion;
	std::string msiManufacturer;
	std::string msiProductCode;
	std::string msiUpgradeCode;
	std::string msiPackageCode;
	std::vector<std::string> msiProductKeywords;
	std::vector<InstallerComponent> components;
	std::vector<InstallerFeature> features;
};

class MSIGenerator
{
public:
	static std::shared_ptr<MSIGenerator> create();

	virtual ~MSIGenerator() = default;
	virtual void generate(const std::string& binDir, const std::string& sourcePath, const InstallerDefinition& installerDef) = 0;
};
