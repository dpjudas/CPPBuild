#pragma once

#include "ScriptValue.h"

class ScriptContext
{
public:
	ScriptContext();
	~ScriptContext();

	ScriptValue eval(const std::string& code, const std::string& filename, int flags);

	bool hasException();
	ScriptValue getException();

private:
	JSRuntime* runtime = nullptr;
	JSContext* context = nullptr;

	ScriptContext(const ScriptContext&) = delete;
	ScriptContext& operator=(const ScriptContext&) = delete;
};
