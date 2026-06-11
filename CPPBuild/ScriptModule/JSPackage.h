R"xxxx(

class Package
{
	constructor(subdirectory, source) {
		this.subdirectory = subdirectory;
		this.source = source;
	}

	toPackageDefinition() {
		return {
			subdirectory: this.subdirectory,
			source: this.source,
		};
	}
}

)xxxx",