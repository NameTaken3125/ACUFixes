#pragma once

#include "vmath/vmath.h"

#include "Component.h"
#include "SharedPtr.h"

class PlayerCameraComponent : public Component
{
public:
    char pad_0020[48]; //0x0020
    Vector4f positionLookFrom; //0x0050
    Vector4f quaternion_mb; //0x0060
    float fov_mb_pi_4; //0x0070
    char pad_0074[172]; //0x0074
}; //Size: 0x0120
assert_sizeof(PlayerCameraComponent, 0x120);

class CameraData;
class CameraSelectorBlenderNode;
class ACUPlayerCameraComponent_9C0;

class ACUPlayerCameraComponent : public PlayerCameraComponent
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
    virtual void Unk168();
    virtual void Unk170();
    virtual void Unk178();
    virtual void Unk180();
    virtual void Unk188();
    virtual void Unk190();
    virtual void Unk198();
    virtual void Unk1A0();
    virtual void Unk1A8();
    virtual void Unk1B0();
    virtual void Unk1B8();
    virtual void Unk1C0();
    virtual void Unk1C8();
    virtual void Unk1D0();
    virtual void Unk1D8();
    virtual void Unk1E0();
    virtual void Unk1E8();
    virtual void Unk1F0();
    virtual void Unk1F8();
    virtual void Unk200();
    virtual void Unk208();
    virtual void Unk210();
    virtual void Unk218();
    virtual void Unk220();
    virtual void Unk228();
    virtual void Unk230();
    virtual void Unk238();
    virtual void Unk240();
    virtual void Unk248();
    virtual void Unk250();
    virtual void Unk258();
    virtual void Unk260();
    virtual void Unk268();
    virtual void Unk270();
    virtual void Unk278();
    virtual void Unk280();
    virtual void Unk288_ApplyCameraFX(__m128* p_cameraPosInOut, __m128* p_cameraQuatInOut, float* p_cameraFOVInOut);

    // @members
    char pad_0120[1936]; //0x0120
    Vector4f locationMovetoward_mb; //0x08B0
    Vector4f quat_8C0; //0x08C0
    float fovPrecalc; //0x08D0
    char pad_08D4[12]; //0x08D4
    Vector4f location_8E0; //0x08E0
    char pad_08F0[8]; //0x08F0
    CameraData* cameraData; //0x08F8
    char pad_0900[76]; //0x0900
    float spinaroundAngleZcurrent; //0x094C
    float spinaroundAngleZtarget; //0x0950
    float spinaroundAngleUpDownCurrent; //0x0954
    float spinaroundAngleUpDownTarget; //0x0958
    float distFromSpinaround_mb; //0x095C
    char pad_0960[84]; //0x0960
    uint8 disableCameraSmoothingForThisFrame; //0x09B4
    char pad_09B5[3]; //0x09B5
    SharedPtrNew<CameraSelectorBlenderNode>* currentCameraSelectorBlenderNode; //0x09B8
    ACUPlayerCameraComponent_9C0* cameraCurrentBlending_mb_9c0; //0x09C0
    char pad_09C8[136]; //0x09C8
    Vector4f kindOfMovingAheadOfCameraLookat; //0x0A50
    Vector4f lagsBehindSpinaround_A60; //0x0A60
    Vector4f locationSpinaroundCanBeOffcenter; //0x0A70
    Vector4f locationSpinaround_A80; //0x0A80
    Vector4f locationLookat_A90; //0x0A90
    Vector4f locationSpinaround_AA0; //0x0AA0
    char pad_0AB0[32]; //0x0AB0
    Vector4f seemsStationaryAndRelatedToNothing; //0x0AD0
    Vector4f someKindOfRaycastEndWhenJustStartedOrStoppedAiming; //0x0AE0
    char pad_0AF0[16]; //0x0AF0
    uint64 qword_b00; //0x0B00
    char pad_0B08[1992]; //0x0B08

    // @helper_functions
    static ACUPlayerCameraComponent* GetSingleton();
}; //Size: 0x12D0
assert_sizeof(ACUPlayerCameraComponent, 0x12D0);
