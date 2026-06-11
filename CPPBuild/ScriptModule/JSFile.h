R"xxxx(

class File
{
	static readAllText(filename) {
		return native.readAllText(Directory.currentPath(filename));
	}

	static readAllJson(filename) {
		return JSON.parse(File.readAllText(filename));
	}

	static readAllBytes(filename) {
		return native.readAllBytes(Directory.currentPath(filename));
	}

	static readAllBytesAsHex(filename) {
		return native.readAllBytesAsHex(Directory.currentPath(filename));
	}

	static writeAllText(filename, text) {
		native.writeAllText(Directory.currentPath(filename), text);
	}

	static writeAllJson(filename, obj) {
		File.writeAllText(filename, JSON.stringify(obj));
	}

	static writeAllBytes(filename, bytes) {
		native.writeAllBytes(Directory.currentPath(filename), bytes);
	}
}

)xxxx",
