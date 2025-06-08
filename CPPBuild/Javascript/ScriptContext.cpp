
#include "Precomp.h"
#include "ScriptContext.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"
#include "CPPBuildJS.h"

ScriptContext::ScriptContext(const std::string& sourcePath) : sourcePath(sourcePath)
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
	native = ScriptValue(context, JS_NewObject(context));
	native.setPropertyStr("addSubdirectory", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::addSubdirectory, "addSubdirectory", 1)));
	native.setPropertyStr("subdirectory", ScriptValue(context, JS_NewString(context, "")));
}

ScriptContext::~ScriptContext()
{
	native.reset();
	JS_FreeContext(context);
	JS_FreeRuntime(runtime);
}

std::string ScriptContext::generateConfiguration()
{
	ScriptValue result(context, JS_Call(context, native.getPropertyStr("generate").handle(), native.handle(), 0, nullptr));
	if (result.isException())
	{
		ScriptValue exception = getException();
		throw std::runtime_error("configuration.generate failed: " + exception.toString());
	}
	return result.toJson();
}

JSValue ScriptContext::addSubdirectory(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 1 || !JS_IsString(argv[0]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to Project.addSubdirectory"));

	ScriptValueConst arg0(ctx, argv[0]);

	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));
	std::string current = context->native.getPropertyStr("subdirectory").toString();

	try
	{
		std::string newpath = FilePath::combine(current, arg0.toString());
		context->native.setPropertyStr("subdirectory", ScriptValue(ctx, JS_NewString(ctx, newpath.c_str())));

		std::string scriptFilename = FilePath::combine(context->sourcePath, { newpath, "CPPBuild.js" });
		std::string configureScript = File::readAllText(scriptFilename);

		ScriptValue result = context->eval(configureScript, scriptFilename, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT);

		context->native.setPropertyStr("subdirectory", ScriptValue(ctx, JS_NewString(ctx, current.c_str())));

		if (result.isException())
			return result.release();
	}
	catch (const std::exception& e)
	{
		context->native.setPropertyStr("subdirectory", ScriptValue(ctx, JS_NewString(ctx, current.c_str())));
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}

	return JS_UNDEFINED;
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
	ScriptValue native = context->native;
	JS_SetModuleExport(ctx, moduleDef, "native", native.release());
	return 0;
}

JSModuleDef* ScriptContext::moduleLoader(JSContext* ctx, const char* moduleName, void* opaque)
{
	if (strcmp(moduleName, "cppbuild/native") == 0)
	{
		JSModuleDef* moduleDef = JS_NewCModule(ctx, moduleName, &ScriptContext::nativeJSModuleInit);
		JS_AddModuleExport(ctx, moduleDef, "native");
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
