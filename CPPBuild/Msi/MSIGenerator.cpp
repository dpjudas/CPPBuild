
#include "Precomp.h"
#include "MSIGenerator.h"
#include "MSIDatabase.h"
#include "MSISchema.h"
#include "MSISequence.h"
#include "Msi/Dialogs/MSIDialog.h"
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
	void generate(const std::string& binDir, const std::string& sourcePath, const BuildInstaller& installerDef) override
	{
#if 0
		MSISchema::saveBinaries("C:\\Development\\UISample.Msi", "C:\\Development\\UISampleBinaries");
#elif 0
		File::writeAllText("C:\\Development\\UISample.txt", MSISchema::exportTables("C:\\Development\\UISample.Msi"));
#elif 0
		File::writeAllText("C:\\Development\\CPPBuild\\CPPBuild\\MSISchema.h", MSISchema::generateCode("C:\\Development\\Schema.Msi"));
#else
		std::string productName = installerDef.msiProductName;
		std::string productVersion = installerDef.msiProductVersion;
		std::string manufacturer = installerDef.msiManufacturer;
		std::string productCode = installerDef.msiProductCode;
		std::string upgradeCode = installerDef.msiUpgradeCode;
		std::string packageCode = installerDef.msiPackageCode;
		std::string productKeywords;
		for (const auto& keyword : installerDef.msiProductKeywords)
		{
			if (!productKeywords.empty())
				productKeywords.push_back(',');
			productKeywords += keyword;
		}
		int productLanguage = 1033;
		int installerVersion = 500; // Windows Installer 5.0; Windows 7 and newer

		HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (FAILED(result))
			throw std::runtime_error("CoInitializeEx(COINIT_APARTMENTTHREADED) failed");

		std::string filename = FilePath::combine(binDir, installerDef.name + ".msi");

		std::vector<MSIDirectory> directory =
		{
			{ "TARGETDIR", "", "SourceDir" },
			{ "ProgramFiles64Folder", "TARGETDIR", "." },
			{ "INSTALLDIR", "ProgramFiles64Folder", installerDef.installDir },
			{ "MENUDIR", "INSTALLDIR", "Menu" },
		};

		const int msidbComponentAttributesLocalOnly = 0;
		// const int msidbComponentAttributesSourceOnly = 1;
		// const int msidbComponentAttributesOptional = 2;
		// const int msidbComponentAttributesRegistryKeyPath = 4;
		const int msidbComponentAttributes64bit = 256;

		std::vector<MSIComponent> components;
		std::vector<std::string> cabFiles;
		std::vector<MSIFile> files;
		int sequence = 1;
		for (const BuildInstallerComponent& component : installerDef.components)
		{
			components.push_back({
				.component = component.name,
				.componentId = component.msiComponentId,
				.directory = "INSTALLDIR",
				.attributes = msidbComponentAttributesLocalOnly | msidbComponentAttributes64bit,
				.keyPath = "file_1"
				});

			for (const std::string& filename : component.files)
			{
				size_t filesize = File::openExisting(FilePath::combine(sourcePath, filename))->size();
				if (filesize > 0x7fffffff)
					throw std::runtime_error("File too large");

				std::string shortname = "aa" + std::to_string(sequence) + "." + FilePath::extension(filename);
				std::string identifier = "file_" + std::to_string(sequence);

				MSIFile file =
				{
					.file = identifier,
					.component = component.name,
					.fileName = shortname + "|" + filename,
					.fileSize = (int)filesize,
					.sequence = sequence
				};

				files.push_back(std::move(file));
				cabFiles.push_back(filename);
				sequence++;
			}
		}

		std::vector<MSIMedia> media =
		{
			{
				.diskId = 1,
				.lastSequence = (int)cabFiles.size(),
				.cabinet = "#cab1.cab"
			}
		};

		std::vector<MSIFeature> features;
		for (const BuildInstallerFeature& feature : installerDef.features)
		{
			features.push_back({
				.feature = feature.name,
				.title = feature.name,
				.description = feature.name,
				.display = 1 /* sort order + odd value = expanded node, even value = collapsed */,
				.level = 3 /* install level is used for Custom, Typical, Minimum style selection */,
				.directory = "INSTALLDIR",
				.attributes = 0
				});
		}

		// "Each Windows Installer Feature uses one or more Windows Installer Components, and features may share components"

		std::vector<MSIFeatureComponents> featureComponents;
		for (const BuildInstallerFeature& feature : installerDef.features)
		{
			for (const std::string& component : feature.components)
			{
				featureComponents.push_back({ .feature = feature.name, .component = component });
			}
		}

		const int msidbRegistryRootLocalMachine = 2;

		std::vector<MSIRegistry> registry;
		std::vector<MSIShortcut> shortcuts;
#if 0
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
				.name = "vkdoom.exe",
				.component = "Notepad",
				.target = "[#file_1]"
			}
		};
#endif

		std::vector<MSIProperty> properties =
		{
			{ .property = "ProductCode", .value = productCode },
			{ .property = "ProductLanguage", .value = std::to_string(productLanguage) },
			{ .property = "Manufacturer", .value = manufacturer },
			{ .property = "ProductVersion", .value = productVersion },
			{ .property = "ProductName", .value = productName },
			{ .property = "UpgradeCode", .value = upgradeCode },
		};

		MSISequences sequences;
		MSIDialogs dialogs;

		auto db = MSIDatabase::createAlways(filename);
		db->createTable<MSIValidation>({});
		dialogs.createTables(db.get(), properties);
		sequences.createTables(db.get());
		db->createTable(directory);
		db->createTable(components);
		db->createTable(files);
		db->createTable(media);
		db->createTable(features);
		db->createTable(featureComponents);
		db->createTable(registry);
		db->createTable(shortcuts);
		db->createTable(properties);

		// Cabinet.cab data stream

		auto cabinetWriter = std::make_unique<CabinetWriter>();
		sequence = 1;
		for (const std::string& filename : cabFiles)
		{
			std::string identifier = "file_" + std::to_string(sequence);
			cabinetWriter->addFile(identifier, FilePath::combine(sourcePath, filename));
			sequence++;
		}
		auto cabfile = cabinetWriter->close();
		cabinetWriter.reset();

		auto view = db->createView("INSERT INTO `_Streams` (`Name`, `Data`) VALUES(?, ?)", 2);
		view->setString(0, "cab1.cab");
		view->setStream(1, cabfile);
		view->execute();
		view.reset();

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
