#include "pch.h"


#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ACU/Visual.h"

constexpr uint64 Visual__VTable = 0x14309C7A0;
Visual* FindVisualCpnt(Entity& entity, uint64 lodSelectorHandle)
{
    for (Component* cpnt : entity.cpnts_mb)
    {
        if (*(uint64*)cpnt == Visual__VTable)
        {
            Visual* vis = static_cast<Visual*>(cpnt);
            if (vis->shared_LODSelector->handle == lodSelectorHandle)
            {
                return vis;
            }
        }
    }
    return nullptr;
}

#include "ACU_DefineNativeFunction.h"
DEFINE_GAME_FUNCTION(Visual__ToggleVisibility, 0x1421F4DD0, int, __fastcall, (Visual* a1, unsigned char a2));
//DEFINE_GAME_FUNCTION(Visual__ToggleVisibility_P, 0x141CE1440, int, __fastcall, (Visual* a1, unsigned char a2));
void EnableVisibilityForVisualCpnt(Entity& entity, uint64 lodSelectorHandle, bool doEnable)
{
    Visual* mph = FindVisualCpnt(entity, lodSelectorHandle);
    if (!mph) { return; }
    Visual__ToggleVisibility(mph, doEnable);
}
struct HoodVariation
{
    HoodVariation(
        const std::string& name
        , uint64 handleHoodOn
        , uint64 handleHoodOff
        , std::optional<uint64> handleHoodOnExtra = {}
    )
        : name(name)
        , handle_hoodOn(handleHoodOn)
        , handle_hoodOff(handleHoodOff)
        , handle_hoodOnExtra(handleHoodOnExtra)
    {}
    std::string name;
    uint64 handle_hoodOn;
    uint64 handle_hoodOff;
    std::optional<uint64> handle_hoodOnExtra;
};
class HoodVariations
{
public:
    HoodVariation* FindByHandle_HoodOn(uint64 handleHoodOn)
    {
        for (HoodVariation& hood : m_Hoods)
        {
            if (hood.handle_hoodOn == handleHoodOn)
            {
                return &hood;
            }
        }
        return nullptr;
    }
    HoodVariations(std::vector<HoodVariation>&& hoods) : m_Hoods(std::move(hoods)) {}
private:
    std::vector<HoodVariation> m_Hoods;
};
constexpr uint64 handle_hair1 = 0x17412AB3AC;
constexpr uint64 handle_hair2 = 0x17412AB39C;
constexpr uint64 handle_greyHairOfFranciadeHoods = 0x2318E0DEA8;
// Note: "Hood Down" means that hood is off.
HoodVariations g_Hoods({
    HoodVariation( "CN_P_FR_Hood_Down_Arnaud_V1", 119263477692, 147798842981 ),
    HoodVariation( "CN_P_FR_Hood_Down_Arnaud_V2", 84857383766, 147798842259 ),
    HoodVariation( "CN_P_FR_Hood_Down_Arnaud_V3", 120094245449, 147798843584 ),
    HoodVariation( "CN_P_FR_Hood_Down_Arnaud_V4", 120094245753, 147798843762 ),
    HoodVariation( "CN_P_FR_Hood_Down_Rodeur_V1", 138594120216, 138594120217 ),
    HoodVariation( "CN_P_FR_Hood_Down_Rodeur_V2", 138594120220, 138594120219 ),
    HoodVariation( "CN_P_FR_Hood_Down_Rodeur_V3", 138594120218, 138594120213 ),
    HoodVariation( "CN_P_FR_Hood_Down_Rodeur_V4", 138594120215, 138594120214 ),
    HoodVariation( "CN_P_FR_HoodDown_Revolutionnaire_V1", 102704136287, 199109962876 ),
    HoodVariation( "CN_P_FR_HoodDown_Revolutionnaire_V2", 102704136472, 199109963034 ),
    HoodVariation( "CN_P_FR_HoodDown_Revolutionnaire_V3", 102704136620, 200166425279 ),
    HoodVariation( "CN_P_FR_HoodDown_Revolutionnaire_V4", 102704136768, 200166425449 ),
    HoodVariation( "CN_P_FR_HoodDown_Napoleonian_V1", 136051788985, 146628658171 ),
    HoodVariation( "CN_P_FR_HoodDown_Napoleonian_V2", 97499925652, 146628658936 ),
    HoodVariation( "CN_P_FR_HoodDown_Napoleonian_V3", 136051789395, 146628659620 ),
    HoodVariation( "CN_P_FR_HoodDown_Napoleonian_V4", 136051789853, 146628659894 ),
    HoodVariation( "CN_P_FR_Hood_Down_Phantom_V1", 132284718496, 161367850706 ),
    HoodVariation( "CN_P_FR_Hood_Down_Phantom_V2", 132284718507, 161893646994 ),
    HoodVariation( "CN_P_FR_Hood_Down_Phantom_V3", 132284718492, 161893647341 ),
    HoodVariation( "CN_P_FR_Hood_Down_Phantom_V4", 158617568238, 161893649233 ),
    HoodVariation( "CN_P_FR_Hood_Down_Musketeers_v1", 161893654071, 161893654073 ),
    HoodVariation( "CN_P_FR_Hood_Down_Musketeers_v2", 161893654101, 161893654072 ),
    HoodVariation( "CN_P_FR_Hood_Down_Musketeers_V3", 161893654102, 161893654074 ),
    HoodVariation( "CN_P_FR_Hood_Down_Musketeers_V4", 161893654099, 161893654100 ),
    HoodVariation( "CN_P_FR_Hood_Down_Medieval_V1", 161893651856, 161893651859 ),
    HoodVariation( "CN_P_FR_Hood_Down_Medieval_V2", 161893651854, 161893651855 ),
    HoodVariation( "CN_P_FR_Hood_Down_Medieval_V3", 161893651857, 161893651853 ),
    HoodVariation( "CN_P_FR_Hood_Down_Medieval_V4", 161893651860, 161893651858 ),
    HoodVariation( "CN_P_FR_Hood_Down_Military_V1", 161893654931, 161893654930 ),
    HoodVariation( "CN_P_FR_Hood_Down_Military_V2", 161893654928, 161893654929 ),
    HoodVariation( "CN_P_FR_Hood_Down_Military_V3", 161893654927, 161893654934 ),
    HoodVariation( "CN_P_FR_Hood_Down_Military_V4", 161893654932, 161893654933 ),
    HoodVariation( "CN_P_FR_Hood_Down_Brigands_V1", 161893653164, 161893653165 ),
    HoodVariation( "CN_P_FR_Hood_Down_Brigands_V2", 161893653240, 161893653241 ),
    HoodVariation( "CN_P_FR_Hood_Down_Brigands_V3", 161893653317, 161893653316 ),
    HoodVariation( "CN_P_FR_Hood_Down_Brigands_V4", 161893653393, 161893653392 ),
    HoodVariation( "CN_P_FR_Hood_Down_Noble_V1", 156675642145, 161893658204 ),
    HoodVariation( "CN_P_FR_Hood_Down_Noble_V2", 156675642146, 161893658202 ),
    HoodVariation( "CN_P_FR_Hood_Down_Noble_V3", 156675642128, 161893658201 ),
    HoodVariation( "CN_P_FR_Hood_Down_Noble_V4", 156675642129, 161893658203 ),
    HoodVariation( "McFarlane", 0x25AB1EB4A0, 0x25AB1EB4A1 ),
    HoodVariation( "Connor", 0x234E8D4922, 0x234E8D4923 ),
    HoodVariation( "Edward", 0x234E8D727E, 0x234E8D727D ),
    HoodVariation( "Arno Fearless", 0x23DAB1789C, 0x2626CE15AE ),

    HoodVariation( "Hood of Franciade", 0x3DA29AFE88, 0x3DA29AFE60, handle_greyHairOfFranciadeHoods ),
    HoodVariation( "Hood of the Raiders", 0x3DA29AFD6B, 0x3DA29AFD85, handle_greyHairOfFranciadeHoods ),
    HoodVariation( "Outfit - Franciade", 0x3DA29AFBF6, 0x3DA29AFB58, handle_greyHairOfFranciadeHoods ),
    HoodVariation( "Outfit - Raider", 0x3DA29AFC85, 0x3DA29AFCA1, handle_greyHairOfFranciadeHoods ),
    });

Visual* ToVisual(Component* cpnt)
{
    if (*(uint64*)cpnt == Visual__VTable)
    {
        return (Visual*)cpnt;
    }
    return nullptr;
}
struct CurrentHoodState
{
    HoodVariation* m_currentHood;
    bool m_isHoodOn;
};
#include "ACU/LODSelectorInstance.h"
CurrentHoodState FindCurrentHoodVariation(Entity& player)
{
    for (Component* cpnt : player.cpnts_mb)
    {
        if (Visual* vis = ToVisual(cpnt))
        {
            uint64 potentialHoodHandleUsedInThisVisual = vis->shared_LODSelector->handle;
            HoodVariation* matchingHood = g_Hoods.FindByHandle_HoodOn(potentialHoodHandleUsedInThisVisual);
            if (matchingHood)
            {
                const bool isVisible = !vis->flags.isHidden;
                return { matchingHood, isVisible };
            }
        }
    }
    return { nullptr, false };
}
void ToggleHoodVisuals(HoodVariation& hood, bool doPutHoodOn)
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return; }
    EnableVisibilityForVisualCpnt(*player, hood.handle_hoodOn, doPutHoodOn);
    if (hood.handle_hoodOnExtra)
    {
        EnableVisibilityForVisualCpnt(*player, *hood.handle_hoodOnExtra, doPutHoodOn);
    }

    EnableVisibilityForVisualCpnt(*player, hood.handle_hoodOff, !doPutHoodOn);
    EnableVisibilityForVisualCpnt(*player, handle_hair1, !doPutHoodOn);
    EnableVisibilityForVisualCpnt(*player, handle_hair2, !doPutHoodOn);
}
void ToggleHood()
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return; }
    CurrentHoodState currentHood = FindCurrentHoodVariation(*player);
    if (!currentHood.m_currentHood) { return; }
    ToggleHoodVisuals(*currentHood.m_currentHood, !currentHood.m_isHoodOn);
}
#include "ACU_InputUtils.h"
#include "MainConfig.h"
void DoManualHoodControls()
{
    if (!g_Config.hacks->hoodControls->isActive)
    {
        return;
    }
    if (ACU::Input::IsJustPressed(g_Config.hacks->hoodControls->hoodToggleButton))
    {
        ToggleHood();
    }
}
#include "ImGuiConfigUtils.h"
void DrawHoodControls()
{
    ImGui::Checkbox("Removable Hood", &g_Config.hacks->hoodControls->isActive.get());
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Can take Hood off and put it on by pressing a button."
            "\nActually, since the Hood isn't animated, this is just switching"
            "\nbetween the On and Off versions."
            "\nBUG: There remains a bad-looking shadow of the removed hood on Arno's forehead."
        );
    }
    if (g_Config.hacks->hoodControls->isActive)
    {
        ImGui::Indent();
        ImGui::DrawEnumPicker(
            "Hood toggle button"
            , g_Config.hacks->hoodControls->hoodToggleButton.get()
            , ImGuiComboFlags_HeightLarge);
        ImGui::Unindent();
    }
}
