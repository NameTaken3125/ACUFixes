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

class GamePlaySettings : public MandatoryUniverseComponent
{
public:
    // @members
    char pad_0010[2880]; //0x0010
    FactionSettings factionSettings_B50; //0x0B50
    char pad_0BC0[2040]; //0x0BC0
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
