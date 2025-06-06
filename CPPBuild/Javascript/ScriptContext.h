#pragma once

#include "ScriptValue.h"

class ScriptContext
{
public:
	ScriptContext();
	~ScriptContext();

	ScriptValue eval(const std::string& code, const std::string& filename, int flags);

	std::string generateConfiguration();

	bool hasException();
	ScriptValue getException();

private:
	static JSModuleDef* moduleLoader(JSContext* ctx, const char* moduleName, void* opaque);
	static int nativeJSModuleInit(JSContext* ctx, JSModuleDef* moduleDef);

	JSRuntime* runtime = nullptr;
	JSContext* context = nullptr;
	ScriptValue configuration;

	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
};
