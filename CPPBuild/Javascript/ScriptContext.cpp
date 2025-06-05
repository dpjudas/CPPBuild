
#include "Precomp.h"
#include "ScriptContext.h"

ScriptContext::ScriptContext()
{
	runtime = JS_NewRuntime();
	if (!runtime)
		throw std::runtime_error("JS_NewRuntime failed");

	context = JS_NewContext(runtime);
	if (!context)
	{
		JS_FreeRuntime(runtime);
		throw std::runtime_error("JS_NewContext failed");
	}
}

ScriptContext::~ScriptContext()
{
	JS_FreeContext(context);
	JS_FreeRuntime(runtime);
}

ScriptValue ScriptContext::eval(const std::string& code, const std::string& filename, int flags)
{
	return ScriptValue(context, JS_Eval(context, code.c_str(), code.size(), filename.c_str(), flags));
}

bool ScriptContext::hasException()
{
	return JS_HasException(context);
}

ScriptValue ScriptContext::getException()
{
	return ScriptValue(context, JS_GetException(context));
}
