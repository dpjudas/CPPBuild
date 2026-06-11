R"xxxx(

class Configuration
{
	constructor(name) {
		this.name = name;
	}

	toConfigDefinition() {
		return {
			name: this.name,
		}
	}
}

)xxxx",