#pragma once

#include "MandatoryUniverseComponent.h"
#include "BallisticProjectileAimingControlsSettings.h"


class SkillSettings : public ManagedObject
{
public:
    char pad_0010[236]; //0x0010
    float eagleVisionDiam; //0x00FC
    float speedToBuildCircleZoneLowerIsFaster; //0x0100
    char pad_0104[12]; //0x0104
    float eagleVisionDiam2; //0x0110
    char pad_0114[164]; //0x0114
}; //Size: 0x01B8
assert_sizeof(SkillSettings, 0x1B8);

class ExplosionSettings
{
public:
    char pad_0000[48]; //0x0000
}; //Size: 0x0030
assert_sizeof(ExplosionSettings, 0x30);

class CommunicationToolsSettings
{
public:
    char pad_0000[96]; //0x0000
}; //Size: 0x0060
assert_sizeof(CommunicationToolsSettings, 0x60);

#include "SmallArray.h"
#include "SharedPtr.h"
class Faction;
class FactionSettings : public ManagedObject
{
public:
    SmallArray<Faction*> factions; //0x0010
    char pad_001C[84]; //0x001C
}; //Size: 0x0070
assert_sizeof(FactionSettings, 0x70);

class TargetingSetting : public Object
{
public:
    float flt_08; //0x0008
    float flt_0C; //0x000C
    float flt_10; //0x0010
    float flt_14; //0x0014
    float flt_18; //0x0018
    float flt_1C; //0x001C
    float flt_20; //0x0020
    float flt_24; //0x0024
    float flt_28; //0x0028
    float flt_2C; //0x002C
    float flt_30; //0x0030
    float flt_34; //0x0034
    float flt_38; //0x0038
    float flt_3C; //0x003C
    float flt_40; //0x0040
    float flt_44; //0x0044
    float flt_48; //0x0048
    uint8 byte_4C; //0x004C
    char pad_004D[3]; //0x004D
}; //Size: 0x0050
assert_sizeof(TargetingSetting, 0x50);

class BallisticProjectileSettings;
class BallisticProjectileSettingsSelector : public Object
{
public:
    SharedPtrNew<BallisticProjectileSettings>* shared_ballProjSett; //0x0008
}; //Size: 0x0010
assert_sizeof(BallisticProjectileSettingsSelector, 0x10);

class AssassinButtonMappingElement;
class AssassinButtonMapping_1C;
class AssassinButtonMapping : public ManagedObject
{
public:
    SmallArray<AssassinButtonMappingElement> arr10; //0x0010
    SmallArray<AssassinButtonMapping_1C> arr1C; //0x001C
}; //Size: 0x0028
assert_sizeof(AssassinButtonMapping, 0x28);

class FightSettings;
class NetFightSettings;
class AwarenessSettingsNew;
class GamePlaySettings_1960_checksEquipmentUnlocked;
class AvatarGearManager;
class AvatarConsumablesManager;
class ParkourModeData;
class Entity;

class GamePlaySettings : public MandatoryUniverseComponent
{
public:
    // @members
    char pad_0010[2880]; //0x0010
    FactionSettings factionSettings_B50; //0x0B50
    char pad_0BC0[624]; //0x0BC0
    TargetingSetting* targetingSettings[27]; //0x0E30
    TargetingSetting defaultTargetingSettings; //0x0F08
    char pad_0F58[128]; //0x0F58
    BallisticProjectileSettingsSelector ballProjSettSelector_SmokeBomb; //0x0FD8
    BallisticProjectileSettingsSelector ballProjSettSelector_CherryBomb; //0x0FE8
    BallisticProjectileSettingsSelector ballProjSettSelector_MoneyPouch; //0x0FF8
    BallisticProjectileSettingsSelector ballProjSettSelector_PoisonBomb; //0x1008
    BallisticProjectileSettingsSelector ballProjSettSelector_Default; //0x1018
    BallisticProjectileSettingsSelector ballProjSettSelector_equip0x19; //0x1028
    BallisticProjectileSettingsSelector ballProjSettSelector_GuillotineGun; //0x1038
    BallisticProjectileSettingsSelector ballProjSettSelector_1048; //0x1048
    SharedPtrNew<Entity>* shared_GEN_ALL_BallisticProjectile; //0x1058
    char pad_1060[104]; //0x1060
    ParkourModeData* parkourModeData; //0x10C8
    char pad_10D0[200]; //0x10D0
    float whenAimBombAtEdgesOfCover_beamOriginHorizontalOffset_1198; //0x1198
    char pad_119C[8]; //0x119C
    float ballisticAimingAngle_11A4; //0x11A4
    float ballisticAimingAngle_11A8; //0x11A8
    char pad_11AC[4]; //0x11AC
    float moneyPouchCooldownTime; //0x11B0
    char pad_11B4[244]; //0x11B4
    uint8 byte_12A8; //0x12A8
    char pad_12A9[271]; //0x12A9
    SkillSettings skillSettings; //0x13B8
    char pad_1570[8]; //0x1570
    BallisticProjectileAimingControlsSettings ballisticProjectileAimingControlsSettings; //0x1578
    char pad_1598[24]; //0x1598
    ExplosionSettings explosionSettings; //0x15B0
    CommunicationToolsSettings communicationToolsSettings; //0x15E0
    char pad_1640[488]; //0x1640
    void* AILockSettings_1828; //0x1828
    char pad_1830[8]; //0x1830
    void* DataLayerSettings; //0x1838
    void* FightSettings_1840; //0x1840
    void* NetFightSettings_1848; //0x1848
    void* FightDirectorSettings; //0x1850
    void* HumanActionMapBank; //0x1858
    void* PerceptionSettings; //0x1860
    void* ShootingSettings; //0x1868
    void* TacticalPositionManagerSettings; //0x1870
    void* AwarenessSettingsNew_1878; //0x1878
    void* BlendingSettings; //0x1880
    void* CrowdEcosystemSettings; //0x1888
    void* RiftScoreSettings; //0x1890
    void* RiftGameSettings; //0x1898
    void* BulkGlobalSettings; //0x18A0
    void* CreedPointsSettings; //0x18A8
    FightSettings* fightSettings; //0x18B0
    NetFightSettings* NetFightSettings_18b8; //0x18B8
    char pad_18C0[40]; //0x18C0
    AwarenessSettingsNew* AwarenessSettingsNew_; //0x18E8
    char pad_18F0[56]; //0x18F0
    AssassinButtonMapping assassinButtonMapping; //0x1928
    char pad_1950[16]; //0x1950
    GamePlaySettings_1960_checksEquipmentUnlocked* avatarSkillTree; //0x1960
    char pad_1968[8]; //0x1968
    SharedPtrNew<AvatarGearManager>* sharedPtr_AvatarGearManager; //0x1970
    char pad_1978[8]; //0x1978
    SharedPtrNew<AvatarConsumablesManager>* shared_AvatarConsumablesManager; //0x1980
    char pad_1988[72]; //0x1988

    // @helper_functions
    static GamePlaySettings* GetSingleton() { return *(GamePlaySettings**)0x1451B4C58; }
}; //Size: 0x19D0
assert_sizeof(GamePlaySettings, 0x19D0);
