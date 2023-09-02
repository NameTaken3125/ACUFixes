#pragma once


template<typename EnumType>
struct enum_reflection;

template<typename EnumType>
class EnumAdapter_template : public JSONAdapter<EnumType, JSON::Class::String>
{
public:
    using JSONAdapter<EnumType, JSON::Class::String>::JSONAdapter;
    bool FromJSON(JSON& obj)
    {
        if (!this->IsCorrectJSONType(obj))
        {
            return false;
        }
        EnumType parsedValue = enum_reflection<EnumType>::GetValue(obj.ToString());
        if (parsedValue == EnumType::INVALID_FROM_STRING) {
            return false;
        }
        this->source = parsedValue;
        return true;
    }
    JSON ToJSON()
    {
        return enum_reflection<EnumType>::GetString(this->source);
    }
};
template<typename EnumType>
EnumAdapter_template<EnumType> EnumAdapter(EnumType& source)
{
    return { source };
}
