R"xxxx(

class FilePath
{
	static hasExtension(filename, ext) {
		return FilePath.extension(filename).toLowerCase() == ext;
	}

	static extension(filename) {
		var file = FilePath.lastComponent(filename);
		var pos = file.lastIndexOf(".");
		if (pos == -1) {
			return "";
		}
		else {
			return file.substring(pos + 1);
		}
	
	}

	static removeExtension(filename) {
		var file = FilePath.lastComponent(filename);
		var pos = file.lastIndexOf(".");
		if (pos == -1) {
			return filename;
		}
		else {
			return filename.substring(0, filename.length - file.length + pos);
		}
	}

	static firstComponent(path) {
		var firstSlash = path.indexOf("/");
		var firstBackslash = path.indexOf("\\");
		if (firstBackslash != -1 && (firstSlash == -1 || firstSlash > firstBackslash)) {
			firstSlash = firstBackslash;
		}

		if (firstSlash != -1) {
			return path.substring(0, firstSlash);
		}
		else {
			return path;
		}
	}

	static removeFirstComponent(path) {
		var firstSlash = path.indexOf("/");
		var firstBackslash = path.indexOf("\\");
		if (firstBackslash != -1 && (firstSlash == -1 || firstSlash > firstBackslash)) {
			firstSlash = firstBackslash;
		}

		if (firstSlash != -1) {
			return path.substring(firstSlash + 1);
		}
		else {
			return "";
		}
	}

	static lastComponent(path) {
		var lastSlash = path.lastIndexOf("/");
		var lastBackslash = path.lastIndexOf("\\");
		if (lastSlash != -1 && lastSlash >= lastBackslash) {
			return path.substring(lastSlash + 1);
		}
		else if (lastBackslash != -1 && lastBackslash >= lastSlash) {
			return path.substring(lastBackslash + 1);
		}
		else {
			return path;
		}
	}

	static removeLastComponent(path) {
		var lastSlash = path.lastIndexOf("/");
		var lastBackslash = path.lastIndexOf("\\");
		if (lastSlash != -1 && lastSlash >= lastBackslash) {
			return path.substring(0, lastSlash);
		}
		else if (lastBackslash != -1 && lastBackslash >= lastSlash) {
			return path.substring(0, lastBackslash);
		}
		else {
			return "";
		}
	}

	static combine(path1, path2) {
		if (Array.isArray(path1)) {
			return FilePath.combine(...path1);
		}
		else if (arguments.length == 1) {
			return path1;
		}
		else if (arguments.length > 2) {
			var result = path1;
			for (var i = 1; i < arguments.length; i++) {
				result = FilePath.combine(result, arguments[i]);
			}
			return result;
		}
		else {
			if (path1 == "") {
				return FilePath.forceSlash(path1);
			}
			else if (path2 == "") {
				return FilePath.forceSlash(path1);
			}
			else if (path2.startsWith("\\") || path2.startsWith("/")) {
				return FilePath.forceSlash(path2);
			}
			else if (path2.length >= 2 && path2[1] == ':') {
				return FilePath.forceSlash(path2);
			}
			else if (!path2.endsWith("\\") && !path2.endsWith("/")) {
				return FilePath.forceSlash(path1 + "/" + path2);
			}
			else {
				return FilePath.forceSlash(path1 + path2);
			}
		}
	}

	static forceSlash(path) {
		return path.replaceAll("\\", "/");
	}

	static forceBackslash(path) {
		return path.replaceAll("/", "\\");
	}
}

)xxxx",