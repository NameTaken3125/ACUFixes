#pragma once

#include "Serialization.h"

class BooleanAdapter : public JSONAdapter<bool, JSON::Class::Boolean>
{
public:
	using BaseAdapter::BaseAdapter;
    JSON ToJSON()
    {
        return this->source;
    }
	bool FromJSON(JSON& obj)
	{
		if (!IsCorrectJSONType(obj))
		{
			return false;
		}
		this->source = obj.ToBool();
		return true;
	}
};
