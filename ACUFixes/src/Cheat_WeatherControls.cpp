#include "pch.h"

#include "ACU/WeatherControls.h"
#include "ACU/TimeOfDayManager.h"
#include "ACU/Season.h"

#include "ACU_DefineNativeFunction.h"

class ActionWeatherWindSelection;
DEFINE_GAME_FUNCTION(ActionWeatherWindSelection__SetWeather_mb, 0x141EC10E0, void, __fastcall, (ActionWeatherWindSelection* a1, char a2));

class Fake_ActionWeatherWindSelection
{
public:
    char pad0[0x40] = { 0 };

    /*
    If using the Action sliders, flt_44 and flt_48 need to be changed simultaneously for nonungly nonflickering nonconfusing results
    When combined, and with byte_68 and byte_69 enabled, they are the "cloudiness+rain" slider.
    If the value in [0, 0.8), there is no rain + cannot edit cloudiness directly.
    The range of [0.8, 1] controls the rain strength + the cloudiness can be controlled directly.
    */

    uint32 dword_40; //0x0040
    float flt_44; //0x0044 // cloudiness+rain TO
    float flt_48; //0x0048 // cloudiness+rain FROM
    float flt_4C; //0x004C // cloudiness+rain transition duration 1
    float flt_50; //0x0050
    float flt_54; //0x0054 // highest dryest spots wetness (includes the player's wetness)
    float flt_58; //0x0058 // lowest wettest spots wetness (puddles size)
    float flt_5C; //0x005C // wind strength
    float flt_60; //0x0060 // cloudiness+rain transition duration 2
    float flt_64; //0x0064
    uint8 byte_68; //0x0068 // doControlCloudinessPlusRain_1
    uint8 byte_69; //0x0069 // doControlCloudinessPlusRain_2
    uint8 byte_6A; //0x006A
    uint8 byte_6B; //0x006B // use flt_54==doControlDrySpots
    uint8 byte_6C; //0x006C // use flt_58==doControlPuddles
    uint8 byte_6D; //0x006D
    uint8 byte_6E; //0x006E
    uint8 byte_6F; //0x006F // use flt_5C==windStrength (flags and vegetation)
    uint8 byte_70; //0x0070 // use flt_64
    uint8 byte_71; //0x0071
    uint8 byte_72; //0x0072
    uint8 byte_73; //0x0073
    uint8 byte_74; //0x0074
    uint8 byte_75; //0x0075
    uint8 byte_76; //0x0076
    uint8 byte_77; //0x0077
    float flt_78; //0x0078 // prev flt_5C
    float flt_7C; //0x007C
    float flt_80; //0x0080 // prev flt_64
    float flt_84; //0x0084
    float flt_88; //0x0088 // prev flt_58
    uint32 dword_8C; //0x008C
}; //Size: 0x0090
assert_sizeof(Fake_ActionWeatherWindSelection, 0x90);
void Weather_Clear()
{
    Fake_ActionWeatherWindSelection fakeWeatherSetter;
    fakeWeatherSetter.dword_40 = 1;
    fakeWeatherSetter.flt_44 = 0;
    fakeWeatherSetter.flt_48 = 0;
    fakeWeatherSetter.flt_4C = 0;
    fakeWeatherSetter.flt_50 = 10.0f;
    fakeWeatherSetter.flt_54 = 0;
    fakeWeatherSetter.flt_58 = 0;
    fakeWeatherSetter.flt_5C = 0.1f;
    fakeWeatherSetter.flt_60 = 0;
    fakeWeatherSetter.flt_64 = 10.000f;
    fakeWeatherSetter.byte_68 = 1;
    fakeWeatherSetter.byte_69 = 1;
    fakeWeatherSetter.byte_6A = 1;
    fakeWeatherSetter.byte_6B = 1;
    fakeWeatherSetter.byte_6C = 1;
    fakeWeatherSetter.byte_6D = 0;
    fakeWeatherSetter.byte_6E = 1;
    fakeWeatherSetter.byte_6F = 1;
    fakeWeatherSetter.byte_70 = 0;
    fakeWeatherSetter.byte_71 = 0;
    fakeWeatherSetter.byte_72 = 0;
    fakeWeatherSetter.byte_73 = 0;
    fakeWeatherSetter.byte_74 = 0;
    fakeWeatherSetter.byte_75 = 0;
    fakeWeatherSetter.byte_76 = 0;
    fakeWeatherSetter.byte_77 = 0;
    fakeWeatherSetter.flt_78 = 0;
    fakeWeatherSetter.flt_7C = 0;
    fakeWeatherSetter.flt_80 = 0;
    fakeWeatherSetter.flt_84 = 0;
    fakeWeatherSetter.flt_88 = 0;
    fakeWeatherSetter.dword_8C = 1;
    ActionWeatherWindSelection__SetWeather_mb((ActionWeatherWindSelection*)&fakeWeatherSetter, 1);
}
void Weather_ModeratelyCloudy()
{
    // When in paris, this removes rain and sets moderately clear sky.

    Fake_ActionWeatherWindSelection fakeWeatherSetter;
    fakeWeatherSetter.dword_40 = 1;
    fakeWeatherSetter.flt_44 = 0.7179999948f;
    fakeWeatherSetter.flt_48 = 0.7179999948f;
    fakeWeatherSetter.flt_4C = 0;
    fakeWeatherSetter.flt_50 = 10.0f;
    fakeWeatherSetter.flt_54 = 0;
    fakeWeatherSetter.flt_58 = 0;
    fakeWeatherSetter.flt_5C = 0.1f;
    fakeWeatherSetter.flt_60 = 0.0f;
    fakeWeatherSetter.flt_64 = 10.0f;
    fakeWeatherSetter.byte_68 = 1;
    fakeWeatherSetter.byte_69 = 1;
    fakeWeatherSetter.byte_6A = 1;
    fakeWeatherSetter.byte_6B = 1;
    fakeWeatherSetter.byte_6C = 1;
    fakeWeatherSetter.byte_6D = 0;
    fakeWeatherSetter.byte_6E = 1;
    fakeWeatherSetter.byte_6F = 1;
    fakeWeatherSetter.byte_70 = 0;
    fakeWeatherSetter.byte_71 = 0;
    fakeWeatherSetter.byte_72 = 0;
    fakeWeatherSetter.byte_73 = 1;
    fakeWeatherSetter.byte_74 = 1;
    fakeWeatherSetter.byte_75 = 0;
    fakeWeatherSetter.byte_76 = 0;
    fakeWeatherSetter.byte_77 = 0;
    fakeWeatherSetter.flt_78 = 0.3f;
    fakeWeatherSetter.flt_7C = 0.81f;
    fakeWeatherSetter.flt_80 = 0;
    fakeWeatherSetter.flt_84 = 0.4f;
    fakeWeatherSetter.flt_88 = 0.2f;
    fakeWeatherSetter.dword_8C = 1;

    ActionWeatherWindSelection__SetWeather_mb((ActionWeatherWindSelection*)&fakeWeatherSetter, 1);
}
void Weather_RainSmugglersParadise()
{
    Fake_ActionWeatherWindSelection fakeWeatherSetter;
    fakeWeatherSetter.dword_40 = 1;
    fakeWeatherSetter.flt_44 = 0.850f;
    fakeWeatherSetter.flt_48 = 0.850f;
    fakeWeatherSetter.flt_4C = 0;
    fakeWeatherSetter.flt_50 = 10.0f;
    fakeWeatherSetter.flt_54 = 0.600f;
    fakeWeatherSetter.flt_58 = 0.800f;
    fakeWeatherSetter.flt_5C = 0.350f;
    fakeWeatherSetter.flt_60 = 0.0f;
    fakeWeatherSetter.flt_64 = 10.0f;
    fakeWeatherSetter.byte_68 = 1;
    fakeWeatherSetter.byte_69 = 1;
    fakeWeatherSetter.byte_6A = 1;
    fakeWeatherSetter.byte_6B = 1;
    fakeWeatherSetter.byte_6C = 1;
    fakeWeatherSetter.byte_6D = 1;
    fakeWeatherSetter.byte_6E = 1;
    fakeWeatherSetter.byte_6F = 1;
    fakeWeatherSetter.byte_70 = 0;
    fakeWeatherSetter.byte_71 = 0;
    fakeWeatherSetter.byte_72 = 0;
    fakeWeatherSetter.byte_73 = 0;
    fakeWeatherSetter.byte_74 = 0;
    fakeWeatherSetter.byte_75 = 0;
    fakeWeatherSetter.byte_76 = 0;
    fakeWeatherSetter.byte_77 = 0;
    fakeWeatherSetter.flt_78 = 0;
    fakeWeatherSetter.flt_7C = 0;
    fakeWeatherSetter.flt_80 = 0;
    fakeWeatherSetter.flt_84 = 0;
    fakeWeatherSetter.flt_88 = 0;
    fakeWeatherSetter.dword_8C = 0;

    ActionWeatherWindSelection__SetWeather_mb((ActionWeatherWindSelection*)&fakeWeatherSetter, 1);
}
void Weather_RainBellecFight()
{
    Fake_ActionWeatherWindSelection fakeWeatherSetter;
    fakeWeatherSetter.dword_40 = 1;
    fakeWeatherSetter.flt_44 = 1.0f;
    fakeWeatherSetter.flt_48 = 1.0f;
    fakeWeatherSetter.flt_4C = 0;
    fakeWeatherSetter.flt_50 = 10.0f;
    fakeWeatherSetter.flt_54 = 0.8f;
    fakeWeatherSetter.flt_58 = 1.0f;
    fakeWeatherSetter.flt_5C = 0.350f;
    fakeWeatherSetter.flt_60 = 0.0f;
    fakeWeatherSetter.flt_64 = 1.0f;
    fakeWeatherSetter.byte_68 = 1;
    fakeWeatherSetter.byte_69 = 1;
    fakeWeatherSetter.byte_6A = 1;
    fakeWeatherSetter.byte_6B = 1;
    fakeWeatherSetter.byte_6C = 1;
    fakeWeatherSetter.byte_6D = 1;
    fakeWeatherSetter.byte_6E = 1;
    fakeWeatherSetter.byte_6F = 1;
    fakeWeatherSetter.byte_70 = 1;
    fakeWeatherSetter.byte_71 = 0;
    fakeWeatherSetter.byte_72 = 0;
    fakeWeatherSetter.byte_73 = 0;
    fakeWeatherSetter.byte_74 = 0;
    fakeWeatherSetter.byte_75 = 0;
    fakeWeatherSetter.byte_76 = 0;
    fakeWeatherSetter.byte_77 = 0;
    fakeWeatherSetter.flt_78 = 0;
    fakeWeatherSetter.flt_7C = 1.0f;
    fakeWeatherSetter.flt_80 = 0;
    fakeWeatherSetter.flt_84 = 20.0f;
    fakeWeatherSetter.flt_88 = 0;
    fakeWeatherSetter.dword_8C = 0;

    ActionWeatherWindSelection__SetWeather_mb((ActionWeatherWindSelection*)&fakeWeatherSetter, 1);
}
void LetTheWeatherGo()
{
    auto& weather = WeatherControls::GetSingleton();
    weather.semaphoreMb_dontRerandomizeCloudiness = 0;
    weather.semaphoreMb_freezeWind = 0;
    weather.int70_needsToBeLe0ForRain = 0;
    weather.semaphoreMb_groundWetness = 0;
}


DEFINE_GAME_FUNCTION(TimeOfDayManager__SetCurrentSeason_mb, 0x141E2F660, void, __fastcall, (TimeOfDayManager* a1, unsigned int seasonID));
void SetTimeOfDay(SeasonID seasonID)
{
    TimeOfDayManager* todMan = TimeOfDayManager::GetSingleton();
    if (!todMan) { return; }
    TimeOfDayManager__SetCurrentSeason_mb(todMan, (unsigned int)seasonID);
}


enum class WorldSection
{
    Paris = 1,
    Versailles = 2,
    Franciade = 3,
};
enum class KnownTimeOfDayHandles : uint64
{
    Paris_Morning = 0x7F5F1BB41,
    Paris_Day = 0x7F5F1BC0B,
    Paris_Evening = 0x7F5F1BC07,
    Paris_Night = 0x7F5F1BC03,
    Versailles_Morning = 0x10CDE128B3,
    Versailles_Day = 0x10CDE128BF,
    Versailles_Evening = 0x10CDE128C5,
    Versailles_Night = 0x10CDE128B9,
    Franciade_Dayish = 0x12F8374554,
    Franciade_Nightish = 0x13786FCCA1,

    OccupiedParis_Unknown0 = 0x61765A1F0,
    OccupiedParis_Night = 0x110E816873,
};
std::array<KnownTimeOfDayHandles, 4> g_KnownSeasons_Paris = {
    KnownTimeOfDayHandles::Paris_Morning,
    KnownTimeOfDayHandles::Paris_Day,
    KnownTimeOfDayHandles::Paris_Evening,
    KnownTimeOfDayHandles::Paris_Night,
};
std::array<KnownTimeOfDayHandles, 4> g_KnownSeasons_Versailles = {
    KnownTimeOfDayHandles::Versailles_Morning,
    KnownTimeOfDayHandles::Versailles_Day,
    KnownTimeOfDayHandles::Versailles_Evening,
    KnownTimeOfDayHandles::Versailles_Night,
};
std::array<KnownTimeOfDayHandles, 2> g_KnownSeasons_Franciade = {
    KnownTimeOfDayHandles::Franciade_Dayish,
    KnownTimeOfDayHandles::Franciade_Nightish,
};
std::optional<WorldSection> WhereIsPlayerNow()
{
    auto* todMan = TimeOfDayManager::GetSingleton();
    if (!todMan) { return {}; }
    auto* currentSeason = todMan->shared_currentSeason;
    if (!currentSeason) { return {}; }
    uint64 currentSeasonHandle = currentSeason->handle;
    for (KnownTimeOfDayHandles knownSeason : g_KnownSeasons_Paris)
    {
        if (currentSeasonHandle == (uint64)knownSeason)
        {
            return WorldSection::Paris;
        }
    }
    for (KnownTimeOfDayHandles knownSeason : g_KnownSeasons_Versailles)
    {
        if (currentSeasonHandle == (uint64)knownSeason)
        {
            return WorldSection::Versailles;
        }
    }
    for (KnownTimeOfDayHandles knownSeason : g_KnownSeasons_Franciade)
    {
        if (currentSeasonHandle == (uint64)knownSeason)
        {
            return WorldSection::Franciade;
        }
    }
    return {};
}

#include "ImGuiCTX.h"
void DrawWeatherControls()
{
    std::optional<WorldSection> recognizedCurrentLocation = WhereIsPlayerNow();
    if (!recognizedCurrentLocation)
    {
        ImGui::Text(
            "You're in unsupported location."
            "\nThe weather sliders might have an effect or might not."
        );
        WeatherControls& weatherSettings = WeatherControls::GetSingleton();
        if (ImGui::SliderFloat("Cloudiness, kind of", &weatherSettings.cloudinessCurrent, 0.0f, 1.0f))
        {
            weatherSettings.cloudinessFrom = weatherSettings.cloudinessTo = weatherSettings.cloudinessCurrent;
        }
        if (ImGui::SliderFloat("Rain", &weatherSettings.rainStrengthCurrent, 0.0f, 1.0f))
        {
            weatherSettings.rainStrengthFrom = weatherSettings.rainStrengthTo = weatherSettings.rainStrengthCurrent;
        }
        ImGui::SliderFloat("Dry spots", &weatherSettings.drySpotsWetness, 0.0f, 1.0f);
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Wetness of dryest patches, also of player clothes");
        }
        ImGui::SliderFloat("Puddles size", &weatherSettings.puddlesSize, 0.0f, 1.0f);
        ImGui::SliderFloat("Wind, maybe", &weatherSettings.windStrength, 0.0f, 1.0f);
        return;
    }
    ImGui::Text("Time of day. Hover to read.");
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Lighting in cutscenes is tuned to a specific time of day, so I don't recommend"
            "\nchanging it during cutscenes."
            "\nAlso, changing the time of day while you're underground in Franciade"
            "\nwill have strange or bad-looking results when you come back up."
        );
    }
    ImGui::Separator();

    if (*recognizedCurrentLocation == WorldSection::Paris
        || *recognizedCurrentLocation == WorldSection::Versailles)
    {

        if (ImGui::Button("Morning"))
            SetTimeOfDay(SeasonID::Paris_Morning);
        ImGui::SameLine();
        if (ImGui::Button("Daytime"))
            SetTimeOfDay(SeasonID::Paris_Daytime);
        ImGui::SameLine();
        if (ImGui::Button("Evening"))
            SetTimeOfDay(SeasonID::Paris_Evening);
        ImGui::SameLine();
        if (ImGui::Button("Night"))
            SetTimeOfDay(SeasonID::Paris_Night);

        ImGui::Separator();
        ImGui::Text("Weather");
        ImGui::Separator();

        if (ImGui::Button("Clear sky"))
            Weather_Clear();
        ImGui::SameLine();
        if (ImGui::Button("Moderately cloudy"))
            Weather_ModeratelyCloudy();
        ImGui::SameLine();
        if (ImGui::Button("Light rain"))
            Weather_RainSmugglersParadise();
        ImGui::SameLine();
        if (ImGui::Button("Heavy rain"))
            Weather_RainBellecFight();
        if (ImGui::Button("Let the weather run its course"))
            LetTheWeatherGo();


        ImGui::Separator();
        if (ImGuiCTX::TreeNode _finerControls{ "Finer controls" })
        {
            //ImGui::Text(
            //    "If you quickly flick the Cloudiness slider left then right, maybe several times,"
            //    "\nyou may end up with a weather that's actually very sunny instead of maximum cloudy."
            //    "\nThis is a bug, but can give you a lovely golden sunlight in the evenings."
            //);
            WeatherControls& weatherSettings = WeatherControls::GetSingleton();
            if (ImGui::SliderFloat("Cloudiness, kind of", &weatherSettings.cloudinessCurrent, 0.0f, 1.0f))
            {
                weatherSettings.cloudinessFrom = weatherSettings.cloudinessTo = weatherSettings.cloudinessCurrent;
            }
            if (ImGui::SliderFloat("Rain", &weatherSettings.rainStrengthCurrent, 0.0f, 1.0f))
            {
                weatherSettings.rainStrengthFrom = weatherSettings.rainStrengthTo = weatherSettings.rainStrengthCurrent;
            }
            ImGui::SliderFloat("Dry spots", &weatherSettings.drySpotsWetness, 0.0f, 1.0f);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Wetness of dryest patches, also of player clothes");
            }
            ImGui::SliderFloat("Puddles size", &weatherSettings.puddlesSize, 0.0f, 1.0f);
            ImGui::SliderFloat("Wind", &weatherSettings.windStrength, 0.0f, 1.0f);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Wind strength (only affects flags and vegetation)");
            }
        }
    }
    else if (*recognizedCurrentLocation == WorldSection::Franciade)
    {
        if (ImGui::Button("Franciade day"))
            SetTimeOfDay(SeasonID::Franciade_0xC_orangeishDay);
        ImGui::SameLine();
        if (ImGui::Button("Franciade night"))
            SetTimeOfDay(SeasonID::Franciade_0x13_greyishNight);
        ImGui::Separator();

        WeatherControls& weatherSettings = WeatherControls::GetSingleton();
        if (ImGui::SliderFloat("Cloudiness, kind of", &weatherSettings.cloudinessCurrent, 0.0f, 1.0f))
        {
            weatherSettings.cloudinessFrom = weatherSettings.cloudinessTo = weatherSettings.cloudinessCurrent;
        }
        if (ImGui::SliderFloat("Rain", &weatherSettings.rainStrengthCurrent, 0.0f, 1.0f))
        {
            weatherSettings.rainStrengthFrom = weatherSettings.rainStrengthTo = weatherSettings.rainStrengthCurrent;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Rain doesn't have visuals in Franciade.");
        }
        ImGui::SliderFloat("Humidity", &weatherSettings.drySpotsWetness, 0.0f, 1.0f);
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Wetness of dryest patches, also of player clothes");
        }
        ImGui::SliderFloat("Puddles size", &weatherSettings.puddlesSize, 0.0f, 1.0f);
    }
}
