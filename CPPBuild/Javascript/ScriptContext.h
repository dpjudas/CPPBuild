#pragma once

#include "ScriptValue.h"

class ScriptContext
{
public:
	ScriptContext(const std::string& sourcePath);
	~ScriptContext();

	ScriptValue eval(const std::string& code, const std::string& filename, int flags);

	std::string generateConfiguration();

	bool hasException();
	ScriptValue getException();

private:
	static JSModuleDef* moduleLoader(JSContext* ctx, const char* moduleName, void* opaque);
	static int nativeJSModuleInit(JSContext* ctx, JSModuleDef* moduleDef);
	static JSValue addSubdirectory(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue getEnvironmentVar(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue createDirectory(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue getFiles(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue getFolders(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue readAllText(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue readAllBytes(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue writeAllText(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);
	static JSValue writeAllBytes(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv);

	JSRuntime* runtime = nullptr;
	JSContext* context = nullptr;

	std::string sourcePath;
	ScriptValue native;

	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
};
