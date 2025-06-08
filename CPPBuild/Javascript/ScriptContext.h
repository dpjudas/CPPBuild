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

	JSRuntime* runtime = nullptr;
	JSContext* context = nullptr;

	std::string sourcePath;
	ScriptValue native;

	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
};
