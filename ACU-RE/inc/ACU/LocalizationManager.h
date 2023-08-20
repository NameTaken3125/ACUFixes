#pragma once

#include "MandatoryUniverseComponent.h"
#include "SmallArray.h"
#include "UIString.h"

class GameAILocalizeString;
class LocalizationManager_28;

/*
This type is used when looking up localized strings.
You can use this to turn a `UIString` into an actual text.
For example when in Customization Menu, the `UIString` 0x7AAC2
is turned into L"Legendary Prowler Hood" that is then displayed.


//Example usage:
std::wstring DumpGear(AvatarGear& gear)
{
    std::wstring result;
    ACU_WStringBuffer name{ gear.uiString_gearName };
    ACU_WStringBuffer desc{ gear.uiString_gearDescription };
    ACU_WStringBuffer req1{ gear.uiString_requirementsLine1 };
    ACU_WStringBuffer req2{ gear.uiString_requirementsLine2 };
    result += L"\n    "; result += (name.m_buf ? name.m_buf : L"NULL");
    result += L"\n    "; result += (desc.m_buf ? desc.m_buf : L"NULL");
    result += L"\n    "; result += (req1.m_buf ? req1.m_buf : L"NULL");
    result += L"\n    "; result += (req2.m_buf ? req2.m_buf : L"NULL");
    return result;
}
*/
class ACU_WStringBuffer
{
public:
    // @members
    wchar_t* m_buf = nullptr;

    // @helper_functions
public:
    ACU_WStringBuffer(UIString uiString);
    void Clear();
    ~ACU_WStringBuffer();

public:
    ACU_WStringBuffer(const ACU_WStringBuffer& rhs) = delete;
    ACU_WStringBuffer(ACU_WStringBuffer&& rhs) noexcept = delete;
    ACU_WStringBuffer& operator=(const ACU_WStringBuffer& rhs) = delete;
    ACU_WStringBuffer& operator=(ACU_WStringBuffer&& rhs) noexcept = delete;
};
assert_sizeof(ACU_WStringBuffer, 8);

class LocalizationManager : public MandatoryUniverseComponent
{
public:
    // @members
    char pad_0010[24]; //0x0010
    SmallArray<LocalizationManager_28> arr28; //0x0028
    char pad_0034[4]; //0x0034
    GameAILocalizeString* gameAILocalizeString; //0x0038
    char pad_0040[112]; //0x0040

    // @helper_functions
    static LocalizationManager* GetSingleton() { return *(LocalizationManager**)0x145219CF0; }
}; //Size: 0x00B0
assert_sizeof(LocalizationManager, 0xB0);
