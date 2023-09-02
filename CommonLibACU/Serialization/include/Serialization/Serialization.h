#pragma once

#include <SimpleJSON/json.hpp>
#include <filesystem>
namespace fs = std::filesystem;

namespace json
{
    template<typename Adapter> bool TryToReadVariableFromJSONObjectUsingAdapter(JSON& jsonObject, const std::string& key, Adapter&& adaptedVariable)
    {
        if (JSON* foundKey = jsonObject.FindByKey(key))
        {
            return adaptedVariable.FromJSON(*foundKey);
        }
        return false;
    }
    template<typename Adapter> void WriteVariableAsJSONObjectMemberUsingAdapter(JSON& jsonObject, const std::string& key, Adapter&& adaptedVariable)
    {
        jsonObject[key] = adaptedVariable.ToJSON();
    }
    // Doesn't modify the `jsonObject`.
    #define READ_JSON_VARIABLE(jsonObject, variable, callableAdapter) TryToReadVariableFromJSONObjectUsingAdapter(jsonObject, #variable, callableAdapter(variable))
    // Forces the `jsonObject` to Object Type.
    #define WRITE_JSON_VARIABLE(jsonObject, variable, callableAdapter) WriteVariableAsJSONObjectMemberUsingAdapter(jsonObject, #variable, callableAdapter(variable))
}

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
/*
Using JSON::ToStringNoEscape() I added because ToString() uses json_escape() which duplicates
'\\' symbols and such, thus mangling filepaths.
Example:
    JSON obj;
    obj = "C:\\Games";
    cout << obj.ToString();
    obj = obj.ToString();
    cout << obj.ToString();
    obj = obj.ToString();
    cout << obj.ToString();
*/
struct StringAdapterNoEscape : public JSONAdapter<std::string, JSON::Class::String>
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
		source = obj.ToStringNoEscape();
		return true;
	}
};
