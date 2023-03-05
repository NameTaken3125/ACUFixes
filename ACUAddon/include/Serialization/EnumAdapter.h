#pragma once

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
        this->source = enum_reflection<EnumType>::GetValue(obj.ToString());
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
