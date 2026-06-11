R"xxxx(

native.generate = function() {
	return {
		name: projectName,
		targets: targetList.map(function(target) { return target.toTargetDefinition(); }),
		configurations: configList.map(function(config) { return config.toConfigDefinition(); }),
		packages: packageList.map(function(pkg) { return pkg.toPackageDefinition(); }),
		installers: installerList.map(function(installer) { return installer.toInstallerDefinition(); }),
		packageInstallers: packageInstallerList.map(function(installer) { return installer.toInstallerDefinition(); }),
		fileFolders: solutionFolderList
	};
};

)xxxx",