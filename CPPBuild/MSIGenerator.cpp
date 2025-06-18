
#include "Precomp.h"
#include "MSIGenerator.h"
#include "MSIDatabase.h"
#include "MSISchema.h"
#include "Text/UTF16.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"
#include "IOData/Directory.h"
#include "Zip/ZipReader.h"
#include <iostream>

#ifdef WIN32

class MSIGeneratorImpl : public MSIGenerator
{
public:
	void generate() override
	{
#if 0
		File::writeAllText("C:\\Development\\CPPBuild\\CPPBuild\\MSISchema.h", MSISchema::generateCode("C:\\Development\\Schema.Msi"));
		return;
#else
		std::string productName = "MNP2000";
		std::string productVersion = "01.40.0000";
		std::string manufacturer = "CPPBuild";
		std::string productCode = "{BDEAD587-1A5D-4547-BE94-DED150E3E088}";
		std::string upgradeCode = "{4D1FB718-DD8F-40E7-BAE5-443DB0E01627}";
		std::string packageCode = "{F50595D3-DEBF-4906-AF02-99DE0659D3F7}";
		std::string productKeywords = "Installer,MNP2000,CPPBuild";
		int productLanguage = 1033;
		int installerVersion = 500; // Windows Installer 5.0; Windows 7 and newer

		HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (FAILED(result))
			throw std::runtime_error("CoInitializeEx(COINIT_APARTMENTTHREADED) failed");

		std::string filename = "C:\\Development\\test.msi";

		std::vector<MSIDirectory> directory =
		{
			{ "TARGETDIR", "", "SourceDir" },
			{ "ProgramFiles64Folder", "TARGETDIR", "." },
			{ "MENUDIR", "INSTALLDIR", "Menu" },
			{ "INSTALLDIR", "ProgramFiles64Folder", "Red_Park:Notepad" }
		};

		const int msidbComponentAttributesLocalOnly = 0;
		// const int msidbComponentAttributesSourceOnly = 1;
		// const int msidbComponentAttributesOptional = 2;
		// const int msidbComponentAttributesRegistryKeyPath = 4;
		const int msidbComponentAttributes64bit = 256;

		std::vector<MSIComponent> components =
		{
			{
				.component = "Notepad",
				.componentId = "{5606A45C-E07F-45C8-9EC9-D1E0224F58C1}",
				.directory = "INSTALLDIR",
				.attributes = msidbComponentAttributesLocalOnly | msidbComponentAttributes64bit,
				.keyPath = "Redpark.exe"
			}
		};

		std::vector<MSIFile> files =
		{
			{
				.file = "Redpark.exe",
				.component = "Notepad",
				.fileName = "Redpark.exe",
				.fileSize = 1337,
				.sequence = 1
			},
			{
				.file = "Redpark.pk3",
				.component = "Notepad",
				.fileName = "Redpark.pk3",
				.fileSize = 1337,
				.sequence = 1
			},
		};

		std::vector<MSIMedia> media =
		{
			{
				.diskId = 1,
				.lastSequence = 1,
				.cabinet = "cabinet.cab"
			}
		};

		std::vector<MSIFeature> features =
		{
			{
				.feature = "Notepad",
				.title = "Notepad",
				.description = "Notepad Editor",
				.display = 1 /* sort order + odd value = expanded node, even value = collapsed */,
				.level = 3 /* install level is used for Custom, Typical, Minimum style selection */,
				.directory = "INSTALLDIR",
				.attributes = 0
			}
		};

		// "Each Windows Installer Feature uses one or more Windows Installer Components, and features may share components"

		std::vector<MSIFeatureComponents> featureComponents =
		{
			{
				.feature = "Notepad",
				.component = "Notepad"
			}
		};

		const int msidbRegistryRootLocalMachine = 2;

		std::vector<MSIRegistry> registry =
		{
			{
				.registry = "MyRegistryKey",
				.root = msidbRegistryRootLocalMachine,
				.key = "SOFTWARE\\CPPBuild\\Notepad Sample",
				.name = "MyRegistryKey",
				.value = "MyRegistryValue" /* #x for REG_BINARY, #% REG_EXPAND_SZ, # for REG_DWORD */,
				.component = "Notepad"
			}
		};

		std::vector<MSIShortcut> shortcuts =
		{
			{
				.shortcut = "sNotepad",
				.directory = "MENUDIR",
				.name = "Redpark.exe",
				.component = "Notepad",
				.target = "[#Redpark.exe]"
			}
		};

		std::vector<MSIProperty> properties =
		{
			{ .property = "ProductCode",     .value = productCode },
			{ .property = "ProductLanguage", .value = std::to_string(productLanguage) },
			{ .property = "Manufacturer",    .value = manufacturer },
			{ .property = "ProductVersion",  .value = productVersion },
			{ .property = "ProductName",     .value = productName },
			{ .property = "UpgradeCode",     .value = upgradeCode }
		};

		std::vector<MSIInstallUISequence> installUISequence =
		{
			{ .action = "FatalError", .sequence = -3 },
			{ .action = "UserExit", .sequence = -2 },
			{ .action = "ExitDialog", .sequence = -1 },
			{ .action = "LaunchConditions", .sequence = 100 },
			{ .action = "PrepareDlg", .sequence = 140 },
			{ .action = "AppSearch", .sequence = 400 },
			{ .action = "CCPSearch", .condition = "NOT Installed", .sequence = 500 },
			{ .action = "RMCCPSearch", .condition = "NOT Installed", .sequence = 600 },
			{ .action = "CostInitialize", .sequence = 800 },
			{ .action = "FileCost", .sequence = 900 },
			{ .action = "CostFinalize", .sequence = 1000 },
			{ .action = "WelcomeDlg", .condition = "NOT Installed", .sequence = 1230 },
			{ .action = "ResumeDlg", .condition = "Installed AND (RESUME OR Preselected)", .sequence = 1240 },
			{ .action = "MaintenanceWelcomeDlg", .condition = "Installed AND NOT RESUME AND NOT Preselected", .sequence = 1250 },
			{ .action = "ProgressDlg", .sequence = 1280 },
			{ .action = "ExecuteAction", .sequence = 1300 }
		};

		std::vector<MSIInstallExecuteSequence> installExecuteSequence =
		{
			{ .action = "LaunchConditions", .sequence = 100 },
			{ .action = "AppSearch", .sequence = 400 },
			{ .action = "CCPSearch", .condition = "NOT Installed", .sequence = 500 },
			{ .action = "RMCCPSearch", .condition = "NOT Installed", .sequence = 600 },
			{ .action = "ValidateProductID", .sequence = 700 },
			{ .action = "CostInitialize", .sequence = 800 },
			{ .action = "FileCost", .sequence = 900 },
			{ .action = "CostFinalize", .sequence = 1000 },
			{ .action = "SetODBCFolders", .sequence = 1100 },
			{ .action = "InstallValidate", .sequence = 1400 },
			{ .action = "InstallInitialize", .sequence = 1500 },
			{ .action = "AllocateRegistrySpace", .condition = "NOT Installed", .sequence = 1550 },
			{ .action = "ProcessComponents", .sequence = 1600 },
			{ .action = "UnpublishComponents", .sequence = 1700 },
			{ .action = "UnpublishFeatures", .sequence = 1800 },
			{ .action = "StopServices", .condition = "VersionNT", .sequence = 1900 },
			{ .action = "DeleteServices", .condition = "VersionNT", .sequence = 2000 },
			{ .action = "UnregisterComPlus", .sequence = 2100 },
			{ .action = "SelfUnregModules", .sequence = 2200 },
			{ .action = "UnregisterTypeLibraries", .sequence = 2300 },
			{ .action = "RemoveODBC", .sequence = 2400 },
			{ .action = "UnregisterFonts", .sequence = 2500 },
			{ .action = "RemoveRegistryValues", .sequence = 2600 },
			{ .action = "UnregisterClassInfo", .sequence = 2700 },
			{ .action = "UnregisterExtensionInfo", .sequence = 2800 },
			{ .action = "UnregisterProgIdInfo", .sequence = 2900 },
			{ .action = "UnregisterMIMEInfo", .sequence = 3000 },
			{ .action = "RemoveIniValues", .sequence = 3100 },
			{ .action = "RemoveShortcuts", .sequence = 3200 },
			{ .action = "RemoveEnvironmentStrings", .sequence = 3300 },
			{ .action = "RemoveDuplicateFiles", .sequence = 3400 },
			{ .action = "RemoveFiles", .sequence = 3500 },
			{ .action = "RemoveFolders", .sequence = 3600 },
			{ .action = "CreateFolders", .sequence = 3700 },
			{ .action = "MoveFiles", .sequence = 3800 },
			{ .action = "InstallFiles", .sequence = 4000 },
			{ .action = "PatchFiles", .sequence = 4090 },
			{ .action = "DuplicateFiles", .sequence = 4210 },
			{ .action = "BindImage", .sequence = 4300 },
			{ .action = "CreateShortcuts", .sequence = 4500 },
			{ .action = "RegisterClassInfo", .sequence = 4600 },
			{ .action = "RegisterExtensionInfo", .sequence = 4700 },
			{ .action = "RegisterProgIdInfo", .sequence = 4800 },
			{ .action = "RegisterMIMEInfo", .sequence = 4900 },
			{ .action = "WriteRegistryValues", .sequence = 5000 },
			{ .action = "WriteIniValues", .sequence = 5100 },
			{ .action = "WriteEnvironmentStrings", .sequence = 5200 },
			{ .action = "RegisterFonts", .sequence = 5300 },
			{ .action = "InstallODBC", .sequence = 5400 },
			{ .action = "RegisterTypeLibraries", .sequence = 5500 },
			{ .action = "SelfRegModules", .sequence = 5600 },
			{ .action = "RegisterComPlus", .sequence = 5700 },
			{ .action = "InstallServices", .condition = "VersionNT", .sequence = 5800 },
			{ .action = "StartServices", .condition = "VersionNT", .sequence = 5900 },
			{ .action = "RegisterUser", .sequence = 6000 },
			{ .action = "RegisterProduct", .sequence = 6100 },
			{ .action = "PublishComponents", .sequence = 6200 },
			{ .action = "PublishFeatures", .sequence = 6300 },
			{ .action = "PublishProduct", .sequence = 6400 },
			{ .action = "InstallFinalize", .sequence = 6600 }
		};

		std::vector<MSIAdminUISequence> adminUISequence =
		{
			{ .action = "FatalError", .sequence = -3 },
			{ .action = "UserExit", .sequence = -2 },
			{ .action = "ExitDialog", .sequence = -1 },
			{ .action = "PrepareDlg", .sequence = 140 },
			{ .action = "CostInitialize", .sequence = 800 },
			{ .action = "FileCost", .sequence = 900 },
			{ .action = "CostFinalize", .sequence = 1000 },
			{ .action = "AdminWelcomeDlg", .sequence = 1230 },
			{ .action = "ProgressDlg", .sequence = 1280 },
			{ .action = "ExecuteAction", .sequence = 1300 }
		};

		std::vector<MSIAdminExecuteSequence> adminExecuteSequence =
		{
			{ .action = "CostInitialize", .sequence = 800 },
			{ .action = "FileCost", .sequence = 900 },
			{ .action = "CostFinalize", .sequence = 1000 },
			{ .action = "InstallValidate", .sequence = 1400 },
			{ .action = "InstallInitialize", .sequence = 1500 },
			{ .action = "InstallAdminPackage", .sequence = 3900 },
			{ .action = "InstallFiles", .sequence = 4000 },
			{ .action = "InstallFinalize", .sequence = 6600 }
		};

		std::vector<MSIAdvtExecuteSequence> advtExecuteSequence =
		{
			{ .action = "CostInitialize", .sequence = 800 },
			{ .action = "CostFinalize", .sequence = 1000 },
			{ .action = "InstallValidate", .sequence = 1400 },
			{ .action = "InstallInitialize", .sequence = 1500 },
			{ .action = "CreateShortcuts", .sequence = 4500 },
			{ .action = "RegisterClassInfo", .sequence = 4600 },
			{ .action = "RegisterExtensionInfo", .sequence = 4700 },
			{ .action = "RegisterProgIdInfo", .sequence = 4800 },
			{ .action = "RegisterMIMEInfo", .sequence = 4900 },
			{ .action = "PublishComponents", .sequence = 6200 },
			{ .action = "PublishFeatures", .sequence = 6300 },
			{ .action = "PublishProduct", .sequence = 6400 },
			{ .action = "InstallFinalize", .sequence = 6600 }
		};

		auto db = MSIDatabase::createAlways(filename);
		db->createTable<MSIValidation>({});
		db->createTable(directory);
		db->createTable(components);
		db->createTable(files);
		db->createTable(media);
		db->createTable(features);
		db->createTable(featureComponents);
		db->createTable(registry);
		db->createTable(shortcuts);
		db->createTable(properties);
		db->createTable(installUISequence);
		db->createTable(installExecuteSequence);
		db->createTable(adminUISequence);
		db->createTable(adminExecuteSequence);
		db->createTable(advtExecuteSequence);

		// Summary Information Stream

		auto summaryInfo = db->getSummaryInfo(12);
		summaryInfo->setCodepage(1252); // The ANSI codepage used to store the strings below
		summaryInfo->setPageCount(installerVersion);
		summaryInfo->setTitle("Installation Database");
		summaryInfo->setSubject(productName);
		summaryInfo->setAuthor(manufacturer);
		summaryInfo->setKeywords(productKeywords);
		summaryInfo->setComments("This installer database contains the logic and data required to install " + productName);
		summaryInfo->setTemplate("x64;1033");
		summaryInfo->setRevisionNumber(packageCode);
		summaryInfo->setCreatingApplication("CPPBuild");
		summaryInfo->setPageCount(installerVersion);
		summaryInfo->setWordCount(2); // Bit flags: 1 = short filenames, 2 = source is compressed, 4 = source is an administrative image created by admin install, 8 = no elevated privileges needed for install
		summaryInfo->setSecurity(2); // "The value of this property should be 2 - Recommended read-only."
		summaryInfo->commit();
		summaryInfo.reset();

		db->commit();
#endif
	}
};

std::shared_ptr<MSIGenerator> MSIGenerator::create()
{
	return std::make_shared<MSIGeneratorImpl>();
}

#else

class MSIGeneratorImpl : public MSIGenerator
{
public:
};

std::shared_ptr<MSIGenerator> MSIGenerator::create()
{
	return std::make_shared<MSIGeneratorImpl>();
}

#endif
