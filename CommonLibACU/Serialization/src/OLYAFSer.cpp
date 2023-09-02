#include "pch.h"

#include "OLYAFSer/OLYAFSer.h"


ConfigMember_Base::ConfigMember_Base(const std::string& thisMemberName)
    : m_ThisMemberName(thisMemberName)
{
    AddThisMemberToCurrentContext();
}
YAConfigSection* g_CurrentContext = nullptr;
void ConfigMember_Base::AddThisMemberToCurrentContext()
{
    g_CurrentContext->AddMember(*this);
}

YAConfigSection::YAConfigSection(PleaseUseTheMacroInsideTheSubclassToDefineTheDefaultConstructorBecauseTheContextNeedsToBeRestoredAfterSubclassInitializes)
    : m_PreviousContext(g_CurrentContext)
{
    g_CurrentContext = this;
}
void YAConfigSection::RestorePreviousContext() { g_CurrentContext = m_PreviousContext; }
void YAConfigSection::SectionToJSON(JSON& jsonOut)
{
    for (auto& member : m_MemberProxies)
    {
        JSON& thisMemberInJSON = jsonOut[member->m_ThisMemberName];
        member->ToJSON(thisMemberInJSON);
    }
}
void YAConfigSection::SectionFromJSON(JSON& jsonObject)
{
    for (auto& member : m_MemberProxies)
    {
        JSON& thisMemberInJSON = jsonObject[member->m_ThisMemberName];
        member->FromJSON(thisMemberInJSON);
    }
}
void YAConfigSection::AddMember(ConfigMember_Base& member)
{
    m_MemberProxies.push_back(&member);
}
JSON YAConfigSectionAdapter::ToJSON()
{
    JSON result;
    this->source.SectionToJSON(result);
    return result;
}
bool YAConfigSectionAdapter::FromJSON(JSON& obj)
{
    if (!this->IsCorrectJSONType(obj))
    {
        return false;
    }
    this->source.SectionFromJSON(obj);
    return true;
}
