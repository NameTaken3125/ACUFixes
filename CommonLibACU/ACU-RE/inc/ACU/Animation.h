#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class ObjectAllocator : public ManagedObject
{
public:
    char pad_0010[8]; //0x0010
}; //Size: 0x0018
assert_sizeof(ObjectAllocator, 0x18);

class AnimTrackEvent;


class AnimTrackDataMapping
{
public:
    uint32 TrackID; //0x0000
}; //Size: 0x0004
assert_sizeof(AnimTrackDataMapping, 4);
class Animation;
class AnimTrackData
{
public:
    uint32 AnimTrackDataKey; //0x0000
    uint16 NumRefs; //0x0004
    SmallArray<AnimTrackDataMapping> AnimTrackDataMapping_; //0x0006
    char pad_0012[6]; //0x0012
    Animation* animation; //0x0018
}; //Size: 0x0020
assert_sizeof(AnimTrackData, 0x20);

enum class UserResourceCategory
{
    _Unknown = 0,
    CHR_Misc = 1,
    CHR_Crowd = 2,
    CHR_Unique = 3,
    CHR_Heads = 4,
    CHR_Weapon = 5,
    CIN_Misc = 6,
    ENV_Exotic = 7,
    DBG_Misc = 8,
    ENV_Misc = 9,
    ENV_Building = 10,
    ENV_BuildingGeneric = 11,
    ENV_Landmark = 12,
    ENV_LandmarkInterior = 13,
    ENV_LandmarkGeneric = 14,
    ENV_Prop = 15,
    ENV_Ground = 16,
    ENV_Water = 17,
    ENV_Sky = 18,
    ENV_Matte = 19,
    ENV_Vehicle = 20,
    ENV_Vegetation = 21,
    ENV_Fake = 22,
    ENV_Lighting = 23,
    ENV_LocalCubeMap = 24,
    ENV_Decals = 25,
    GFX_Misc = 26,
    GFX_Animus = 27,
    ENV_LGS = 28,
    ENV_Underground = 29,
    ENV_Illegal = 30,
    MISC_Collectibles = 31,
    MISC_Illegal = 32,
    MIS_Misc = 33,
    UI_Misc = 34,
    CHR_Shared = 35,
    CHR_Allies = 36,
    CHR_Enemies = 37,
    CHR_Gameplay = 38,
    ENV_Clutter = 39,
    CHR_Animals = 40,
    CHR_Playable = 41,
    CHR_HighRes = 42,
    CHR_Props = 43,
    CHR_CinProps = 44,
    CHR_Illegal = 45,
    ANM_Facial = 46,
    ANM_Cinematics = 47,
    ANM_Cinematics_Streaming = 48,
    ANM_Scripted_Events = 49,
    ANM_Animals = 50,
    ANM_Animal_Horse = 51,
    ANM_Objects = 52,
    ANM_World_Specific = 53,
    ANM_CS_Char_Specific = 54,
    ANM_CS_Gameplay = 55,
    ANM_CS_Crowd = 56,
    ANM_CS_Unique = 57,
    ANM_CS_CrowdCAS = 58,
    ANM_CS_CrowdLife = 59,
    ANM_CG_Char_Generic = 60,
    ANM_CG_Navigation = 61,
    ANM_CG_Nav_Climb = 62,
    ANM_CG_Nav_Climb_AC3 = 63,
    ANM_CG_Nav_Ground = 64,
    ANM_CG_Nav_Tree = 65,
    ANM_CG_Nav_Horse = 66,
    ANM_CG_Nav_InAir = 67,
    ANM_CG_Fight = 68,
    ANM_CG_Fight_AC3 = 69,
    ANM_CG_Behavior = 70,
    ANM_Camera = 71,
    ANM_GFX = 72,
    MISC_IgnoreRegionLayoutMeshShape = 73,
    SND_RegionLayout = 74,
    SHDR_Characters = 75,
    SHDR_World = 76,
    SHDR_Fx = 77,
    SHDR_Misc = 78,
    GFX_Illegal = 79,
    Total_NumUserCategory = 80,
};

class Animation : public ObjectAllocator
{
public:
    // @members
    SmallArray<uint16> TeleportAtTime; //0x0018
    SmallArray<void*> arr24; //0x0024
    SmallArray<AnimTrackEvent*> EventsTracks; //0x0030
    char pad_003C[4]; //0x003C
    AnimTrackData* AnimTrackData_; //0x0040
    float Length; //0x0048
    uint32 AnimationKey; //0x004C

    bool Additive : 1; //0x0050
    bool AdditiveWithAlwaysMatchBlendLength : 1;
    bool UseGlobalCoordsForReferenceBone : 1;
    bool IsStreamed : 1;

    char pad_0051[15]; //0x0051
    SmallArray<void*> rawTracks; //0x0060
    char pad_006C[4]; //0x006C

    // @helper_functions
    static TypeInfo& GetTI() { return *(TypeInfo*)0x1439CD110; }
    static constexpr uint64 GetVTBL() { return 0x142E7ACC0; }
}; //Size: 0x0070
assert_sizeof(Animation, 0x70);
