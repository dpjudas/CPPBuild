R"xxxx(

import { native } from "cppbuild/native";
export { Project, Target, PackageInstaller, Installer, File, FilePath, Directory, Environment };

function isObject(x) {
	return typeof x === 'object' && !Array.isArray(x) && x !== null;
}

var projectName = "";
var configList = [];
var targetList = [];
var packageList = [];
var installerList = [];
var packageInstallerList = [];
var solutionFolderList = [];

)xxxx",