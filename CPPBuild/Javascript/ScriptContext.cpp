
#include "Precomp.h"
#include "ScriptContext.h"
#include "IOData/File.h"

static const char* cppbuildJS = R"xxxx(

	import { configuration } from "cppbuild/native";
	export { Project, Targets, File, FilePath, Directory, Environment };

	class Target
	{
		constructor(type, name) {
			this.type = type;
			this.name = name;
			this.defines = [];
			this.includePaths = [];
			this.files = [];
			this.filters = [];
			this.linkLibraries = [];
			this.wwwRootDir = "wwwroot";
			this.cssRootFile = name + ".css";
			this.htmlShellFile = "shell.html";
		}

		addFiles(files) {
			var self = this;
			files.forEach(function(file) { self.addFile(file); });
		}

		addFile(file) {
			this.files.push(file);
		}

		addFilters(filters) {
			var self = this;
			filters.forEach(function(filter) { self.addFilter(filter); });
		}

		addFilter(filter) {
			this.filters.push(filter);
		}

		addDefines(defines) {
			var self = this;
			defines.forEach(function(define) { self.addDefine(define); });
		}

		addDefine(define) {
			this.defines.push(define);
		}

		addIncludePaths(paths) {
			var self = this;
			paths.forEach(function(path) { self.addIncludePath(path); });
		}

		addIncludePath(path) {
			this.includePaths.push(path);
		}

		addLinkLibraries(paths) {
			var self = this;
			paths.forEach(function(path) { self.addLinkLibrary(path); });
		}

		addLinkLibrary(lib) {
			this.linkLibraries.push(path);
		}

		setWebRootPath(path) {
			this.wwwrootDir = path;
		}

		setCSSRootFile(file) {
			this.cssRootFile = file;
		}

		setHtmlShellFile(file) {
			this.htmlShellFile = file;
		}

		toTargetDefinition() {
			return {
				type: this.type,
				name: this.name,
				defines: this.defines,
				includePaths: this.includePaths,
				files: this.files,
				filters: this.filters,
				linkLibraries: this.linkLibraries,
				wwwRootDir: this.wwwRootDir,
				cssRootFile: this.cssRootFile,
				htmlShellFile: this.htmlShellFile
			};
		}
	}

	class Configuration
	{
		constructor(name, platform) {
			if (platform === undefined) {
				if (Environment.isX86()) {
					platform = "win32";
				}
				else if (Environment.isARM64()) {
					platform = "arm64";
				}
				else {
					platform = "x64";
				}
			}

			this.name = name;
			this.platform = platform;
		}

		toConfigDefinition() {
			return {
				name: this.name,
				platform: this.platform
			}
		}
	}

	var projectName = "";
	var configList = [];
	var targetList = [];

	class Targets
	{
		static addExecutable(name) {
			var target = new Target("exe", name);
			targetList.push(target);
			return target;
		}

		static addWebsite(name) {
			var target = new Target("website", name);
			targetList.push(target);
			return target;
		}

		static addWebComponent(name) {
			var target = new Target("webcomponent", name);
			targetList.push(target);
			return target;
		}

		static addWebLibrary(name) {
			var target = new Target("weblibrary", name);
			targetList.push(target);
			return target;
		}
	}

	class Project
	{
		static setName(name) {
			projectName = name;
		}

		static addConfiguration(name, platform) {
			var config = new Configuration(name, platform);
			configList.push(config);
			return config;
		}
	}

	class File
	{
		static readAllText(filename) {
		}

		static readAllJson(filename) {
			return JSON.parse(File.readAllText());
		}

		static readAllBytes(filename) {
		}

		static writeAllText(filename, text) {
		}

		static writeAllJson(filename, obj) {
			File.writeAllText(filename, JSON.stringify(obj));
		}

		static writeAllBytes(filename, bytes) {
		}
	}

	class FilePath
	{
		static hasExtension(filename, extension) {
			return false;
		}

		static extension(filename) {
		}

		static removeExtension(filename) {
		}

		static firstComponent(path) {
		}

		static removeFirstComponent(path) {
		}

		static lastComponent(path) {
		}

		static removeLastComponent(path) {
		}

		static combine(path1, path2) {
		}

		static forceSlash(path) {
		}

		static forceBackslash(path) {
		}
	}

	class Directory
	{
		static files(filename) {
			return [];
		}

		static folders(filename) {
			return [];
		}

		static projectPath() {
		}

		static create(path) {
		}
	}

	class Environment
	{
		static getVariable(name) {
		}

		static isMSVC() {
			return true;
		}

		static isClang() {
			return false;
		}

		static isGCC() {
			return false;
		}

		static isWindows() {
			return true;
		}

		static isUnix() {
			return false;
		}

		static isApple() {
			return false;
		}

		static isX86() {
			return false;
		}

		static isX64() {
			return true;
		}

		static isARM32() {
			return false;
		}

		static isARM64() {
			return false;
		}
	}

	configuration.generate = function() {
		return {
			name: projectName,
			targets: targetList.map(function(target) { return target.toTargetDefinition(); }),
			configurations: configList.map(function(config) { return config.toConfigDefinition(); })
		};
	};

)xxxx";

ScriptContext::ScriptContext()
{
	runtime = JS_NewRuntime();
	if (!runtime)
		throw std::runtime_error("JS_NewRuntime failed");

	JS_SetRuntimeOpaque(runtime, this);
	JS_SetModuleLoaderFunc(runtime, nullptr, &ScriptContext::moduleLoader, this);

	context = JS_NewContext(runtime);
	if (!context)
	{
		JS_FreeRuntime(runtime);
		throw std::runtime_error("JS_NewContext failed");
	}

	JS_SetContextOpaque(context, this);
	configuration = ScriptValue(context, JS_NewObject(context));
}

ScriptContext::~ScriptContext()
{
	configuration.reset();
	JS_FreeContext(context);
	JS_FreeRuntime(runtime);
}

std::string ScriptContext::generateConfiguration()
{
	ScriptValue result(context, JS_Call(context, configuration.getPropertyStr("generate").handle(), configuration.handle(), 0, nullptr));
	if (result.isException())
	{
		ScriptValue exception = getException();
		throw std::runtime_error("configuration.generate failed: " + exception.toString());
	}
	return result.toJson();
}

ScriptValue ScriptContext::eval(const std::string& code, const std::string& filename, int flags)
{
	ScriptValue result = ScriptValue(context, JS_Eval(context, code.c_str(), code.size(), filename.c_str(), flags));
	if (result.isPromise())
	{
		JSPromiseStateEnum state = result.promiseState();
		while (state == JS_PROMISE_PENDING)
		{
			JSContext* ctx1 = nullptr;
			int err = JS_ExecutePendingJob(JS_GetRuntime(context), &ctx1);
			if (err < 0)
				throw std::runtime_error("Coult not execute promise");
			state = result.promiseState();
		}
		if (state == JS_PROMISE_REJECTED)
			result = ScriptValue(context, JS_Throw(context, JS_PromiseResult(context, result.handle())));
		else
			result = result.promiseResult();
	}
	return result;
}

bool ScriptContext::hasException()
{
	return JS_HasException(context);
}

ScriptValue ScriptContext::getException()
{
	return ScriptValue(context, JS_GetException(context));
}

static int moduleSetImportMeta(JSContext* ctx, JSValueConst func_val, bool is_main)
{
	JSModuleDef* m = static_cast<JSModuleDef*>(JS_VALUE_GET_PTR(func_val));

	JSAtom module_name_atom = JS_GetModuleName(ctx, m);
	const char* module_name = JS_AtomToCString(ctx, module_name_atom);
    JS_FreeAtom(ctx, module_name_atom);
    if (!module_name)
        return -1;
	std::string buf = module_name;
    JS_FreeCString(ctx, module_name);

	if (buf.find(':') == std::string::npos)
		buf = "file://" + buf;

	JSValue meta_obj = JS_GetImportMeta(ctx, m);
    if (JS_IsException(meta_obj))
        return -1;

    JS_DefinePropertyValueStr(ctx, meta_obj, "url", JS_NewString(ctx, buf.c_str()), JS_PROP_C_W_E);
    JS_DefinePropertyValueStr(ctx, meta_obj, "main", JS_NewBool(ctx, is_main), JS_PROP_C_W_E);
    JS_FreeValue(ctx, meta_obj);
    return 0;
}

int ScriptContext::nativeJSModuleInit(JSContext* ctx, JSModuleDef* moduleDef)
{
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));
	ScriptValue configuration = context->configuration;
	JS_SetModuleExport(ctx, moduleDef, "configuration", configuration.release());
	return 0;
}

JSModuleDef* ScriptContext::moduleLoader(JSContext* ctx, const char* moduleName, void* opaque)
{
	if (strcmp(moduleName, "cppbuild/native") == 0)
	{
		JSModuleDef* moduleDef = JS_NewCModule(ctx, moduleName, &ScriptContext::nativeJSModuleInit);
		JS_AddModuleExport(ctx, moduleDef, "configuration");
		return moduleDef;
	}
	else
	{
		std::string code;
		if (strcmp(moduleName, "cppbuild") == 0)
		{
			code = cppbuildJS;
		}
		else
		{
			try
			{
				code = File::readAllText(moduleName);
			}
			catch (...)
			{
				JS_ThrowReferenceError(ctx, "could not load module '%s'", moduleName);
				return nullptr;
			}
		}

		JSValue result = JS_Eval(ctx, code.c_str(), code.size(), moduleName, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
		if (JS_IsException(result))
			return nullptr;

		if (moduleSetImportMeta(ctx, result, false) < 0)
		{
			JS_FreeValue(ctx, result);
			return nullptr;
		}

		JSModuleDef* moduleDef = static_cast<JSModuleDef*>(JS_VALUE_GET_PTR(result));
		JS_FreeValue(ctx, result);
		return moduleDef;
	}
}
