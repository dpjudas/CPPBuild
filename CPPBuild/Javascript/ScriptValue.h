#pragma once

#include "quickjs.h"

class ScriptValue
{
public:
	ScriptValue() = default;
	ScriptValue(JSContext* context, JSValue value) : context(context), value(value) {}

	ScriptValue(const ScriptValue& other)
	{
		if (other.context)
			value = JS_DupValue(other.context, other.value);
		context = other.context;
	}

	ScriptValue(ScriptValue&& other) noexcept : context(other.context), value(other.value)
	{
		other.context = nullptr;
	}

	~ScriptValue()
	{
		freeValue();
	}

	ScriptValue& operator=(const ScriptValue& other)
	{
		if (this != &other)
		{
			freeValue();
			if (other.context)
				value = JS_DupValue(other.context, other.value);
			context = other.context;
		}
		return *this;
	}

	ScriptValue& operator=(ScriptValue&& other) noexcept
	{
		std::swap(value, other.value);
		std::swap(context, other.context);
		return *this;
	}

	std::string toString()
	{
		if (!context)
			throw std::runtime_error("ScriptValue not assigned");
		size_t strLength = 0;
		const char* strData = JS_ToCStringLen(context, &strLength, value);
		std::string str(strData, strLength);
		if (strData)
			JS_FreeCString(context, strData);
		return str;
	}

	bool isException() const
	{
		if (!context)
			throw std::runtime_error("ScriptValue not assigned");
		return JS_IsException(value);
	}

	std::string toJson()
	{
		if (!context)
			throw std::runtime_error("ScriptValue not assigned");
		ScriptValue json(context, JS_JSONStringify(context, value, JS_UNDEFINED, JS_UNDEFINED));
		return json.toString();
	}

private:
	void freeValue()
	{
		if (context)
		{
			JS_FreeValue(context, value);
			context = nullptr;
		}
	}

	JSContext* context = nullptr;
	JSValue value = {};
};

class ScriptValueConst
{
public:
	ScriptValueConst() = default;
	ScriptValueConst(JSContext* context, JSValue value) : context(context), value(value) {}

	std::string toString() const
	{
		if (!context)
			throw std::runtime_error("ScriptValue not assigned");
		size_t strLength = 0;
		const char* strData = JS_ToCStringLen(context, &strLength, value);
		std::string str(strData, strLength);
		if (strData)
			JS_FreeCString(context, strData);
		return str;
	}

	bool isException() const
	{
		if (!context)
			throw std::runtime_error("ScriptValue not assigned");
		return JS_IsException(value);
	}

	std::string toJson()
	{
		if (!context)
			throw std::runtime_error("ScriptValue not assigned");
		ScriptValue json(context, JS_JSONStringify(context, value, JS_UNDEFINED, JS_UNDEFINED));
		return json.toString();
	}

private:
	JSContext* context = nullptr;
	JSValue value = {};
};
