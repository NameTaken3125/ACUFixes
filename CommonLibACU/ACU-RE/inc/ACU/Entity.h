#pragma once

#include "BaseEntity.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class Component;

class Entity_CpntIndices_157
{
public:
    char pad_0000[1]; //0x0000
    uint8 atomAnimCpnt; //0x0001
    char pad_0002[6]; //0x0002
}; //Size: 0x0008

class Entity_CpntIndices_15F
{
public:
    uint8 byte_15f; //0x0000
    uint8 cpntIdx_BhvAssassin_or_AIComponent; //0x0001
    char pad_0002[3]; //0x0002
}; //Size: 0x0005
assert_sizeof(Entity_CpntIndices_15F, 5);


class EntityDescriptor
{
public:
    enum class EntityDescriptorType : uint32
    {
        EntityDescriptor_None = 0x00,
        EntityDescriptor_Main = 0x01,
        EntityDescriptor_NPC = 0x02,
        EntityDescriptor_Object = 0x03,
        EntityDescriptor_FarObject = 0x04,
        EntityDescriptor_COUNT = 0x05,
    };
    enum class EntityDescriptorNetworkType : uint32
    {
        EntityDescriptorNetworkType_Any = 0,
        EntityDescriptorNetworkType_Local = 1,
        EntityDescriptorNetworkType_Replica = 2,
        EntityDescriptorNetworkType_Count = 3,
    };
    enum class EntityDescriptorNPCSubType : uint32
    {
        EntityDescriptorNPC_All = 0,
        EntityDescriptorNPC_Templar = 1,
        EntityDescriptorNPC_Peasant = 2,
        EntityDescriptorNPC_Thief = 3,
        EntityDescriptorNPC_Courtesan = 4,
        EntityDescriptorNPC_Mercenary = 5,
        EntityDescriptorNPC_Target = 6,
        EntityDescriptorNPC_CityEventBourgeois = 7,
        EntityDescriptorNPC_CityEventPickpocketeer = 8,
        EntityDescriptorNPC_CityEventOfficial = 9,
        EntityDescriptorNPC_Orator = 10,
        EntityDescriptorNPC_Horse = 11,
        EntityDescriptorNPC_Nurse = 12,
        EntityDescriptorNPC_TemplarAchievement = 13,
        EntityDescriptorNPC_Male_Present_Hardsole = 14,
        EntityDescriptorNPC_Assassin = 15,
        EntityDescriptorNPC_Harasser = 16,
        EntityDescriptorNPC_CesareBorgia = 17,
        EntityDescriptorNPC_WolfmenLeader = 18,
        EntityDescriptorNPC_Animal = 19,
        EntityDescriptorNPC_Haytham = 20,
        EntityDescriptorNPC_HomesteadNPC = 21,
        EntityDescriptorNPC_Drunk = 22,
        EntityDescriptorNPC_NavalNPC = 23,
        EntityDescriptorNPC_CartHorse = 24,
        EntityDescriptorNPC_Criminal = 25,
        EntityDescriptorNPC_Dealer = 26,
        EntityDescriptorNPC_NewspaperBoy = 27,
        EntityDescriptorNPC_Vigilantes = 28,
        EntityDescriptorNPC_RiftNPC = 29,
        EntityDescriptorNPC_FakeStationNPC = 30,
        EntityDescriptorNPC_VillaNPC = 31,
        EntityDescriptorNPC_GroupBhvDriver = 32,
        EntityDescriptorNPC_UniqueNPC = 33,
    };
    enum class EntityDescriptorObjectSubType : uint32
    {
        EntityDescriptorObject_All = 0,
        EntityDescriptorObject_Weapon = 1,
        EntityDescriptorObject_Ladder = 2,
        EntityDescriptorObject_Pole = 3,
        EntityDescriptorObject_Rope = 4,
        EntityDescriptorObject_Breakable = 5,
        EntityDescriptorObject_Kiosk = 6,
        EntityDescriptorObject_Scaffold = 7,
        EntityDescriptorObject_HayStack = 8,
        EntityDescriptorObject_HidingPlace = 9,
        EntityDescriptorObject_Boat = 10,
        EntityDescriptorObject_Horse_Hitch = 11,
        EntityDescriptorObject_Wagon_Cart = 12,
        EntityDescriptorObject_Wheel_Front = 13,
        EntityDescriptorObject_Wheel_Rear = 14,
        EntityDescriptorObject_CherryBomb = 15,
        EntityDescriptorObject_TreasureChest = 16,
        EntityDescriptorObject_WaterWell = 17,
        EntityDescriptorObject_Bench = 18,
        EntityDescriptorObject_SmokeBomb = 19,
        EntityDescriptorObject_CornerSpin = 20,
        EntityDescriptorObject_SpringJump = 21,
        EntityDescriptorObject_Cannon = 22,
        EntityDescriptorObject_Money = 23,
        EntityDescriptorObject_CannonBall = 24,
        EntityDescriptorObject_Lute = 25,
        EntityDescriptorObject_BatteringRamUnit = 26,
        EntityDescriptorObject_Tank = 27,
        EntityDescriptorObject_GenericFocusableEntity = 28,
        EntityDescriptorObject_Watercraft = 29,
        EntityDescriptorObject_WatercraftControlPoint = 30,
        EntityDescriptorObject_Snare = 31,
        EntityDescriptorObject_Tripmine = 32,
        EntityDescriptorObject_PowderKeg = 33,
        EntityDescriptorObject_NavalFort = 34,
        EntityDescriptorObject_NavalProjectileBarrier = 35,
        EntityDescriptorObject_SwivelGun = 36,
        EntityDescriptorObject_Bait = 37,
        EntityDescriptorObject_ShipRowBoat = 38,
        EntityDescriptorObject_FloatingPhysicObject = 39,
        EntityDescriptorObject_PuzzleObject = 40,
        EntityDescriptorObject_Tree = 41,
        EntityDescriptorObject_StunGrenade = 42,
        EntityDescriptorObject_Lantern = 43,
        EntityDescriptorObject_Cart = 44,
        EntityDescriptorObject_UsableObject = 45,
        EntityDescriptorObject_NoCover = 46,
        EntityDescriptorObject_BlendTrap = 47,
        EntityDescriptorObject_RiftMachineGun = 48,
        EntityDescriptorObject_RiftAutomaticMachineGun = 49,
        EntityDescriptorObject_RiftTank = 50,
        EntityDescriptorObject_ForceLowCover = 51,
        EntityDescriptorObject_RiftAirplane = 52,
        EntityDescriptorObject_RiftRelay = 53,
        EntityDescriptorObject_RiftBallisticWeapon = 54,
        EntityDescriptorObject_PowderReserve = 55,
        EntityDescriptorObject_RiftWindObject = 56,
        EntityDescriptorObject_CrowdAnimatedStation = 57,
        EntityDescriptorObject_CrowdAttachableObject = 58,
        EntityDescriptorObject_CrowdAnimatableObject = 59,
        EntityDescriptorObject_RiftCrossbow = 60,
        EntityDescriptorObject_RiftSearchLight = 61,
        EntityDescriptorObject_AlarmBell = 62,
        EntityDescriptorObject_CartHorse = 63,
        EntityDescriptorObject_Count = 64,
    };
    enum class EntityDescriptorTreasureChestSubType : uint32
    {
        EntityDescriptorTreasureChest_None = 0,
        EntityDescriptorTreasureChest_Small = 1,
        EntityDescriptorTreasureChest_Large = 2,
        EntityDescriptorTreasureChest_Small_Locked = 3,
        EntityDescriptorTreasureChest_Large_Locked = 4,
        EntityDescriptorTreasureChest_Lootable_Boat = 5,
    };
    enum class EntityDescriptorExplicitAnimal : uint32
    {
        EntityDescriptorExplicitAnimal_Any = 0,
        EntityDescriptorExplicitAnimal_Wolf = 1,
        EntityDescriptorExplicitAnimal_Bear = 2,
        EntityDescriptorExplicitAnimal_Deer = 3,
        EntityDescriptorExplicitAnimal_Dog = 4,
        EntityDescriptorExplicitAnimal_Turkey = 5,
        EntityDescriptorExplicitAnimal_Chicken = 6,
        EntityDescriptorExplicitAnimal_Fox = 7,
        EntityDescriptorExplicitAnimal_Cougar = 8,
        EntityDescriptorExplicitAnimal_Cow = 9,
        EntityDescriptorExplicitAnimal_GuardDog = 10,
        EntityDescriptorExplicitAnimal_Bobcat = 11,
        EntityDescriptorExplicitAnimal_Elk = 12,
        EntityDescriptorExplicitAnimal_Hare = 13,
        EntityDescriptorExplicitAnimal_Beaver = 14,
        EntityDescriptorExplicitAnimal_Racoon = 15,
        EntityDescriptorExplicitAnimal_Pig = 16,
        EntityDescriptorExplicitAnimal_Cat = 17,
        EntityDescriptorExplicitAnimal_Sheep = 18,
        EntityDescriptorExplicitAnimal_Goat = 19,
        EntityDescriptorExplicitAnimal_Max = 20,
    };
    enum class EntityDescriptorExplicitRiftNPC : uint32
    {
        EntityDescriptorExplicitRiftNPC_Soldier = 0,
        EntityDescriptorExplicitRiftNPC_Agent = 1,
        EntityDescriptorExplicitRiftNPC_Max = 2,
    };
    enum class EntityDescriptorExplicitRiftWindObject : uint32
    {
        EntityDescriptorExplicitRiftWindObject_WoodBarrel = 0,
        EntityDescriptorExplicitRiftWindObject_WoodTable = 1,
        EntityDescriptorExplicitRiftWindObject_Count = 2,
    };
    enum class EntityDescriptorExplicitVillaNPC : uint32
    {
        EntityDescriptorExplicitVillaNPC_Barista = 0,
        EntityDescriptorExplicitVillaNPC_Gardener = 1,
        EntityDescriptorExplicitVillaNPC_Maid = 2,
        EntityDescriptorExplicitVillaNPC_Cook = 3,
        EntityDescriptorExplicitVillaNPC_Intendant = 4,
        EntityDescriptorExplicitVillaNPC_Max = 5,
    };
    enum class EntityDescriptorExplicitUniqueNPC : uint32
    {
        EntityDescriptorExplicitUniqueNPC_Napoleon = 0,
        EntityDescriptorExplicitUniqueNPC_GuillotineVictim = 1,
        EntityDescriptorExplicitUniqueNPC_MissionSpecific1 = 2,
        EntityDescriptorExplicitUniqueNPC_MissionSpecific2 = 3,
        EntityDescriptorExplicitUniqueNPC_MissionSpecific3 = 4,
        EntityDescriptorExplicitUniqueNPC_MissionSpecific4 = 5,
        EntityDescriptorExplicitUniqueNPC_MissionSpecific5 = 6,
        EntityDescriptorExplicitUniqueNPC_Max = 7,
    };

    EntityDescriptorType DescriptorType : 3;
    uint32 SubDescriptorType : 6;
    uint32 ExplicitProperty : 6;
    uint32 IsPickableByPlayer : 1;
    uint32 IsPickableByNPC : 1;
    uint32 IsTargetable : 1;
    EntityDescriptorNetworkType NetworkType : 2;
    uint32 ADBEntryID : 6;
    uint32 HasWideHips : 1;
};
const int x = sizeof(EntityDescriptor);
assert_sizeof(EntityDescriptor, 4);

#include "LODLevel.h"
struct EntityFlags
{
    enum class EntityCategory : uint64
    {
        Unknown = 0x00,
        EntityCharacter = 0x01,
        EntitySmallObject = 0x02,
        EntityBigObject = 0x03,
        EntityInvisibleObject = 0x04,
        EntitySmallBuilding = 0x05,
        EntityBigBuilding = 0x06,
        EntityLandmark = 0x07,
        EntityGround = 0x08,
        EntityOmnipresent = 0x09,
        EntityDebug1 = 0x0A,
        EntityObsolete1 = 0x0B,
        EntityAutoFake = 0x0C,
        EntityLargeMeshChunk = 0x0D,
        EntityMissionShared = 0x0E,
        EntityWorldOnly = 0x0F,
        EntityMissionOnly = 0x10,
        EntityTemporaryHelpers = 0x11,
        EntitySnow = 0x12,
        EntityToolModeOnly = 0x13,
        EntityUIWorld = 0x14,
        EntityUIHUD = 0x15,
        EntityInterior = 0x16,
        EntityUIWorldTop = 0x17,
        EntityHBInvisible = 0x18,
        EntityUIMinimap = 0x19,
        EntityUIWorldmap = 0x1A,
        EntityUIZone = 0x1B,
        EntityUIHUDMask = 0x1C,
    };
    using bool_64 = uint64;

    bool_64 SkipGroupMatrixUpdate : 1;
    bool_64 IsGraphicsUnitTestCameraReferencePosition : 1;
    bool_64 IsSpawned : 1;
    bool_64 IsAutomaticallyRemovedFromWorld : 1;
    bool_64 IsVisible : 1;
    bool_64 WasVisible : 1;
    bool_64 IsPhantom : 1;
    bool_64 IsHidden : 1;

    bool_64 IsSmallObject : 1;
    bool_64 IsMediumObject : 1;
    uint64 RenderLast : 2;
    bool_64 ForceIncludedInReflection : 1;
    bool_64 ForceExcludedFromReflection : 1;
    bool_64 UsePlanarDistanceForGraphicObject : 1;
    bool_64 WasHidden : 1;

    bool_64 IsHiddenByInheritance : 1;
    bool_64 HasMovedFromOrigin : 1;
    bool_64 WasHiddenByAFakeEntity : 1;
    bool_64 IsInitialized : 1;
    LODLevel CurrentLODLevel : 3;
    bool_64 LODLevelDirty : 1;

    bool_64 IsCharacter : 1;
    bool_64 ShadowCaster : 1;
    bool_64 ShadowReceiver : 1;
    bool_64 SunlightReceiver : 1;
    bool_64 DynamicLightsReceiver : 1;
    bool_64 ForbidNotVisibleLODFromRagdoll : 1;
    bool_64 HasSkeletonComponent : 1;
    bool_64 UseVisibilityQueries : 1;

    bool_64 VisibilityOccluder : 1;
    bool_64 AlwaysVisibilityOccluder : 1;
    bool_64 IgnoreOcclusionForLODLevel : 1;
    bool_64 CanUsePreviousOcclusionForLODLevel : 1;
    bool_64 CanUseOcclusionForLODLevel : 1;
    bool_64 RequiresCullingInfo : 1;
    bool_64 VisibleFromMultipleViews : 1;
    bool_64 IgnoreUserCullingPlanes : 1;

    bool_64 IsHiddenByUserCullingPlanes : 1;
    bool_64 UseExactSkeletonUpdateThreadPath : 1;
    bool_64 LockLODToNearest : 1;
    EntityCategory EntityCategory : 5;

    bool_64 UserMask1 : 1;
    bool_64 UserMask2 : 1;
    bool_64 UserMask3 : 1;
    bool_64 UserMask4 : 1;
    bool_64 UserMask5 : 1;
    bool_64 OnEngineStartFlag : 1;
    bool_64 HighResBulk : 1;
    bool_64 ComputeDistanceFromCam : 1;

    bool_64 ManuallySorted : 1;
    bool_64 DistanceFromCameraOverriden : 1;
    bool_64 UIFlag : 1;
    bool_64 IsUIHUDMasked : 1;
    bool_64 IsUIHUDMaskedByInheritance : 1;
    bool_64 IsCullingFlagsValid : 1;
    bool_64 ExcludeEntityBVFromParentGroupBV : 1;
    bool_64 NeverDynamicInStencil : 1;
};
assert_sizeof(EntityFlags, 8);

class BoundingVolume
{
public:
	uint32 Type; //0x0000
	Vector3f Min; //0x0004
	Vector3f Max; //0x0010
	char pad_001C[12]; //0x001C
}; //Size: 0x0028
assert_sizeof(BoundingVolume, 0x28);
class DataLayerAction;
class DataLayerFilter
{
public:
    SmallArray<DataLayerAction> LayerActions; //0x0000
}; //Size: 0x000C
assert_sizeof(DataLayerFilter, 0xC);

class GameStateData;

class Entity : public BaseEntity
{
public:
    // @vtbl

    virtual void Unk000() override;
    virtual void Unk008() override;
    virtual void Unk010_Deserialize(DeserializationStream* deserializationStream) override;
    virtual Object* Unk018_Clone(uint64 a2, uint32 a3) override;
    virtual void Unk020() override;
    virtual TypeInfo& Unk028_GetTI() override;
    virtual void Unk030();
    virtual void Unk038();
    virtual void Unk040();
    virtual void Unk048();
    virtual void Unk050();
    virtual void Unk058();
    virtual void Unk060();
    virtual void Unk068();
    virtual void Unk070();
    virtual void Unk078();
    virtual void Unk080();
    virtual void Unk088();
    virtual void Unk090();
    virtual void Unk098();
    virtual void Unk0A0();
    virtual void Unk0A8();
    virtual void Unk0B0();
    virtual void Unk0B8();
    virtual void Unk0C0();
    virtual void Unk0C8();
    virtual void Unk0D0();
    virtual void Unk0D8();
    virtual void Unk0E0();
    virtual void Unk0E8();
    virtual void Unk0F0();
    virtual void Unk0F8();
    virtual void Unk100();
    virtual void Unk108();
    virtual void Unk110();
    virtual void Unk118();
    virtual void Unk120();
    virtual void Unk128();
    virtual void Unk130();
    virtual void Unk138();
    virtual void Unk140();
    virtual void Unk148();
    virtual void Unk150();
    virtual void Unk158();
    virtual void Unk160();
    virtual void Unk168_FindComponentThatsASubclassForTypeInfo(TypeInfo* ti, Component*& cpntOut);

    // @members
    SmallArray<Component*> cpnts_mb; //0x0078
    char pad_0084[4]; //0x0084
    EntityFlags flags88; //0x0088 // Read below.
    GameStateData* ResetData; //0x0090
    BoundingVolume BoundingVolume_; //0x0098
    char pad_00C0[8]; //0x00C0
    DataLayerFilter DataLayerFilter_; //0x00C8
    EntityDescriptor EntityDescriptor_; //0x00D4
    char pad_00D8[88]; //0x00D8
    SharedPtrNew<Entity>* selfSharedPtr; //0x0130
    char pad_0138[31]; //0x0138
    Entity_CpntIndices_157 cpntIndices_157; //0x0157
    Entity_CpntIndices_15F cpntIndices; //0x015F
    char pad_0164[12]; //0x0164

    // @helper_functions
    Component* FindComponentByVTBL(uint64 vtbl);
    static TypeInfo& GetTI() { return *(TypeInfo*)0x144E6ABB0; }
}; //Size: 0x0170
assert_offsetof(Entity, selfSharedPtr, 0x130);
assert_offsetof(Entity, cpntIndices_157.atomAnimCpnt, 0x158);
assert_sizeof(Entity, 0x0170);

/*
* If `(Entity::flags88 >> 58) & 1 == 1`, then animations will not be affected by bendtime.
* However, actual movement speed will be, and Quickshot projectile might not fly after animation finishes.
*/
