#pragma once

class InputContainer;
class InputContainerBig;

class HasInputContainers_10
{
public:
    char pad_0000[8]; //0x0000
    InputContainer* inputContainer; //0x0008
    InputContainerBig* inputContainerBig; //0x0010
    char pad_0018[560]; //0x0018
}; //Size: 0x0248
assert_offsetof(HasInputContainers_10, inputContainer, 8);
assert_offsetof(HasInputContainers_10, inputContainerBig, 0x10);
class HasInputContainers
{
public:
    // @members
    char pad_0000[16]; //0x0000
    HasInputContainers_10* p_10; //0x0010
    char pad_0018[80]; //0x0018

    // @helper_functions
    static HasInputContainers* GetSingleton() { return *(HasInputContainers**)0x14525BEC8; }
}; //Size: 0x0068
assert_offsetof(HasInputContainers, p_10, 0x10);
assert_sizeof(HasInputContainers, 0x68);
