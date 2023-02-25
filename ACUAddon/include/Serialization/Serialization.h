#pragma once

#include <SimpleJSON/json.hpp>
#include <filesystem>
namespace fs = std::filesystem;

namespace json
{
	JSON LoadFromFile(const fs::path& pathShort);
	inline JSON& GetByKey(JSON& jsonObj, const std::string& key) { return jsonObj.at(key); }
	inline JSON* GetByKeyAndType(JSON& obj, const std::string& key, JSON::Class requiredJsonCls)
	{
		if (!obj.hasKey(key))
		{
			return nullptr;
		}
		JSON& foundValue = obj.at(key);
		if (foundValue.JSONType() != requiredJsonCls)
		{
			return nullptr;
		}
		return &foundValue;
	}
}
#define PARSE_JSON_VARIABLE(declObject, variable, callableAdapter) callableAdapter(variable).FromJSON(json::GetByKey(declObject, #variable))
#define ASSURE_PARSE_JSON_VARIABLE(jsonObject, variable, callableAdapter) if (!PARSE_JSON_VARIABLE(jsonObject, variable, callableAdapter)) return false;

using json::JSON;
template<class AdaptedCls, JSON::Class jsonType>
struct JSONAdapter
{
	using BaseAdapter = JSONAdapter<AdaptedCls, jsonType>;
	AdaptedCls& source;
	JSONAdapter(AdaptedCls& source) : source(source) {}
	static inline json::JSON::Class GetAdapterType()
	{
		return jsonType;
	}
	bool IsCorrectJSONType(const JSON& obj)
	{
		return obj.JSONType() == jsonType;
	}
};

struct StringAdapter : public JSONAdapter<std::string, JSON::Class::String>
{
	using BaseAdapter::BaseAdapter;
	// Serialize.
	JSON ToJSON()
	{
		return source;
	}
	// Deserialize.
	bool FromJSON(const JSON& obj)
	{
		if (!this->IsCorrectJSONType(obj))
		{
			return false;
		}
		source = obj.ToString();
		return true;
	}
};
