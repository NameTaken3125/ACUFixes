#include "pch.h"
#include "Serialization/enumFactory.h"


#define SOME_ENUM(XX) \
    XX(K_NONE, = 0) \
    XX(K_ESCAPE, = 1) \
    XX(K_1, = 2) \
    XX(K_2, = 3) \
    XX(K_3, = 4) \
    XX(K_4, = 5)
DECLARE_ENUM(SomeEnum, SOME_ENUM)
// Identical to the first - there should be no name collisions since started using the `enum class`.
#define SOME_OTHER_ENUM(XX) \
    XX(K_NONE, = 0) \
    XX(K_ESCAPE, = 1) \
    XX(K_1, = 2) \
    XX(K_2, = 3) \
    XX(K_3, = 4) \
    XX(K_4, = 5)
DECLARE_ENUM(SomeOtherEnum, SOME_OTHER_ENUM)

void TEST_EnumFactoryStaticReflection()
{
    constexpr const char* s11 = enum_reflection<SomeEnum>::GetString(SomeEnum::K_1);
    constexpr const char* s1 = enum_reflection<SomeEnum>::GetString(SomeEnum::K_1);
    constexpr const char* s2 = enum_reflection<SomeEnum>::GetString((SomeEnum)2);
    constexpr const char* s3_invalidToString = enum_reflection<SomeEnum>::GetString((SomeEnum)5555);
    constexpr bool testOfInvalidTranslation = s3_invalidToString == INVALID_ENUM_TO_STRING;
    constexpr SomeEnum val1 = enum_reflection<SomeEnum>::GetValue("K_NONE");
    constexpr SomeEnum val2 = enum_reflection<SomeEnum>::GetValue("K_2");
    constexpr SomeEnum val3_invalidFromString = enum_reflection<SomeEnum>::GetValue("K_5555");

    constexpr auto numItems = enum_reflection<SomeEnum>::GetNumItems();
    constexpr auto allStrings = enum_reflection<SomeEnum>::GetAllStrings();
    using allStringsType = decltype(allStrings);
    constexpr auto allValues = enum_reflection<SomeEnum>::GetAllValues();
    using allValuesType = decltype(allValues);
    constexpr auto allPairs = enum_reflection<SomeEnum>::GetAllPairs();
    using allPairsType = decltype(allPairs);
}

#include "Serialization/Serialization.h"
#include "Serialization/EnumAdapter.h"
void TEST_EnumAdapter()
{
    JSON cfg;
    SomeEnum autowalkButton = SomeEnum::K_4;
    READ_JSON_VARIABLE(cfg, autowalkButton, EnumAdapter);
    WRITE_JSON_VARIABLE(cfg, autowalkButton, EnumAdapter);
}
