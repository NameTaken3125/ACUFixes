
#include "OLYAFSer/OLYAFSer.h"


#include "Serialization/EnumAdapter.h"

#include "Serialization/enumFactory.h"
#define ENUM_AS_MACRO_EnumKeyCode(XX) \
    XX(K_A, = 30) \
    XX(K_B, = 48) \
    XX(K_C, = 46) \
    XX(K_NUMPAD_0, = 82) \
    XX(K_NUMPAD_5, = 76) \
    XX(K_F1, = 59) \
    XX(K_F2, = 60)
DECLARE_ENUM(EnumKeyCode, ENUM_AS_MACRO_EnumKeyCode)
#define ENUM_AS_MACRO_EnumLanguage(XX) \
    XX(English, = 1) \
    XX(German, = 2) \
    XX(Indonesian, = 3)
DECLARE_ENUM(EnumLanguage, ENUM_AS_MACRO_EnumLanguage)

#define ACM(varName, VarType, AdapterType, optionalDefaultValue) ADD_CONFIG_MEMBER(varName, VarType, AdapterType, optionalDefaultValue)
#define YACSTOR(SubclsName) YACONFIGSECTION_SUBCLASS_CTOR(SubclsName)
struct ExampleConfig : YAConfigSection {
    YACSTOR(ExampleConfig);
    ACM(guiOpenByDefault, bool, BooleanAdapter, true);
    ACM(toggleGUIhotkey, EnumKeyCode, EnumAdapter_template<EnumKeyCode>, EnumKeyCode::K_NUMPAD_0);
    ACM(language, EnumLanguage, EnumAdapter_template<EnumLanguage>, EnumLanguage::English);
    struct Cheats : YAConfigSection {
        YACSTOR(Cheats);
        struct Godmode : YAConfigSection {
            YACSTOR(Godmode);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(hotkey, EnumKeyCode, EnumAdapter_template<EnumKeyCode>, EnumKeyCode::K_B);
        };
        ACM(godmode, Godmode, YAConfigSectionAdapter, );

        ACM(invisibility, bool, BooleanAdapter, true);
        ACM(refillAmmoHotkey, EnumKeyCode, EnumAdapter_template<EnumKeyCode>, EnumKeyCode::K_C);
        ACM(bugfixes, bool, BooleanAdapter, false);
    };
    ACM(cheats, Cheats, YAConfigSectionAdapter, );
};

//#include "imgui/imgui.h"
#define dprintf(what)       //ImGui::Text(what)
#define dseparator()        //ImGui::Separator()
void YAConfigTest()
{
    ExampleConfig cfg;
    EnumKeyCode hk = cfg.cheats->godmode->hotkey;
    JSON jsonOut;
    cfg.SectionToJSON(jsonOut);
    dprintf("ExampleConfig default values:");
    dseparator();
    dprintf(jsonOut.dump().c_str());
    cfg.guiOpenByDefault = false;
    cfg.toggleGUIhotkey = EnumKeyCode::K_F1;
    cfg.language = EnumLanguage::Indonesian;
    cfg.cheats->bugfixes = true;
    cfg.cheats->godmode->hotkey = EnumKeyCode::K_NUMPAD_5;
    cfg.cheats->refillAmmoHotkey = EnumKeyCode::K_F2;
    dseparator();
    dprintf("ExampleConfig after direct modification:");
    dseparator();
    cfg.SectionToJSON(jsonOut);
    dprintf(jsonOut.dump().c_str());
}
