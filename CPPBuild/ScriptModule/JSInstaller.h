R"xxxx(

class Installer
{
	static add(name) {
		var installer = new Installer(native.subdirectory, name);
		installerList.push(installer);
		return installer;
	}

	constructor(subdirectory, name) {
		this.subdirectory = subdirectory;
		this.name = name;
		this.installDir = name;
		this.components = [];
		this.features = [];
		this.msiProductName = name;
		this.msiProductVersion = "01.01.0000";
		this.msiManufacturer = "";
		this.msiProductCode = "";
		this.msiUpgradeCode = "";
		this.msiPackageCode = "";
		this.msiProductKeywords = [ "Installer" ];
	}

	setInstallDir(path) {
		this.installDir = path;
	}

	setMsiProductName(name) {
		this.msiProductName = name;
	}

	setMsiProductVersion(version) {
		this.msiProductVersion = version;
	}

	setMsiManufacturer(manufacturer) {
		this.msiManufacturer = manufacturer;
	}

	setMsiProductCode(productCode) {
		this.msiProductCode = productCode;
	}

	setMsiUpgradeCode(upgradeCode) {
		this.msiUpgradeCode = upgradeCode;
	}

	setMsiPackageCode(packageCode) {
		this.msiPackageCode = packageCode;
	}

	addMsiProductKeywords(keywords) {
		var self = this;
		keywords.forEach(function(keyword) { self.addMsiProductKeyword(keyword); });
	}

	addMsiProductKeyword(keyword) {
		this.msiProductKeywords.push(keyword);
	}

	addComponent(name) {
		var component = new InstallerComponent(name);
		this.components.push(component);
		return component;
	}

	addFeature(name) {
		var feature = new InstallerFeature(name);
		this.features.push(feature);
		return feature;
	}

	toInstallerDefinition() {
		return {
			subdirectory: this.subdirectory,
			name: this.name,
			installDir: this.installDir,
			components: this.components.map(function(component) { return component.toInstallerDefinition(); }),
			features: this.features.map(function(feature) { return feature.toInstallerDefinition(); }),
			msiProductName: this.msiProductName,
			msiProductVersion: this.msiProductVersion,
			msiManufacturer: this.msiManufacturer,
			msiProductCode: this.msiProductCode,
			msiUpgradeCode: this.msiUpgradeCode,
			msiPackageCode: this.msiPackageCode,
			msiProductKeywords: this.msiProductKeywords
		};
	}
}

class InstallerComponent
{
	constructor(name) {
		this.name = name;
		this.msiComponentId = "";
		this.files = [];
	}

	setMsiComponentId(id) {
		this.msiComponentId = id;
	}

	addFiles(files) {
		var self = this;
		files.forEach(function(file) { self.addFile(file); });
	}

	addFile(file) {
		this.files.push(file);
	}

	toInstallerDefinition() {
		return {
			name: this.name,
			msiComponentId: this.msiComponentId,
			files: this.files
		};
	}
}

class InstallerFeature
{
	constructor(name) {
		this.name = name;
		this.components = [];
	}

	addComponents(components) {
		var self = this;
		components.forEach(function(component) { self.addComponent(component); });
	}

	addComponent(component) {
		this.components.push(component);
	}

	toInstallerDefinition() {
		return {
			name: this.name,
			components: this.components.map(function(component) { return component.name; })
		};
	}
}

)xxxx",