R"xxxx(

class Environment
{
	static getVariable(name) {
		return native.getEnvironmentVar(name);
	}

	static isMSVC() {
		return native.environmentMsvc;
	}

	static isClang() {
		return native.environmentClang;
	}

	static isGCC() {
		return native.environmentGCC;
	}

	static isWindows() {
		return native.environmentWindows;
	}

	static isUnix() {
		return native.environmentUnix;
	}

	static isApple() {
		return native.environmentApple;
	}

	static isX86() {
		return native.environmentX86;
	}

	static isX64() {
		return native.environmentX64;
	}

	static isARM32() {
		return native.environmentARM32;
	}

	static isARM64() {
		return native.environmentARM64;
	}
}

)xxxx",