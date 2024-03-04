#pragma once

#include "Serialization.h"
#include "vmath/vmath.h"

inline std::pair<bool, double> JSONToFloat(const JSON& obj)
{
	if (obj.JSONType() == JSON::Class::Floating)
	{
		return { true, obj.ToFloat() };
	}
	if (obj.JSONType() == JSON::Class::Integral)
	{
		return { true, obj.ToInt() };
	}
	return { false, 0 };
}
inline bool TryParseJSON_NumericArray(const JSON& obj, float* arrayOut, unsigned int expectedNumElements)
{
	if (obj.JSONType() != JSON::Class::Array)
	{
		return false;
	}
	if (obj.length() < (int)expectedNumElements)
	{
		return false;
	}
	for (unsigned int i = 0; i < expectedNumElements; i++)
	{
		auto isNumeric__asFloat = JSONToFloat(obj.at(i));
		if (!isNumeric__asFloat.first)
		{
			return false;
		}
		arrayOut[i] = isNumeric__asFloat.second;
	}
	return true;
}

template<class CastTo>
struct IntegerAdapter_template : public JSONAdapter<CastTo, JSON::Class::Integral>
{
	using JSONAdapter<CastTo, JSON::Class::Integral>::JSONAdapter;
	// Serialize.
	JSON ToJSON()
	{
		return (int)this->source;
	}
	// Deserialize.
	bool FromJSON(const JSON& obj)
	{
		if (!this->IsCorrectJSONType(obj))
		{
			return false;
		}
		this->source = (CastTo)obj.ToInt();
		return true;
	}
};
template<class CastTo>
inline IntegerAdapter_template<CastTo> IntegerAdapter(CastTo& source)
{
	return { source };
}
template<class CastTo> struct NumericAdapter_template
// I put `Floating` here but can actually be an Integer too.
	: public JSONAdapter<CastTo, JSON::Class::Floating>
{
	using JSONAdapter<CastTo, JSON::Class::Floating>::JSONAdapter;
	bool FromJSON(const JSON& obj)
	{
		auto isNumeric__asFloat = JSONToFloat(obj);
		if (isNumeric__asFloat.first)
		{
			this->source = (CastTo)isNumeric__asFloat.second;
			return true;
		}
		return false;
	}
	JSON ToJSON()
	{
		return this->source;
	}
};
template<class CastTo> NumericAdapter_template<CastTo> NumericAdapter(CastTo& source)
{
	return NumericAdapter_template<CastTo>(source);
}

struct Vector2fAdapter : public JSONAdapter<Vector2f, JSON::Class::Array>
{
	enum { k_NumElements = 2 };
	using BaseAdapter::BaseAdapter;
	// Deserialize.
	bool FromJSON(const JSON& obj)
	{
		return TryParseJSON_NumericArray(obj, (float*)&source, k_NumElements);
	}
	JSON ToJSON()
	{
		return json::Array(source.x, source.y);
	}
};
struct Vector3fAdapter : public JSONAdapter<Vector3f, JSON::Class::Array>
{
	enum { k_NumElements = 3 };
	using BaseAdapter::BaseAdapter;
	// Deserialize.
	bool FromJSON(const JSON& obj)
	{
		return TryParseJSON_NumericArray(obj, (float*)&source, k_NumElements);
	}
	JSON ToJSON()
	{
		return json::Array(source.x, source.y, source.z);
	}
};
struct Vector4fAdapter : public JSONAdapter<Vector4f, JSON::Class::Array>
{
	enum { k_NumElements = 4 };
	using BaseAdapter::BaseAdapter;
	// Deserialize.
	bool FromJSON(const JSON& obj)
	{
		return TryParseJSON_NumericArray(obj, (float*)&source, k_NumElements);
	}
	JSON ToJSON()
	{
		return json::Array(
			source.x,
			source.y,
			source.z,
			source.w
			);
	}
};

template<class T> auto NumericVectorAdapter(T& source);

template<> inline auto NumericVectorAdapter(Vector2f& source) { return Vector2fAdapter(source); }
template<> inline auto NumericVectorAdapter(Vector3f& source) { return Vector3fAdapter(source); }
template<> inline auto NumericVectorAdapter(Vector4f& source) { return Vector4fAdapter(source); }
