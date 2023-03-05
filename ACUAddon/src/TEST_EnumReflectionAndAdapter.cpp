
#include "Enum_BindableKeyCode_Keyboard.h"


#define SOME_ENUM(XX) \
    XX(INVALID_FROM_STRING, = -1) \
    XX(K_NONE, = 0) \
    XX(K_ESCAPE, = 1) \
    XX(K_1, = 2) \
    XX(K_2, = 3) \
    XX(K_3, = 4) \
    XX(K_4, = 5)
DECLARE_ENUM(SomeEnum, SOME_ENUM)
#define SOME_OTHER_ENUM(XX) \
    XX(INVALID_FROM_STRING, = -1) \
    XX(K_NONE, = 0) \
    XX(K_ESCAPE, = 1) \
    XX(K_1, = 2) \
    XX(K_2, = 3) \
    XX(K_3, = 4) \
    XX(K_4, = 5)
DECLARE_ENUM(SomeOtherEnum, SOME_OTHER_ENUM)

void f()
{
    constexpr const char* s11 = enum_reflection<BindableKeyCode_Keyboard>::GetString(BindableKeyCode_Keyboard::K_1);
    constexpr const char* s1 = enum_reflection<SomeEnum>::GetString(SomeEnum::K_1);
    constexpr const char* s2 = enum_reflection<SomeEnum>::GetString((SomeEnum)2);
    constexpr const char* s3_invalidToString = enum_reflection<SomeEnum>::GetString((SomeEnum)5555);
    constexpr bool testOfInvalidTranslation = s3_invalidToString == INVALID_ENUM_TO_STRING;
    constexpr SomeEnum val1 = enum_reflection<SomeEnum>::GetValue("K_NONE");
    constexpr SomeEnum val2 = enum_reflection<SomeEnum>::GetValue("K_2");
    constexpr SomeEnum val3_invalidFromString = enum_reflection<SomeEnum>::GetValue("K_5555");
}
template<typename EnumType>
struct enum_reflection;

#include "Serialization/Serialization.h"
#include "Serialization/EnumAdapter.h"
void f2()
{
    JSON cfg;
    BindableKeyCode_Keyboard autowalkButton = BindableKeyCode_Keyboard::K_B;
    READ_JSON_VARIABLE(cfg, autowalkButton, EnumAdapter);
    WRITE_JSON_VARIABLE(cfg, autowalkButton, EnumAdapter);
}
