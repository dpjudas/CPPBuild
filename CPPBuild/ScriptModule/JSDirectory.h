R"xxxx(

class Directory
{
	static files(searchpattern, options) {
		var recursive = isObject(options) && options.recursive === true;
		var includeSearchPath = !isObject(options) || options.includeSearchPath !== false;
		if (includeSearchPath) {
			var basedir = FilePath.removeLastComponent(searchpattern);
			return native.getFiles(Directory.currentPath(searchpattern), recursive).map(function(name) { return FilePath.combine(basedir, name); });
		}
		else {
			return native.getFiles(Directory.currentPath(searchpattern), recursive);
		}
	}

	static folders(searchpattern, options) {
		var recursive = isObject(options) && options.recursive === true;
		var includeSearchPath = !isObject(options) || options.includeSearchPath !== false;
		if (includeSearchPath) {
			var basedir = FilePath.removeLastComponent(searchpattern);
			return native.getFolders(Directory.currentPath(searchpattern), recursive).map(function(name) { return FilePath.combine(basedir, name); });
		}
		else {
			return native.getFolders(Directory.currentPath(searchpattern), recursive);
		}
	}

	static currentPath(relpath) {
		if (relpath === undefined) {
			return FilePath.combine(native.sourcePath, native.subdirectory);
		}
		else {
			return FilePath.combine(FilePath.combine(native.sourcePath, native.subdirectory), relpath);
		}
	}

	static buildPath(relpath) {
		if (relpath === undefined) {
			return native.buildPath;
		}
		else {
			return FilePath.combine(native.buildPath, relpath);
		}
	}

	static create(path) {
		return native.createDirectory(path);
	}
}

)xxxx",