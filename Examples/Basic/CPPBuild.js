
import { Targets } from "cppbuild";

var files = [
	"Basic.cpp",
	"Basic.h"
];

var filters = [
];

var basic = Targets.addExecutable("Basic");
basic.addFiles(files);
basic.addFilters(filters);
