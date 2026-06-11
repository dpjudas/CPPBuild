R"xxxx(

class Target
{
	static addApplication(name) {
		var target = new Target(native.subdirectory, "application", name);
		targetList.push(target);
		return target;
	}

	static addConsole(name) {
		var target = new Target(native.subdirectory, "console", name);
		targetList.push(target);
		return target;
	}

	static addStaticLibrary(name) {
		var target = new Target(native.subdirectory, "lib", name);
		targetList.push(target);
		return target;
	}

	static addDynamicLibrary(name) {
		var target = new Target(native.subdirectory, "dll", name);
		targetList.push(target);
		return target;
	}

	static addCustom(name) {
		var target = new Target(native.subdirectory, "custom", name);
		targetList.push(target);
		return target;
	}

	static addWebsite(name) {
		var target = new Target(native.subdirectory, "website", name);
		targetList.push(target);
		return target;
	}

	static addWebComponent(name) {
		var target = new Target(native.subdirectory, "webcomponent", name);
		targetList.push(target);
		return target;
	}

	static addWebLibrary(name) {
		var target = new Target(native.subdirectory, "weblibrary", name);
		targetList.push(target);
		return target;
	}

	constructor(subdirectory, type, name) {
		this.subdirectory = subdirectory;
		this.type = type;
		this.name = name;
		this.group = "";
		this.copyFiles = [];
		this.defines = [];
		this.cCompileOptions = [];
		this.cxxCompileOptions = [];
		this.linkOptions = [];
		this.includePaths = [];
		this.files = [];
		this.filePropertySets = [];
		this.customCommands = [];
		this.filters = [];
		this.linkLibraries = [];
		this.libraryPaths = [];
		this.packages = [];
		this.wwwRootDir = "wwwroot";
		this.cssRootFile = name + ".css";
		this.htmlShellFile = "shell.html";
		this.buildCommand = "";
		this.cleanCommand = "";
		this.configurations = {};
	}

	getConfiguration(name) {
		if (this.configurations[name] === undefined) {
			this.configurations[name] = {
				copyFiles: [],
				defines: [],
				cCompileOptions: [],
				cxxCompileOptions: [],
				linkOptions: [],
				includePaths: [],
				linkLibraries: [],
				libraryPaths: [],
				packages: [],
				buildCommand: "",
				cleanCommand: ""
			};
		}
		return this.configurations[name];
	}

	setGroup(group) {
		this.group = group;
	}

	addFiles(files) {
		var self = this;
		files.forEach(function(file) { self.files.push(file); });
	}

	addFilters(filters) {
		var self = this;
		filters.forEach(function(filter) { self.filters.push(filter); });
	}

	addCustomCommand(desc) {
		var cmd = {
			inputFile: desc.inputFile,
			commands: [].concat(desc.commands),
			outputFiles: [].concat(desc.outputFiles),
			configName: ""
		};
		if (desc.configuration !== undefined) {
			cmd.configName = desc.configuration;
		}
		this.customCommands.push(cmd);
	}

	addCopyFiles(files, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			files.forEach(function(file) {
				config.copyFiles.push(file);
			});
		}
		else {
			var self = this;
			files.forEach(function(file) {
				self.copyFiles.push(file);
			});
		}
	}

	addDefines(defines, options) {
		if (isObject(options) && options.files !== undefined) {
			var propSet = {
				files: [].concat(options.files),
				configName: options.configuration,
				defines: [].concat(defines),
				compileOptions: [],
				includePaths: []
			};
			this.filePropertySets.push(propSet);
		}
		else if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			defines.forEach(function(define) { config.defines.push(define); });
		}
		else {
			var self = this;
			defines.forEach(function(define) { self.defines.push(define); });
		}
	}

	addCompileOptions(opts, options) {
		if (isObject(options) && options.files !== undefined) {
			var propSet = {
				files: [].concat(options.files),
				configName: options.configuration,
				defines: [],
				compileOptions: [].concat(opts),
				includePaths: []
			};
			this.filePropertySets.push(propSet);
		}
		else if (isObject(options)) {
			var obj = this;
			if (options.configuration !== undefined) {
				obj = this.getConfiguration(options.configuration);
			}
			if (options.lang === undefined || options.lang == "c") {
				opts.forEach(function(opt) { obj.cCompileOptions.push(opt); });
			}
			if (options.lang === undefined || options.lang == "c++") {
				opts.forEach(function(opt) { obj.cxxCompileOptions.push(opt); });
			}
		}
		else {
			var self = this;
			opts.forEach(function(opt) {
				self.cCompileOptions.push(opt);
				self.cxxCompileOptions.push(opt);
			});
		}
	}

	addIncludePaths(paths, options) {
		if (isObject(options) && options.files !== undefined) {
			var propSet = {
				files: [].concat(options.files),
				configName: options.configuration,
				defines: [],
				compileOptions: [],
				includePaths: [].concat(paths)
			};
			this.filePropertySets.push(propSet);
		}
		else if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			paths.forEach(function(path) { config.includePaths.push(path); });
		}
		else {
			var self = this;
			paths.forEach(function(path) { self.includePaths.push(path); });
		}
	}

	addLinkOptions(opts, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			opts.forEach(function(opt) { config.linkOptions.push(opt); });
		}
		else {
			var self = this;
			opts.forEach(function(opt) { self.linkOptions.push(opt); });
		}
	}

	addLinkLibraries(libs, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			libs.forEach(function(lib) { config.linkLibraries.push(lib); });
		}
		else {
			var self = this;
			libs.forEach(function(lib) { self.linkLibraries.push(lib); });
		}
	}

	addLibraryPaths(paths, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			paths.forEach(function(path) { config.libraryPaths.push(path); });
		}
		else {
			var self = this;
			paths.forEach(function(path) { self.libraryPaths.push(path); });
		}

	}

	addPackage(name, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.packages.push(name);
		}
		else {
			this.packages.push(name);
		}
	}

	addPackages(names, options) {
		var self = this;
		names.forEach(function(name) { self.addPackage(name, options); });
	}

	setWebRootPath(path) {
		this.wwwRootDir = path;
	}

	setCSSRootFile(file) {
		this.cssRootFile = file;
	}

	setHtmlShellFile(file) {
		this.htmlShellFile = file;
	}

	setBuildCommand(command, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.buildCommand = command;
		}
		else {
			this.buildCommand = command;
		}
	}

	setCleanCommand(command, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.cleanCommand = command;
		}
		else {
			this.cleanCommand = command;
		}
	}

	toTargetDefinition() {
		return {
			subdirectory: this.subdirectory,
			type: this.type,
			name: this.name,
			group: this.group,
			copyFiles: this.copyFiles,
			defines: this.defines,
			cCompileOptions: this.cCompileOptions,
			cxxCompileOptions: this.cxxCompileOptions,
			linkOptions: this.linkOptions,
			includePaths: this.includePaths,
			files: this.files,
			filePropertySets: this.filePropertySets,
			customCommands: this.customCommands,
			filters: this.filters,
			linkLibraries: this.linkLibraries,
			libraryPaths: this.libraryPaths,
			packages: this.packages,
			wwwRootDir: this.wwwRootDir,
			cssRootFile: this.cssRootFile,
			htmlShellFile: this.htmlShellFile,
			configurations: this.configurations
		};
	}
}

)xxxx",