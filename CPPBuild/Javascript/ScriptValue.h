#pragma once

#include "quickjs.h"
#include "IOData/DataBuffer.h"

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
		reset();
	}

	ScriptValue& operator=(const ScriptValue& other)
	{
		if (this != &other)
		{
			reset();
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

	bool isException() const
	{
		checkContext();
		return JS_IsException(value);
	}

	std::string getExceptionStack()
	{
		checkContext();
		if (!JS_IsError(value))
			return {};

		return getPropertyStr("stack").toString();
	}

	ScriptValue getPropertyStr(JSAtom name)
	{
		checkContext();
		return ScriptValue(context, JS_GetProperty(context, value, name));
	}

	ScriptValue getPropertyStr(const std::string& name)
	{
		checkContext();
		return ScriptValue(context, JS_GetPropertyStr(context, value, name.c_str()));
	}

	ScriptValue getPropertyUint32(uint32_t index)
	{
		checkContext();
		return ScriptValue(context, JS_GetPropertyInt64(context, value, index));
	}

	ScriptValue getPropertyInt64(int64_t index)
	{
		checkContext();
		return ScriptValue(context, JS_GetPropertyInt64(context, value, index));
	}

	void setPropertyStr(JSAtom name, ScriptValue newvalue)
	{
		checkContext();
		JS_SetProperty(context, handle(), name, newvalue.release());
	}

	void setPropertyStr(const std::string& name, ScriptValue newvalue)
	{
		checkContext();
		JS_SetPropertyStr(context, handle(), name.c_str(), newvalue.release());
	}

	bool isPromise() const
	{
		checkContext();
		return JS_IsPromise(value);
	}

	JSPromiseStateEnum promiseState() const
	{
		checkContext();
		return JS_PromiseState(context, value);
	}

	ScriptValue promiseResult()
	{
		checkContext();
		return ScriptValue(context, JS_PromiseResult(context, value));
	}

	int32_t toInt32()
	{
		int32_t result = 0;
		int status = JS_ToInt32(context, &result, value);
		if (status < 0)
			throw std::runtime_error("Could not get int32 script value");
		return result;
	}

	int64_t toInt64()
	{
		int64_t result = 0;
		int status = JS_ToInt64(context, &result, value);
		if (status < 0)
			throw std::runtime_error("Could not get int64 script value");
		return result;
	}

	uint64_t toIndex()
	{
		uint64_t result = 0;
		int status = JS_ToIndex(context, &result, value);
		if (status < 0)
			throw std::runtime_error("Could not get index script value");
		return result;
	}

	std::string toString()
	{
		checkContext();
		size_t strLength = 0;
		const char* strData = JS_ToCStringLen(context, &strLength, value);
		std::string str(strData, strLength);
		if (strData)
			JS_FreeCString(context, strData);
		return str;
	}

	std::string toJson()
	{
		checkContext();
		ScriptValue json(context, JS_JSONStringify(context, value, JS_UNDEFINED, JS_UNDEFINED));
		return json.toString();
	}

	void reset()
	{
		if (context)
		{
			JS_FreeValue(context, value);
			context = nullptr;
		}
	}

	JSValue release()
	{
		checkContext();
		context = nullptr;
		return value;
	}

	JSValue handle() const { return value; }

private:
	void checkContext() const
	{
		if (!context)
			throw std::runtime_error("ScriptValue not assigned");
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
		checkContext();
		size_t strLength = 0;
		const char* strData = JS_ToCStringLen(context, &strLength, value);
		std::string str(strData, strLength);
		if (strData)
			JS_FreeCString(context, strData);
		return str;
	}

	bool toBool() const
	{
		int val = JS_ToBool(context, value);
		if (val == -1)
			throw std::runtime_error("ScriptValueConst is an exception");
		return val == 1;
	}

	std::shared_ptr<DataBuffer> toBuffer() const
	{
		size_t size = 0;
		uint8_t* data = JS_GetArrayBuffer(context, &size, value);
		return DataBuffer::create(data, size);
	}

	bool isException() const
	{
		checkContext();
		return JS_IsException(value);
	}

	std::string toJson()
	{
		checkContext();
		ScriptValue json(context, JS_JSONStringify(context, value, JS_UNDEFINED, JS_UNDEFINED));
		return json.toString();
	}

private:
	void checkContext() const
	{
		if (!context)
			throw std::runtime_error("ScriptValueConst not assigned");
	}

	JSContext* context = nullptr;
	JSValue value = {};
};
