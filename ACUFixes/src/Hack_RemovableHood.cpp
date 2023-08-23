/*
Thank you to Vector4782
on NexusMods for the original "Hood Down Outfits Pack"
https://www.nexusmods.com/assassinscreedunity/users/98797348
https://www.nexusmods.com/assassinscreedunity/mods/93
and kamzik123
for AnvilToolkit
https://www.nexusmods.com/assassinscreedunity/users/84094778
https://www.nexusmods.com/assassinscreedunity/mods/38

There are several cutscenes in the game where Arno can be seen with his hood down.
Not all outfits support this (e.g. Altair's and Ezio's outfits don't),
but most do.
The player's `Entity` has several components of type `Visual`. The exact number depends
on which specific Gear items are currently equipped.
If the equipped Hood or Outfit has a "hood down" version, then the `Entity` simultaneously
has separate `Visual` components for the "hood up" and "hood down" versions.
When the hood is taken off, the visibility for the "hood on" version is turned off,
and for the "hood off" is turned on. The visibility of Arno's hair is also toggled
(there are 2 separate `Visual` components for it)
(the Franciade/Raider hoods also have a partial hair mesh with yet another `Visual` component -
it can be seen when the hood is on).

In this little hack, I identify the relevant `Visual` components
and toggle visibility for them.
To identify them, I search the array of player's components for `Visual`s that
have a `LODSelector` member with a particular Handle.
Most of these handles were discovered using the AnvilToolkit to unpack the following file:
    DataPC_ACU_LGS_BelleEpoque.forge\334_-_CN_P_Arno.data\122_-_CN_P_ArnoAvatar_Hoods.BuildTable
    // Note: In game files, "Hood Down" means that the hood is off.
The Handles for outfits not present in that file were caught by setting a breakpoint at
    `Visual::SetVisibility()` 0x1421F4DD0
and catching the `Visual`s that belong to player's `Entity`
when the Hood mode changes in the Sequence 7, Mission 1: Cautious Alliance.
*/
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
    for (Component* cpnt : entity.cpnts_mb)
    {
        if (*(uint64*)cpnt == Visual__VTable)
        {
            Visual* vis = static_cast<Visual*>(cpnt);
            if (vis->shared_LODSelector->handle == lodSelectorHandle)
            {
                Visual__ToggleVisibility(vis, doEnable);
                // In unmodded game, there should only be a single matching `Visual`,
                // but some Forge Mods can result in player's entity having duplicate `Visual`s.
                // If I don't toggle their visibility accordingly, I'll end up with hair on top of hood.
                // So I continue walking the array of components.
            }
        }
    }
}
struct HoodVariation
{
    HoodVariation(
        const std::string& name
        , uint64 handlePrimaryIdentifierWhenOn
        , std::vector<uint64>&& handlesWhenOn
        , std::vector<uint64>&& handlesWhenOff
    )
        : name(name)
        , handlePrimaryIdentifierWhenOn(handlePrimaryIdentifierWhenOn)
        , handlesWhenOn(std::move(handlesWhenOn))
        , handlesWhenOff(std::move(handlesWhenOff))
    {}
    std::string name;
    uint64 handlePrimaryIdentifierWhenOn;
    std::vector<uint64> handlesWhenOn;
    std::vector<uint64> handlesWhenOff;
};
class HoodVariations
{
public:
    HoodVariation* FindByHandle_HoodOn(uint64 handleHoodOn)
    {
        for (HoodVariation& hood : m_Hoods)
        {
            if (hood.handlePrimaryIdentifierWhenOn == handleHoodOn)
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
HoodVariations g_Hoods({
    HoodVariation("CN_P_FR_Hood_Down_Arnaud_V1",         119263477692, {119263477692}, {147798842981, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Arnaud_V2",          84857383766, { 84857383766}, {147798842259, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Arnaud_V3",         120094245449, {120094245449}, {147798843584, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Arnaud_V4",         120094245753, {120094245753}, {147798843762, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Rodeur_V1",         138594120216, {138594120216}, {138594120217, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Rodeur_V2",         138594120220, {138594120220}, {138594120219, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Rodeur_V3",         138594120218, {138594120218}, {138594120213, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Rodeur_V4",         138594120215, {138594120215}, {138594120214, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Revolutionnaire_V1", 102704136287, {102704136287}, {199109962876, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Revolutionnaire_V2", 102704136472, {102704136472}, {199109963034, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Revolutionnaire_V3", 102704136620, {102704136620}, {200166425279, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Revolutionnaire_V4", 102704136768, {102704136768}, {200166425449, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Napoleonian_V1",     136051788985, {136051788985}, {146628658171, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Napoleonian_V2",      97499925652, { 97499925652}, {146628658936, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Napoleonian_V3",     136051789395, {136051789395}, {146628659620, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_HoodDown_Napoleonian_V4",     136051789853, {136051789853}, {146628659894, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Phantom_V1",        132284718496, {132284718496}, {161367850706, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Phantom_V2",        132284718507, {132284718507}, {161893646994, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Phantom_V3",        132284718492, {132284718492}, {161893647341, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Phantom_V4",        158617568238, {158617568238}, {161893649233, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Musketeers_v1",     161893654071, {161893654071}, {161893654073, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Musketeers_v2",     161893654101, {161893654101}, {161893654072, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Musketeers_V3",     161893654102, {161893654102}, {161893654074, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Musketeers_V4",     161893654099, {161893654099}, {161893654100, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Medieval_V1",       161893651856, {161893651856}, {161893651859, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Medieval_V2",       161893651854, {161893651854}, {161893651855, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Medieval_V3",       161893651857, {161893651857}, {161893651853, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Medieval_V4",       161893651860, {161893651860}, {161893651858, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Military_V1",       161893654931, {161893654931}, {161893654930, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Military_V2",       161893654928, {161893654928}, {161893654929, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Military_V3",       161893654927, {161893654927}, {161893654934, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Military_V4",       161893654932, {161893654932}, {161893654933, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Brigands_V1",       161893653164, {161893653164}, {161893653165, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Brigands_V2",       161893653240, {161893653240}, {161893653241, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Brigands_V3",       161893653317, {161893653317}, {161893653316, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Brigands_V4",       161893653393, {161893653393}, {161893653392, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Noble_V1",          156675642145, {156675642145}, {161893658204, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Noble_V2",          156675642146, {156675642146}, {161893658202, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Noble_V3",          156675642128, {156675642128}, {161893658201, handle_hair1, handle_hair2}),
    HoodVariation("CN_P_FR_Hood_Down_Noble_V4",          156675642129, {156675642129}, {161893658203, handle_hair1, handle_hair2}),
    HoodVariation("McFarlane",                           0x25AB1EB4A0, {0x25AB1EB4A0}, {0x25AB1EB4A1, handle_hair1, handle_hair2}),
    HoodVariation("Connor",                              0x234E8D4922, {0x234E8D4922}, {0x234E8D4923, handle_hair1, handle_hair2}),
    HoodVariation("Edward",                              0x234E8D727E, {0x234E8D727E}, {0x234E8D727D, handle_hair1, handle_hair2}),
    HoodVariation("Arno Fearless",                       0x23DAB1789C, {0x23DAB1789C}, {0x2626CE15AE, handle_hair1, handle_hair2}),
    HoodVariation("Hood of Franciade",                   0x3DA29AFE88, {0x3DA29AFE88, handle_greyHairOfFranciadeHoods}, {0x3DA29AFE60, handle_hair1, handle_hair2}),
    HoodVariation("Hood of the Raiders",                 0x3DA29AFD6B, {0x3DA29AFD6B, handle_greyHairOfFranciadeHoods}, {0x3DA29AFD85, handle_hair1, handle_hair2}),
    HoodVariation("Outfit - Franciade",                  0x3DA29AFBF6, {0x3DA29AFBF6, handle_greyHairOfFranciadeHoods}, {0x3DA29AFB58, handle_hair1, handle_hair2}),
    HoodVariation("Outfit - Raider",                     0x3DA29AFC85, {0x3DA29AFC85, handle_greyHairOfFranciadeHoods}, {0x3DA29AFCA1, handle_hair1, handle_hair2}),
    HoodVariation("Napoleon Artillery Outfit",           264721070714, {264721070714}, {}),
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
void FindPlayerFaceComponentAndApplyTheMaterialWithoutTheFakeHoodShadow(Entity& arno, const bool trueAddFalseRemove);
void ToggleHoodVisuals(HoodVariation& hood, bool doPutHoodOn)
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return; }
    for (uint64 handle : hood.handlesWhenOn)
        EnableVisibilityForVisualCpnt(*player, handle, doPutHoodOn);
    for (uint64 handle : hood.handlesWhenOff)
        EnableVisibilityForVisualCpnt(*player, handle, !doPutHoodOn);
    FindPlayerFaceComponentAndApplyTheMaterialWithoutTheFakeHoodShadow(*player, !doPutHoodOn);
}
#include "ACU_SoundUtils.h"
#include "ACU/ACUGlobalSoundSet.h"
void PlaySound_ToggleHood(Entity& player, bool doPutItOn)
{
    auto* agss = ACUGlobalSoundSet::GetSingleton();
    if (!agss) { return; }

    SoundInstance& sound =
        doPutItOn
        ? agss->arrSoundEvents_38[ACUGlobalSoundSet::k_ClothSoundSharp]
        : agss->arrSoundEvents_38[ACUGlobalSoundSet::k_LikeStartDisguiseButSharper];
    ACU::Sound::PlaySoundFromEntity(sound, player);
}
bool IsToggleHoodSoundEnabled()
{
    return true;
}
void ToggleHood()
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return; }
    CurrentHoodState currentHood = FindCurrentHoodVariation(*player);
    if (!currentHood.m_currentHood) { return; }
    ToggleHoodVisuals(*currentHood.m_currentHood, !currentHood.m_isHoodOn);
    if (IsToggleHoodSoundEnabled())
        PlaySound_ToggleHood(*player, !currentHood.m_isHoodOn);
}
#include "Common_Plugins/ACU_InputUtils.h"
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
#include "ACU/LODSelectorInstance.h"
class MeshInstanceData;
class Material;
class FXMaterialOverlayContainer_10_0
{
public:
    MeshInstanceData* meshInstData; //0x0000
    SharedPtrNew<Material>* shared_material8; //0x0008
    SharedPtrNew<Material>* shared_material10; //0x0010
    SmallArray<SharedPtrNew<Material>*> arrSharedMaterials; //0x0018
    char pad_0024[4]; //0x0024
}; //Size: 0x0028
assert_offsetof(FXMaterialOverlayContainer_10_0, arrSharedMaterials, 0x18);
class MeshInstanceData_3C;
class MeshInstanceData_48;
class MeshInstanceData_58;
class Mesh;
class MeshInstanceData : public GraphicObjectInstanceData
{
public:
    Mesh* mesh; //0x0028
    SmallArray<FXMaterialOverlayContainer_10_0*> arr30; //0x0030
    SmallArray<MeshInstanceData_3C> arr3C; //0x003C
    SmallArray<MeshInstanceData_48> arr48; //0x0048
    char pad_0054[4]; //0x0054
    MeshInstanceData_58* p58; //0x0058
    char pad_0060[24]; //0x0060
}; //Size: 0x0078
assert_sizeof(MeshInstanceData, 0x78);
DEFINE_GAME_FUNCTION(FXMaterialOverlayContainer_10_0__AddToPendingOverlayCommands_AddOverlay, 0x1421BE490, void, __fastcall, (FXMaterialOverlayContainer_10_0* a1, Material* a2, unsigned __int8 a3));
DEFINE_GAME_FUNCTION(FXMaterialOverlayContainer_10_0__AddToPendingOverlayCommands_RemoveOverlay, 0x1421DEB50, void, __fastcall, (FXMaterialOverlayContainer_10_0* a1, Material* a2));
Material* TryFindMaterialToApply(LODSelectorInstance& faceLODs)
{
    MeshInstanceData* meshInstDataHighestLOD_mb = faceLODs.meshInstDatas[0];
    if (!meshInstDataHighestLOD_mb) {
        return nullptr;
    }
    if (!meshInstDataHighestLOD_mb->arr30.size) { return nullptr; }
    return meshInstDataHighestLOD_mb->arr30[0]->shared_material8->GetPtr();
}
constexpr uint64 handle_ArnoFace = 211488955670;
void FindPlayerFaceComponentAndApplyTheMaterialWithoutTheFakeHoodShadow(Entity& arno, const bool trueAddFalseRemove)
{
    Visual* arnoFaceVisual = FindVisualCpnt(arno, handle_ArnoFace);
    if (!arnoFaceVisual) { return; }
    LODSelectorInstance* faceLODs = arnoFaceVisual->LODSelectorInstance_;
    if (!faceLODs)
    {
        return;
    }
    Material* appliedMaterial = TryFindMaterialToApply(*faceLODs);
    if (!appliedMaterial)
    {
        return;
    }
    for (MeshInstanceData* meshInstData : arnoFaceVisual->LODSelectorInstance_->meshInstDatas)
    {
        if (!meshInstData->arr30.size) { continue; }
        FXMaterialOverlayContainer_10_0* a1 = meshInstData->arr30[0];
        if (trueAddFalseRemove)
        {
            FXMaterialOverlayContainer_10_0__AddToPendingOverlayCommands_AddOverlay(
                a1,
                appliedMaterial,
                1
            );
        }
        else
        {
            FXMaterialOverlayContainer_10_0__AddToPendingOverlayCommands_RemoveOverlay(
                a1,
                appliedMaterial
            );
        }
    }
}
#include "ImGuiConfigUtils.h"
void DrawEntityVisualsControls(Entity& entity)
{
    static std::string fmt;
    for (Component* cpnt : entity.cpnts_mb)
    {
        if (*(uint64*)cpnt != Visual__VTable)
        {
            continue;
        }
        Visual* vis = (Visual*)cpnt;
        ImGui::PushID(vis);
        bool isVisible = !vis->flags.isHidden;
        fmt.clear();
        fmt = std::to_string(vis->shared_LODSelector->handle);
        if (ImGui::Checkbox(fmt.c_str(), &isVisible))
        {
            Visual__ToggleVisibility(vis, isVisible);
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::SetClipboardText(fmt.c_str());
        }
        ImGui::PopID();
    }
}
void DrawPlayerVisualsControls()
{
    ImGui::Text(
        "The list of player's Visual components to turn on and off."
        "\nThe numbers are the unique Handles of the corresponding LODSelector objects."
        "\nClick with the Right Mouse Button to copy the Handle to clipboard."
    );
    Entity* player = ACU::GetPlayer();
    if (!player) { return; }
    DrawEntityVisualsControls(*player);
}
void DrawHoodControls()
{
    ImGui::Checkbox("Removable Hood", &g_Config.hacks->hoodControls->isActive.get());
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Can take Hood off and put it on by pressing a button."
            "\nActually, since the Hood isn't animated, this is just switching"
            "\nbetween the On and Off versions."
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
