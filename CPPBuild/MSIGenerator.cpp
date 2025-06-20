
#include "Precomp.h"
#include "MSIGenerator.h"
#include "MSIDatabase.h"
#include "MSISchema.h"
#include "MSIDialog.h"
#include "MSISequence.h"
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
		MSISchema::saveBinaries("C:\\Development\\UISample.Msi", "C:\\Development\\UISampleBinaries");
#elif 0
		File::writeAllText("C:\\Development\\UISample.txt", MSISchema::exportTables("C:\\Development\\UISample.Msi"));
#elif 0
		File::writeAllText("C:\\Development\\CPPBuild\\CPPBuild\\MSISchema.h", MSISchema::generateCode("C:\\Development\\Schema.Msi"));
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

		std::string srcFolder = "C:\\Development\\VkDoom\\build\\RelWithDebInfo";
		std::vector<std::string> programFiles =
		{
			"vkdoom.exe",
			//"vkdoom.pdb",
			"vkdoom.pk3",
			"vktool.exe",
			//"vktool.pdb",
			"zmusic.dll",
			"openal32.dll",
			"lights.pk3",
			//"licenses.zip",
			"libsndfile-1.dll",
			"game_widescreen_gfx.pk3",
			"game_support.pk3",
			"brightmaps.pk3"
		};

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
				.keyPath = "file_1"
			}
		};

		std::vector<MSIFile> files;
		int sequence = 1;
		for (const std::string& filename : programFiles)
		{
			size_t filesize = File::openExisting(FilePath::combine(srcFolder, filename))->size();
			if (filesize > 0x7fffffff)
				throw std::runtime_error("File too large");

			std::string shortname = "aa" + std::to_string(sequence) + "." + FilePath::extension(filename);
			std::string identifier = "file_" + std::to_string(sequence);

			MSIFile file =
			{
				.file = identifier,
				.component = "Notepad",
				.fileName = shortname + "|" + filename,
				.fileSize = (int)filesize,
				.sequence = sequence
			};
			files.push_back(std::move(file));

			sequence++;
		}

		std::vector<MSIMedia> media =
		{
			{
				.diskId = 1,
				.lastSequence = (int)programFiles.size(),
				.cabinet = "#cab1.cab"
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
				.name = "vkdoom.exe",
				.component = "Notepad",
				.target = "[#file_1]"
			}
		};

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
		for (const std::string& filename : programFiles)
		{
			std::string identifier = "file_" + std::to_string(sequence);
			cabinetWriter->addFile(identifier, FilePath::combine(srcFolder, filename));
			sequence++;
		}
		auto cabfile = cabinetWriter->close();
		cabinetWriter.reset();

		// File::writeAllBytes("c:\\development\\test.cab", cabfile);

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
