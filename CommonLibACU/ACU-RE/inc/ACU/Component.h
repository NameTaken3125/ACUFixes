#pragma once

#include "ManagedObject.h"

class Entity;

class BaseComponent : public ManagedObject
{
public:
}; //Size: 0x0010
assert_sizeof(BaseComponent, 0x10);

class Component : public BaseComponent
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
    virtual void Unk100_ResetScale_mb();

    // @members
    Entity* entity; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(Component, 0x20);
