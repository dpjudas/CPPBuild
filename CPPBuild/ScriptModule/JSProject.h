R"xxxx(

class Project
{
	static setName(name) {
		projectName = name;
	}

	static addConfiguration(name) {
		var config = new Configuration(name);
		configList.push(config);
		return config;
	}

	static addPackage(source) {
		var pkg = new Package(native.subdirectory, source);
		packageList.push(pkg);
		return pkg;
	}

	static addSubdirectory(path) {
		native.addSubdirectory(path);
	}

	static addSolutionFolder(name, files) {
		solutionFolderList.push({ name: name, files: files });
	}

	static getProperty(name) {
		return native.properties[name];
	}
}

)xxxx",