R"xxxx(

class PackageInstaller
{
	static add(name) {
		var installer = new PackageInstaller(native.subdirectory, name);
		packageInstallerList.push(installer);
		return installer;
	}

	constructor(subdirectory, name) {
		this.subdirectory = subdirectory;
		this.name = name;
		this.files = [];
		this.copyFiles = [];
		this.defines = [];
		this.cCompileOptions = [];
		this.cxxCompileOptions = [];
		this.linkOptions = [];
		this.includePaths = [];
		this.linkLibraries = [];
		this.libraryPaths = [];
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
				libraryPaths: []
			};
		}
		return this.configurations[name];
	}

	addFiles(files) {
		var self = this;
		files.forEach(function(file) {
			self.files.push(file);
		});
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
		var self = this;
		defines.forEach(function(define) { self.addDefine(define, options); });
	}

	addDefine(define, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.defines.push(define);
		}
		else {
			this.defines.push(define);
		}
	}

	addCompileOptions(opts, options) {
		var self = this;
		opts.forEach(function(opt) { self.addCompileOption(opt, options); });
	}

	addCompileOption(opt, options) {
		if (isObject(options)) {
			var obj = this;
			if (options.configuration !== undefined) {
				obj = this.getConfiguration(options.configuration);
			}
			if (options.lang === undefined || options.lang == "c") {
				obj.cCompileOptions.push(opt);
			}
			if (options.lang === undefined || options.lang == "c++") {
				obj.cxxCompileOptions.push(opt);
			}
		}
		else {
			this.cCompileOptions.push(opt);
			this.cxxCompileOptions.push(opt);
		}
	}

	addLinkOptions(opts, options) {
		var self = this;
		opts.forEach(function(opt) { self.addLinkOption(opt, options); });
	}

	addLinkOption(opt, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.linkOptions.push(opt);
		}
		else {
			this.linkOptions.push(opt);
		}
	}

	addIncludePaths(paths, options) {
		var self = this;
		paths.forEach(function(path) { self.addIncludePath(path, options); });
	}

	addIncludePath(path, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.includePaths.push(path);
		}
		else {
			this.includePaths.push(path);
		}
	}

	addLinkLibraries(libs, options) {
		var self = this;
		libs.forEach(function(lib) { self.addLinkLibrary(lib, options); });
	}

	addLinkLibrary(lib, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.linkLibraries.push(lib);
		}
		else {
			this.linkLibraries.push(lib);
		}
	}

	addLibraryPaths(paths, options) {
		var self = this;
		paths.forEach(function(path) { self.addLibraryPath(path, options); });
	}

	addLibraryPath(path, options) {
		if (isObject(options) && options.configuration !== undefined) {
			var config = this.getConfiguration(options.configuration);
			config.libraryPaths.push(path);
		}
		else {
			this.libraryPaths.push(path);
		}
	}

	toInstallerDefinition() {
		return {
			subdirectory: this.subdirectory,
			name: this.name,
			files: this.files,
			copyFiles: this.copyFiles,
			defines: this.defines,
			cCompileOptions: this.cCompileOptions,
			cxxCompileOptions: this.cxxCompileOptions,
			linkOptions: this.linkOptions,
			includePaths: this.includePaths,
			linkLibraries: this.linkLibraries,
			libraryPaths: this.libraryPaths,
			configurations: this.configurations
		};
	}
}

)xxxx",