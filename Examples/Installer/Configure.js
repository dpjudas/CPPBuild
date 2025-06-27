import { Project, Installers, File, FilePath, Directory, Environment } from "cppbuild";

Project.setName("Installer");

var files = [
	"vkdoom.exe",
	"vkdoom.pdb",
	"vkdoom.pk3",
	"vktool.exe",
	"vktool.pdb",
	"zmusic.dll",
	"libsndfile-1.dll",
	"lights.pk3",
	"brightmaps.pk3",
	"game_support.pk3",
	"game_widescreen_gfx.pk3",
	"licenses.zip",
//	"fm_banks/GENMIDI.GS.wopl",
//	"fm_banks/gs-by-papiezak-and-sneakernets.wopn",
//	"soundfonts/vkdoom.sf2"
];

var installer = Installers.addInstaller("vkdoom");
installer.setInstallDir("VKDoom");
installer.setMsiProductName("VKDoom");
installer.setMsiProductVersion("1.0.0.0");
installer.setMsiManufacturer("vkdoom.org");
installer.setMsiProductCode("{380F8AE3-EF1E-466A-B498-E23F8C83A1D4}");
installer.setMsiUpgradeCode("{A125BF4D-FCC2-412B-9515-7C4CB63809A7}");
installer.setMsiPackageCode("{E70F1358-2B61-4488-AC2F-C56CA6FA2584}");
installer.addMsiProductKeyword("vkdoom");

var component = installer.addComponent("vkdoom");
component.setMsiComponentId("{5355EB98-07EE-4C78-950A-2262CDF3758E}");
component.addFiles(files);

var feature = installer.addFeature("vkdoom");
feature.addComponent(component);
