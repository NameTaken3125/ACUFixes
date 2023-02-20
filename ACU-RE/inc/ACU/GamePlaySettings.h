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

class GamePlaySettings : public MandatoryUniverseComponent
{
public:
    // @members
    char pad_0010[5032]; //0x0010
    SkillSettings skillSettings; //0x13B8
    char pad_1570[8]; //0x1570
    BallisticProjectileAimingControlsSettings ballisticProjectileAimingControlsSettings; //0x1578
    char pad_1598[24]; //0x1598
    ExplosionSettings explosionSettings; //0x15B0
    CommunicationToolsSettings communicationToolsSettings; //0x15E0
    char pad_1640[912]; //0x1640

    // @helper_functions
    static GamePlaySettings* GetSingleton() { return *(GamePlaySettings**)0x1451B4C58; }
}; //Size: 0x19D0
assert_sizeof(GamePlaySettings, 0x19D0);
