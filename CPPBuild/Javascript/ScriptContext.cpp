
#include "Precomp.h"
#include "ScriptContext.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"
#include "IOData/Directory.h"
#include "CPPBuildJS.h"

ScriptContext::ScriptContext(const std::string& sourcePath, const std::string& buildPath) : sourcePath(sourcePath)
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
	native.setPropertyStr("getEnvironmentVar", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::getEnvironmentVar, "getEnvironmentVar", 1)));
	native.setPropertyStr("sourcePath", ScriptValue(context, JS_NewString(context, sourcePath.c_str())));
	native.setPropertyStr("buildPath", ScriptValue(context, JS_NewString(context, buildPath.c_str())));

	// To do: we should detect/ask this at runtime, since the compiler building CPPBuild may not be what is to be used.

#if defined(_MSC_VER)
	native.setPropertyStr("environmentMsvc", ScriptValue(context, JS_NewBool(context, true)));
	native.setPropertyStr("environmentClang", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentGCC", ScriptValue(context, JS_NewBool(context, false)));
#elif defined(__clang__)
	native.setPropertyStr("environmentMsvc", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentClang", ScriptValue(context, JS_NewBool(context, true)));
	native.setPropertyStr("environmentGCC", ScriptValue(context, JS_NewBool(context, false)));
#elif defined(__GNUC__)
	native.setPropertyStr("environmentMsvc", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentClang", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentGCC", ScriptValue(context, JS_NewBool(context, true)));
#else
	#error "Unknown compiler"
#endif

#if defined(WIN32)
	native.setPropertyStr("environmentWindows", ScriptValue(context, JS_NewBool(context, true)));
	native.setPropertyStr("environmentUnix", ScriptValue(context, JS_NewBool(context, false)));
#else
	native.setPropertyStr("environmentWindows", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentUnix", ScriptValue(context, JS_NewBool(context, true)));
#endif

#if defined(__APPLE__)
	native.setPropertyStr("environmentApple", ScriptValue(context, JS_NewBool(context, true)));
#else
	native.setPropertyStr("environmentApple", ScriptValue(context, JS_NewBool(context, false)));
#endif

#if defined(_M_X64) || defined(__x86_64__)
	native.setPropertyStr("environmentX86", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentX64", ScriptValue(context, JS_NewBool(context, true)));
	native.setPropertyStr("environmentARM32", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentARM64", ScriptValue(context, JS_NewBool(context, false)));
#elif defined(_M_ARM64) || defined(__aarch64__)
	native.setPropertyStr("environmentX86", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentX64", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentARM32", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentARM64", ScriptValue(context, JS_NewBool(context, true)));
#elif defined(_M_IX86) || defined(__i386__)
	native.setPropertyStr("environmentX86", ScriptValue(context, JS_NewBool(context, true)));
	native.setPropertyStr("environmentX64", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentARM32", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentARM64", ScriptValue(context, JS_NewBool(context, false)));
#elif defined(_M_ARM) || defined(__arm__)
	native.setPropertyStr("environmentX86", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentX64", ScriptValue(context, JS_NewBool(context, false)));
	native.setPropertyStr("environmentARM32", ScriptValue(context, JS_NewBool(context, true)));
	native.setPropertyStr("environmentARM64", ScriptValue(context, JS_NewBool(context, false)));
#else
	#error "Unknown platform"
#endif

	native.setPropertyStr("createDirectory", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::createDirectory, "createDirectory", 1)));
	native.setPropertyStr("getFiles", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::getFiles, "getFiles", 2)));
	native.setPropertyStr("getFolders", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::getFolders, "getFolders", 2)));
	native.setPropertyStr("readAllText", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::readAllText, "readAllText", 1)));
	native.setPropertyStr("readAllBytes", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::readAllBytes, "readAllBytes", 1)));
	native.setPropertyStr("writeAllText", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::writeAllText, "writeAllText", 2)));
	native.setPropertyStr("writeAllBytes", ScriptValue(context, JS_NewCFunction(context, &ScriptContext::writeAllBytes, "writeAllBytes", 2)));
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

		std::string scriptName = FilePath::removeExtension(FilePath::lastComponent(newpath)) + ".js";

		std::string scriptFilename = FilePath::combine(context->sourcePath, { newpath, scriptName });
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

JSValue ScriptContext::getEnvironmentVar(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 1 || !JS_IsString(argv[0]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to Environment.getVariable"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		#ifdef _MSC_VER
		#pragma warning(push)
		#pragma warning(disable: 4996) // C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s instead
		#endif
		std::string value = std::getenv(arg0.toString().c_str());
		#ifdef _MSC_VER
		#pragma warning(pop)
		#endif
		return JS_NewString(ctx, value.c_str());
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
}

JSValue ScriptContext::createDirectory(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 1 || !JS_IsString(argv[0]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to Directory.create"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		Directory::create(arg0.toString().c_str());
		return JS_UNDEFINED;
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
}

static void scanFiles(JSContext* ctx, std::vector<JSValue>& result, const std::string& searchDir, const std::string& searchPattern, const std::string& basePath, bool recursive)
{
	std::string s = FilePath::combine(searchDir, searchPattern);
	for (const std::string& filename : Directory::files(s))
	{
		result.push_back(JS_NewString(ctx, FilePath::combine(basePath, filename).c_str()));
	}
	if (recursive)
	{
		for (const std::string& foldername : Directory::folders(FilePath::combine(searchDir, "*")))
		{
			scanFiles(ctx, result, FilePath::combine(searchDir, foldername), searchPattern, FilePath::combine(basePath, foldername), true);
		}
	}
}

JSValue ScriptContext::getFiles(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 2 || !JS_IsString(argv[0]) || !JS_IsBool(argv[1]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to Directory.files"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptValueConst arg1(ctx, argv[1]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		std::string searchPattern = arg0.toString();
		bool recursive = arg1.toBool();

		std::vector<JSValue> files;
		scanFiles(ctx, files, FilePath::removeLastComponent(searchPattern), FilePath::lastComponent(searchPattern), std::string(), recursive);
		return JS_NewArrayFrom(ctx, (int)files.size(), files.data());
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
}

static void scanFolders(JSContext* ctx, std::vector<JSValue>& result, const std::string& searchDir, const std::string& searchPattern, const std::string& basePath, bool recursive)
{
	for (const std::string& foldername : Directory::folders(FilePath::combine(searchDir, searchPattern)))
	{
		std::string s = FilePath::combine(basePath, foldername);
		result.push_back(JS_NewString(ctx, s.c_str()));
		if (recursive)
			scanFolders(ctx, result, FilePath::combine(searchDir, foldername), searchPattern, s, true);
	}
	if (recursive)
	{
		for (const std::string& foldername : Directory::folders(FilePath::combine(searchDir, "*")))
		{
			scanFolders(ctx, result, FilePath::combine(searchDir, foldername), searchPattern, FilePath::combine(basePath, foldername), true);
		}
	}
}

JSValue ScriptContext::getFolders(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 2 || !JS_IsString(argv[0]) || !JS_IsBool(argv[1]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to Directory.files"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptValueConst arg1(ctx, argv[1]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		std::string searchPattern = arg0.toString();
		bool recursive = arg1.toBool();

		std::vector<JSValue> folders;
		scanFolders(ctx, folders, FilePath::removeLastComponent(searchPattern), FilePath::lastComponent(searchPattern), std::string(), recursive);
		return JS_NewArrayFrom(ctx, (int)folders.size(), folders.data());
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
}

JSValue ScriptContext::readAllText(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 1 || !JS_IsString(argv[0]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to File.readAllText"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		std::string text = File::readAllText(arg0.toString());
		return JS_NewString(ctx, text.c_str());
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
}

JSValue ScriptContext::readAllBytes(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 1 || !JS_IsString(argv[0]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to File.readAllBytes"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		auto data = File::readAllBytes(arg0.toString());
		return JS_NewArrayBufferCopy(ctx, data->data<uint8_t>(), data->size());
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
}

JSValue ScriptContext::writeAllText(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 2 || !JS_IsString(argv[0]) || !JS_IsString(argv[1]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to File.writeAllText"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptValueConst arg1(ctx, argv[1]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		File::writeAllText(arg0.toString(), arg1.toString());
		return JS_UNDEFINED;
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
}

JSValue ScriptContext::writeAllBytes(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
	if (argc < 2 || !JS_IsString(argv[0]) || !JS_IsArrayBuffer(argv[1]))
		return JS_Throw(ctx, JS_NewString(ctx, "Invalid arguments passed to File.writeAllBytes"));

	ScriptValueConst arg0(ctx, argv[0]);
	ScriptValueConst arg1(ctx, argv[1]);
	ScriptContext* context = static_cast<ScriptContext*>(JS_GetContextOpaque(ctx));

	try
	{
		File::writeAllBytes(arg0.toString(), arg1.toBuffer());
		return JS_UNDEFINED;
	}
	catch (const std::exception& e)
	{
		return JS_Throw(ctx, JS_NewString(ctx, e.what()));
	}
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
			code += cppbuildJS2;
		}
		else
		{
			// Is this the correct way to get the path? moduleSetImportMeta is wrong? If only there was some documentation...
			ScriptContext* context = static_cast<ScriptContext*>(opaque);
			std::string current = context->native.getPropertyStr("subdirectory").toString();
			std::string moduleFilename = FilePath::combine(context->sourcePath, { current, moduleName });
			try
			{
				code = File::readAllText(moduleFilename);
			}
			catch (...)
			{
				JS_ThrowReferenceError(ctx, "could not load module '%s'", moduleFilename.c_str());
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
