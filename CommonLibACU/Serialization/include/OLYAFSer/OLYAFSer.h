#pragma once

#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"

// Oh Look, Yet Another Serialization.
// This one is oriented at global, statically defined configs with members
// that are more or less trivial like bool, number, string, enum
// etc. with support for nested config sections.
// No complicated behavior in members,
// as the "configs" are holding vectors of member proxies now.
// Conceptually, I'm serializing a POD structure with subsections.

class YAConfigSection;

// Non-templated base so that the templated derived classes can be stored in a vector.
class ConfigMember_Base
{
protected:
    friend YAConfigSection;
    const std::string m_ThisMemberName;
    ConfigMember_Base(const std::string& thisMemberName);
public:
    virtual void FromJSON(JSON& cfg) = 0;
    virtual void ToJSON(JSON& cfg) = 0;
private:
    void AddThisMemberToCurrentContext();
};
template<class UnderlyingType, class Adapter>
class ConfigMember : public ConfigMember_Base
{
private:
    UnderlyingType m_ActualVariable;
public:
    UnderlyingType& get() { return m_ActualVariable; }
    UnderlyingType* operator->() { return &m_ActualVariable; }
    operator UnderlyingType& () { return m_ActualVariable; }
    void operator=(const UnderlyingType& rhs) { m_ActualVariable = rhs; }
    ConfigMember(const std::string& thisMemberName)
        : ConfigMember_Base(thisMemberName)
    {}
    ConfigMember(const std::string& thisMemberName, const UnderlyingType& initialValue)
        : ConfigMember_Base(thisMemberName)
        , m_ActualVariable(initialValue)
    {}
    // These use the Adapter.
    virtual void FromJSON(JSON& jsonValueForThisKey) override
    {
        Adapter(m_ActualVariable).FromJSON(jsonValueForThisKey);
    }
    virtual void ToJSON(JSON& jsonValueForThisKey) override
    {
        jsonValueForThisKey = Adapter(m_ActualVariable).ToJSON();
    }
};
class YAConfigSection
{
private:
    std::vector<ConfigMember_Base*> m_MemberProxies;
protected:
    struct PleaseUseTheMacroInsideTheSubclassToDefineTheDefaultConstructorBecauseTheContextNeedsToBeRestoredAfterSubclassInitializes;
protected:
    YAConfigSection(PleaseUseTheMacroInsideTheSubclassToDefineTheDefaultConstructorBecauseTheContextNeedsToBeRestoredAfterSubclassInitializes);
    void RestorePreviousContext();
public:
    void SectionToJSON(JSON& jsonOut);
    void SectionFromJSON(JSON& jsonObject);
private:
    friend ConfigMember_Base;
    void AddMember(ConfigMember_Base& member);
    YAConfigSection* m_PreviousContext;
protected:
    struct PleaseUseTheMacroInsideTheSubclassToDefineTheDefaultConstructorBecauseTheContextNeedsToBeRestoredAfterSubclassInitializes {};
};
#define YACONFIGSECTION_SUBCLASS_CTOR(SubclsName)\
SubclsName()\
    : YAConfigSection(PleaseUseTheMacroInsideTheSubclassToDefineTheDefaultConstructorBecauseTheContextNeedsToBeRestoredAfterSubclassInitializes())\
    /* magic of the global context manipulation has happened */\
{ RestorePreviousContext(); }

class YAConfigSectionAdapter : public JSONAdapter<YAConfigSection, JSON::Class::Object>
{
public:
    using BaseAdapter::BaseAdapter;
    JSON ToJSON();
    bool FromJSON(JSON& obj);
};

#define ADD_CONFIG_MEMBER(varName, VarType, AdapterType, optionalDefaultValue)\
    ConfigMember<VarType, AdapterType> varName{ #varName, optionalDefaultValue };
